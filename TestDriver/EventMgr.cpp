﻿
#include <Windows.h>
#include <time.h>
#include "ProcessMgr.h"
#include "EventMgr.h"
#include "DriverWrapper.h"
#include "util.h"
#include "nt.h"
#include<boost/thread.hpp>
#include"boost/format.hpp"
#include<boost/log/trivial.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

#define USE_DL_PREFIX
#include "dlmalloc.h"

CEventMgr *m_EventMgr = NULL;
CEventMgr g_CEventMgr;

CCallStack::CCallStack()
{
    m_ReturnAddress = NULL;
    m_UniqueModule = NULL;
}

CCallStack::CCallStack(ULONG64 ReturnAddress, CUniqueModule *um)
{
    m_ReturnAddress = ReturnAddress;
    m_UniqueModule = um;
}

ULONG CCallStack::GetClosestFunctionOffset(std::string &funcName)
{
    return m_UniqueModule->FindClosestFunction(m_ReturnAddress - m_UniqueModule->m_ImageBase, funcName);
}

//event

CUniqueEvent::CUniqueEvent(CUniqueProcess *up, ULONG ThreadId, ULONG64 KeSystemTime, ULONG64 EventId)
{
    m_UniqueProcess = up;
    m_ThreadId = ThreadId;
    m_EventId = EventId;
    m_EventTime = (KeSystemTime - m_EventMgr->m_u64KeSystemTimeStart) + m_EventMgr->m_u64UnixTimeStart;
    m_KernelCallerCount = 0;
    m_UserCallerCount = 0;
}

size_t g_total;

void* CUniqueEvent::operator new(size_t size)
{
    g_total += size;
    //OutputDebugString((LPCWSTR)std::string("CRegKeyValue alloc %1 / %2 bytes\n").arg(size).arg(g_total).utf16());
    return dlmalloc(size);
}

void CUniqueEvent::operator delete(void*p)
{
    dlfree(p);
}

void CUniqueEvent::CreateCallStacks(int kernelCaller, int userCaller, ULONG64 *callers)
{
    m_CallStacks.reserve(kernelCaller + userCaller);
    for(int i = 0; i < kernelCaller; ++i) {
        ULONG64 retAddr = callers[i];
        if(NULL == retAddr)
            continue;
        m_CallStacks.emplace_back(CCallStack(retAddr, m_ProcessMgr->m_PsSystemProcess->GetModuleFromAddress(retAddr)));
        m_KernelCallerCount ++;
    }
    for(int i = 0; i < userCaller; ++i) {
        ULONG64 retAddr = callers[i + kernelCaller];
        if(NULL == retAddr)
            continue;
        m_CallStacks.emplace_back(CCallStack(retAddr, GetUniqueProcess()->GetModuleFromAddress(retAddr)));
        m_UserCallerCount ++;
    }
}

void CUniqueEvent::FixCallStacks(bool bQueryUnknownMods)
{
    for(int i = 0; i < (int)m_CallStacks.size(); ++i)
    {
        CUniqueModule *um = m_CallStacks[i].m_UniqueModule;
        if(um) continue;
        if(i < m_KernelCallerCount)
        {
            um = m_ProcessMgr->m_PsSystemProcess->GetModuleFromAddress(m_CallStacks[i].m_ReturnAddress);
            if(um)
            {
                m_CallStacks[i].m_UniqueModule = um;
                continue;
            }
        }
        else
        {
            um = GetUniqueProcess()->GetModuleFromAddress(m_CallStacks[i].m_ReturnAddress);
            if(um)
            {
                 m_CallStacks[i].m_UniqueModule = um;
                 continue;
            }
            else if(bQueryUnknownMods)
            {
                m_ModuleMgr->QueuedGetModuleInfo(GetUniqueProcess(), m_CallStacks[i].m_ReturnAddress);
                continue;
            }
        }
    }
}

std::string CUniqueEvent::GetEventName(void) const
{
    return m_EventMgr->m_EventNames[GetEventType()];
}

std::string CUniqueEvent::GetEventClassName(void) const
{
    return m_EventMgr->m_EventClassNames[GetEventClassify()];
}

//worker

CEventWorker::CEventWorker()
{

}

void CEventWorker::OnFilterRoutine(void)
{
    m_EventMgr->FilterRoutine();
}

//mgr

CEventMgr::CEventMgr()
{
    m_EventMgr = this;
    m_CaptureEnable = TRUE;
    m_DropExclude = FALSE;
    InitializeCriticalSection(&m_Lock);
    m_hReadyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_EventList.resize(100000);
}

CEventMgr::~CEventMgr()
{
    if(m_hReadyEvent != NULL)
        CloseHandle(m_hReadyEvent);

    DeleteCriticalSection(&m_Lock);
}
void CEventMgr::Initialize(void)
{
    m_EventNames[EV_ProcessCreate] = ("ProcessCreate");
    m_EventNames[EV_ProcessDestroy] = ("ProcessExit");
    m_EventNames[EV_CreateProcess] = ("CreateProcess");
    m_EventNames[EV_ThreadCreate] = ("ThreadCreate");
    m_EventNames[EV_ThreadDestroy] = ("ThreadExit");
    m_EventNames[EV_CreateThread] = ("CreateThread");
    m_EventNames[EV_LoadImage] = ("LoadImage");
    m_EventNames[EV_LoadDriver] = ("LoadDriver");
    m_EventNames[EV_EnumProcess] = ("EnumProcesses");
    m_EventNames[EV_EnumSystemModule] = ("EnumSystemModules");
    m_EventNames[EV_EnumSystemHandle] = ("EnumSystemHandles");
    m_EventNames[EV_EnumSystemObject] = ("EnumSystemObjects");
    m_EventNames[EV_OpenProcess] = ("OpenProcess");
    m_EventNames[EV_OpenThread] = ("OpenThread");
    m_EventNames[EV_TerminateProcess] = ("TerminateProcess");
    m_EventNames[EV_AllocateVirtualMemory] = ("AllocateVirtualMemory");
    m_EventNames[EV_ReadVirtualMemory] = ("ReadVirtualMemory");
    m_EventNames[EV_WriteVirtualMemory] = ("WriteVirtualMemory");
    m_EventNames[EV_ProtectVirtualMemory] = ("ProtectVirtualMemory");
    m_EventNames[EV_QueryVirtualMemory] = ("QueryVirtualMemory");
    m_EventNames[EV_CreateMutex] = ("CreateMutex");
    m_EventNames[EV_OpenMutex] = ("OpenMutex");
    m_EventNames[EV_CreateDirectoryObject] = ("CreateDirectoryObject");
    m_EventNames[EV_OpenDirectoryObject] = ("OpenDirectoryObject");
    m_EventNames[EV_QueryDirectoryObject] = ("QueryDirectoryObject");
    m_EventNames[EV_SetWindowsHook] = ("SetWindowsHook");
    m_EventNames[EV_FindWindow] = ("FindWindow");
    m_EventNames[EV_GetWindowText] = ("GetWindowText");
    m_EventNames[EV_GetWindowClass] = ("GetWindowClass");
    m_EventNames[EV_CreateFile] = ("CreateFile");
    m_EventNames[EV_CloseFile] = ("CloseFile");
    m_EventNames[EV_ReadFile] = ("ReadFile");
    m_EventNames[EV_WriteFile] = ("WriteFile");
    m_EventNames[EV_CreateFileMapping] = ("CreateFileMapping");
    m_EventNames[EV_QueryFileInformation] = ("QueryFileInformation");
    m_EventNames[EV_CreateKey] = ("RegCreateKey");
    m_EventNames[EV_OpenKey] = ("RegOpenKey");
    m_EventNames[EV_SetValueKey] = ("RegSetValueKey");
    m_EventNames[EV_QueryValueKey] = ("RegQueryValueKey");
    m_EventNames[EV_QueryKey] = ("RegQueryKey");

    m_EventClassNames[EVClass_PsNotify] = ("PsNotify");
    m_EventClassNames[EVClass_Syscall] = ("SystemService");
    m_EventClassNames[EVClass_FileSystem] = ("FileSystem");
    m_EventClassNames[EVClass_Registry] = ("Registry");

    m_FltKeyTable[FltKey_PID] = ("PID");
    m_FltKeyTable[FltKey_ProcessName] = ("Process Name");
    m_FltKeyTable[FltKey_ProcessPath] = ("Image Path");
    m_FltKeyTable[FltKey_EventPath] = ("Path");
    m_FltKeyTable[FltKey_Arch] = ("Architecture");
    m_FltKeyTable[FltKey_SessionId] = ("Session ID");
    m_FltKeyTable[FltKey_EventType] = ("Behavior");
    m_FltKeyTable[FltKey_EventClass] = ("Event Class");
    m_FltKeyTable[FltKey_BriefResult] = ("Result");

    m_FltRelTable[FltRel_Is] = ("=");
    m_FltRelTable[FltRel_IsNot] = ("!=");
    m_FltRelTable[FltRel_LargerThan] = (">=");
    m_FltRelTable[FltRel_SmallerThan] = ("<=");
    m_FltRelTable[FltRel_Contain] = ("contain");
    m_FltRelTable[FltRel_NotContain] = ("not contain");

    m_FltIncTable[0] = ("Include");
    m_FltIncTable[1] = ("Exclude");

    SyncKeSystemTime(GetKeSystemTime());

  /*  CEventWorker *worker = new CEventWorker;
    worker->moveToThread(&m_workerThread);
    connect(&m_workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(this, &CEventMgr::StartFilter, worker, &CEventWorker::OnFilterRoutine, Qt::QueuedConnection);
    m_workerThread.start();
	boost::thread(&CEventMgr::StartFilter, this);*/

    //Add basic filters, exclude current process and system process.
    CEventFilter *flt;

    flt = new CEventFilter_PID(GetCurrentProcessId(), FltRel_Is, false);
    flt->Reference();
    m_FilterList.push_back(flt);
    m_KeyFilterList[FltKey_PID].push_back(flt);

    flt = new CEventFilter_PID(4, FltRel_Is, false);
    flt->Reference();
    m_FilterList.push_back(flt);
    m_KeyFilterList[FltKey_PID].push_back(flt);
	{
#if 0
		namespace keywords = boost::log::keywords;
		namespace sinks = boost::log::sinks;
		//初始化日志
		auto file_sink = boost::log::add_file_log(
			keywords::file_name = "info_%N.log",                                        /*< file name pattern >*/
			keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
			keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
			keywords::format = "[%TimeStamp%]: %Message%"
		);
		file_sink->locked_backend()->set_file_collector(sinks::file::make_collector(
			keywords::target = "logs",        //文件夹名
			keywords::max_size = 50 * 1024 * 1024,    //文件夹所占最大空间
			keywords::min_free_space = 100 * 1024 * 1024  //磁盘最小预留空间
		));
		boost::log::add_common_attributes();
		file_sink->locked_backend()->scan_for_files();
		file_sink->locked_backend()->auto_flush(true);

		boost::log::core::get()->add_global_attribute("Scope", attrs::named_scope());
		boost::log::core::get()->add_sink(file_sink);
#endif
	}
}

void CEventMgr::Uninitialize(void)
{
    //m_workerThread.quit();
    //m_workerThread.wait();

    /*for(int i = 0;i < m_FilterList.size(); ++i)
        m_FilterList[i]->Dereference();

    ClearAllEvents();*/
}

void CEventMgr::StartParsing(void)
{
    //Ready to go
    SetEvent(m_hReadyEvent);
}

void CEventMgr::AddEvent(CUniqueEvent *ev)
{
    bool bPass = DoFilter(ev);
    if(!m_DropExclude) {
        InsertEvent(ev);
        if(bPass)
            AddEventItem(ev);
    } else {
        if(bPass){
            InsertEvent(ev);
            AddEventItem(ev);
        } else {
            delete ev;
        }
    }
}

void CEventMgr::Lock(void)
{
    EnterCriticalSection(&m_Lock);
}

void CEventMgr::Unlock(void)
{
    LeaveCriticalSection(&m_Lock);
}

void CEventMgr::LoadFilters(const CFilterList &list)
{
    for(size_t i = 0;i < FltKey_Max; ++i)
        m_KeyFilterList[i].clear();

    for(size_t i = 0;i < list.size(); ++i)
    {
        m_KeyFilterList[ (int)list[i]->GetKey() ].push_back( list[i] );
        list[i]->Reference();
    }

    //Delete all filters which is not referenced anymore
    for(size_t i = 0;i < m_FilterList.size(); ++i)
        m_FilterList[i]->Dereference();

    //Copy list
    m_FilterList = list;
}

void CEventMgr::GetFilters(CFilterList &list)
{
    list = m_FilterList;
}

void CEventMgr::AddEventItem(CUniqueEvent * ev)
{
	if (!ev)
		return;
	//所以事件派发的入口
	//GetEventTime	GetProcessName processid threadid eventid action path
	boost::posix_time::ptime(boost::posix_time::from_time_t(ev->GetEventTime()));
	auto info = (boost::format("%1%\t%2%\t%3%:%4%\t%5%\t%6%\t%7%")
		% ev->GetEventTime()
		% ev->GetProcessName()
		% ev->GetProcessId()
		% ev->GetThreadId()
		% ev->GetEventId()
		% ev->GetEventName()
		% ev->GetProcessPath()).str();
	BOOST_LOG_TRIVIAL(info) << info;
}

void CEventMgr::RefillEventItems(QEventList * evs)
{
	//刷新事件
}

void CEventMgr::FilterUpdatePercent(size_t curEvent, size_t totalEvents)
{
	//过滤事件 进度条
}

CUniqueEvent *CEventMgr::FindEventById(ULONG64 EventId)
{
    QEventList::reverse_iterator it = m_EventList.rbegin();
    if(it == m_EventList.rend())
        return NULL;

    CUniqueEvent *ev = (CUniqueEvent *)(*it);

    if(EventId > ev->GetEventId())
        return NULL;

    while(it != m_EventList.rend()){
        ev = (CUniqueEvent *)(*it);
        if(ev->GetEventId() == EventId && ev->GetEventType() != EV_ProcessCreate && ev->GetEventType() != EV_ThreadCreate)
            return ev;
        ++it;
    }
    return NULL;
}

void CEventMgr::ClearAllEvents(void)
{
    Lock();

    ClearAllDisplayingEvents();

    QEventList::iterator it = m_EventList.begin();
    while(it != m_EventList.end()){
        delete (*it);
        ++it;
    }
    m_EventList.clear();

    Unlock();
}

void CEventMgr::InsertEvent(CUniqueEvent *ev)
{
    //No need to sort
    m_EventList.push_back(ev);
}

void CEventMgr::SyncKeSystemTime(const ULONG64 KeSystemTime)
{
	m_u64UnixTimeStart = time(NULL) * 1000;
	m_u64KeSystemTimeStart = KeSystemTime;
}

bool CEventMgr::DoFilter(const CUniqueEvent *ev)
{
    int bAnyInclude[FltKey_Max] = { 0 };
    int bKeyPass[FltKey_Max] = { 0 };
    for (size_t i = 0; i < FltKey_Max; ++i)
    {
        CFilterList &list = m_KeyFilterList[i];
        for (size_t j = 0; j < list.size(); ++j)
        {
            bool bPass = list[j]->DoFilter(ev);
            bool bInclude = list[j]->m_Include;

            if (!bAnyInclude[i] && bInclude)
                bAnyInclude[i] = 1;

            if (bPass && !bInclude)
                return false;

            if (bPass && bInclude)
            {
                bKeyPass[i] = 1;
                break;
            }
        }
    }
    for (size_t i = 0; i < FltKey_Max; ++i)
    {
        if (bAnyInclude[i] && !bKeyPass[i])
            return false;
    }
    return true;
}

//Thread routine

void CEventMgr::FilterRoutine(void)
{
    Lock();

    QEventList *resultList = new QEventList;

    size_t total = m_EventList.size();
    size_t step = max(total / 100, 1);
    resultList->resize((int)total);
	auto iter = m_EventList.begin();
	int i = 0;
	for (; iter != m_EventList.end(); ++iter,++i) {
		const auto ev = *iter;
		if (DoFilter(ev))
			resultList->push_back(ev);
		if (i % step == 0)
			FilterUpdatePercent(i, total);
	}
	/* for(int i = 0;i < m_EventList.size(); ++i){
		 const auto ev = m_EventList.;
		 if(DoFilter(ev))
			 resultList->push_back(ev);
		 if(i % step == 0)
			 FilterUpdatePercent(i, total);
	 }*/

    RefillEventItems(resultList);

    Unlock();
}

void CEventMgr::OnCallStack(std::string & data)
{
    auto param = (svc_callstack_data *)data.c_str();
    auto ev = FindEventById(param->eventId);
    if(!ev)
        return;

    ev->CreateCallStacks(param->KernelCallerCount, param->UserCallerCount, param->Callers);
}

void CEventMgr::OnPsCreateProcess(std::string & data)
{
	printf("xxxxx");
    auto param = (svc_ps_create_process_data *)data.c_str();

    std::wstring NormalizedImagePath;
    NormalizeFilePath(param->ImagePath, NormalizedImagePath);

    CUniqueProcess *up = NULL;

    if (param->Create)
    {
        up = m_ProcessMgr->Find(param->ProcessId);
        if (!up) {

            //File operation is slow...?
            QIcon *pIcon = m_ProcessMgr->GetImageFileIcon(NormalizedImagePath);

            up = new CUniqueProcess(param->ProcessId, param->ParentProcessId,
                                    param->CreateTime, param->Is64Bit ? true : false,
                                    param->SessionId, NormalizedImagePath.c_str(),
                                    param->CommandLine, param->CurDirectory, pIcon);
            m_ProcessMgr->InsertProcess(up);
            m_ProcessMgr->FillParent(up);

            m_ProcessMgr->AddProcessItem(up);
        }
    } else {
        up = m_ProcessMgr->Find(param->ProcessId);
        if (up)
        {
            up->m_bAlive = false;
            m_ProcessMgr->KillProcessItem(up);
        }
    }

    if(!m_CaptureEnable)
        return;

    if(param->Create)
    {
        auto creator = m_ProcessMgr->Find(param->ParentProcessId);
        if(creator) {
            AddEvent(new CUniqueEvent_CreateProcess(
                         creator, param->ThreadId, up,
                         param->time, param->eventId));
            AddEvent(new CUniqueEvent_ProcessCreate(
                         up, param->ThreadId,
                         param->time, param->eventId));
        }
    } else {
        if (up)
        {
            AddEvent(new CUniqueEvent_ProcessDestroy(
                         up, param->ThreadId,
                         param->time, param->eventId));
        }
    }
}

void CEventMgr::OnPsCreateThread(std::string & data)
{
    if (!m_CaptureEnable)
        return;
    auto param = (svc_ps_create_thread_data *)data.c_str();

    auto up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    if(param->Create)
    {
        auto creator = m_ProcessMgr->Find(param->CurProcessId);
        if(creator)
        {
            AddEvent(new CUniqueEvent_CreateThread(
                         creator, param->CurThreadId,
                         up, param->ThreadId,
                         param->time, param->eventId));

            AddEvent(new CUniqueEvent_ThreadCreate(
                         up, param->ThreadId, creator, param->CurThreadId,
                         param->ThreadStartAddress, param->ThreadFlags,
                         param->time, param->eventId));
        }
    }
    else
    {
        AddEvent(new CUniqueEvent_ThreadDestroy(
                     up, param->ThreadId,
                     param->time, param->eventId) );
    }
}

void CEventMgr::OnPsLoadImage(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_ps_load_image_data *)data.c_str();
    auto up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    bool bIs64Bit;
    if(param->ProcessId == 4)
        bIs64Bit = IsAMD64() ? true : false;
    else
        bIs64Bit = param->ImageBase > 0xffffffff ? true : false;

    std::wstring NormalizedImagePath;
    NormalizeFilePath(param->ImagePath, NormalizedImagePath);

    auto ui = m_ModuleMgr->GetImage( NormalizedImagePath,
                                     param->ImageSize, bIs64Bit, (param->ProcessId == 4) ? true : false);

    up->m_ModuleList.push_back(new CUniqueModule(param->ImageBase, ui));
    if(param->eventId > 0)
    {
        AddEvent(new CUniqueEvent_LoadImage(
                     up, param->ThreadId,
                     NormalizedImagePath.c_str(), param->ImageBase, param->ImageSize,
                     param->time, param->eventId));
    }
}

void CEventMgr::OnNtLoadDriver(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_load_driver_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    AddEvent(new CUniqueEvent_LoadDriver(
                 up, param->ThreadId,
                 param->RegisterPath, param->ImagePath,
                 param->ResultStatus, param->time, param->eventId));
}

void CEventMgr::OnNtQuerySystemInfo(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_query_systeminfo_data *)data.c_str();
    auto up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    switch (param->QueryClass)
    {
    case SystemProcessInformation:
        AddEvent(new CUniqueEvent_EnumProcess(up, param->ThreadId, param->time, param->eventId));
        break;
    case SystemModuleInformation:
        AddEvent(new CUniqueEvent_EnumSystemModule(up, param->ThreadId, param->time, param->eventId));
        break;
    case SystemHandleInformation:
        AddEvent(new CUniqueEvent_EnumSystemHandle(up, param->ThreadId, param->time, param->eventId));
        break;
    case SystemObjectInformation:
        AddEvent(new CUniqueEvent_EnumSystemObject(up, param->ThreadId, param->time, param->eventId));
        break;
    }
}

void CEventMgr::OnNtOpenProcess(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_open_process_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    CUniqueProcess *target = m_ProcessMgr->Find(param->TargetProcessId);
    if (!up || !target)
        return;
    AddEvent(new CUniqueEvent_OpenProcess(
                 up, param->ThreadId, target,
                 param->DesiredAccess, param->ResultStatus,
                 param->time, param->eventId));
}

void CEventMgr::OnNtOpenThread(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_open_thread_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    CUniqueProcess *target = m_ProcessMgr->Find(param->TargetProcessId);
    if (!up || !target)
        return;
    AddEvent(new CUniqueEvent_OpenThread(
                 up, param->ThreadId, target, param->TargetThreadId,
                 param->DesiredAccess, param->ResultStatus,
                 param->time, param->eventId));
}

void CEventMgr::OnNtTerminateProcess(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_terminate_process_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    CUniqueProcess *target = m_ProcessMgr->Find(param->TargetProcessId);
    if (!up || !target)
        return;
    AddEvent(new CUniqueEvent_TerminateProcess(
                 up, param->ThreadId,
                 target, param->ResultStatus,
                 param->time, param->eventId));
}

void CEventMgr::OnNtAllocateVirtualMemory(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_alloc_virtual_mem_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    CUniqueProcess *target = m_ProcessMgr->Find(param->TargetProcessId);
    if (!up || !target)
        return;
    AddEvent(new CUniqueEvent_AllocateVirtualMemory(
                 up, param->ThreadId, target,
                 param->OldBaseAddress, param->OldRegionSize,
                 param->NewBaseAddress, param->NewRegionSize,
                 param->AllocationType, param->Protect,
                 param->ResultStatus, param->time, param->eventId));
}

void CEventMgr::OnNtReadWriteVirtualMemory(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_readwrite_virtual_mem_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    CUniqueProcess *target = m_ProcessMgr->Find(param->TargetProcessId);
    if (!up || !target)
        return;
    if(param->IsWrite)
        AddEvent(new CUniqueEvent_WriteVirtualMemory(
                     up, param->ThreadId, target,
                     param->BaseAddress, param->BufferSize,
                     param->ResultStatus, param->time, param->eventId));
    else
        AddEvent(new CUniqueEvent_ReadVirtualMemory(
                     up, param->ThreadId, target,
                     param->BaseAddress, param->BufferSize,
                     param->ResultStatus, param->time, param->eventId));

}

void CEventMgr::OnNtProtectVirtualMemory(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_protect_virtual_mem_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    CUniqueProcess *target = m_ProcessMgr->Find(param->TargetProcessId);
    if (!up || !target)
        return;
    AddEvent(new CUniqueEvent_ProtectVirtualMemory(
                 up, param->ThreadId, target,
                 param->OldBaseAddress, param->OldRegionSize,
                 param->NewBaseAddress, param->NewRegionSize,
                 param->OldProtect, param->NewProtect,
                 param->ResultStatus, param->time, param->eventId));
}

void CEventMgr::OnNtQueryVirtualMemory(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_query_virtual_mem_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    CUniqueProcess *target = m_ProcessMgr->Find(param->TargetProcessId);
    if (!up || !target)
        return;
    if(param->QueryClass == MemoryBasicInformationEx) {
        AddEvent(new CUniqueEvent_QueryVirtualMemory_BasicInformation(
                     up, param->ThreadId,
                     target, param->BaseAddress, &param->mbi,
                     param->ResultStatus, param->time, param->eventId));
    } else if(param->QueryClass == MemoryMappedFilenameInformation) {
        AddEvent(new CUniqueEvent_QueryVirtualMemory_MappedFileName(
                     up, param->ThreadId, target,
                     param->BaseAddress, param->MappedFileName,
                     param->ResultStatus, param->time, param->eventId));
    } else {
        AddEvent(new CUniqueEvent_QueryVirtualMemory(
                     up, param->ThreadId, target,
                     param->BaseAddress, param->QueryClass,
                     param->ResultStatus, param->time, param->eventId));
    }
}

void CEventMgr::OnNtCreateOpenMutant(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_createopen_mutant_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up )
        return;
    if(!param->IsOpen)
        AddEvent(new CUniqueEvent_CreateMutex(
                 up, param->ThreadId, param->MutexName,
                 param->DesiredAccess, param->InitialOwner ? true : false,
                 param->ResultStatus, param->time, param->eventId));
    else
        AddEvent(new CUniqueEvent_OpenMutex(
                 up, param->ThreadId, param->MutexName, param->DesiredAccess,
                     param->ResultStatus, param->time, param->eventId));
}

void CEventMgr::OnNtCreateOpenDirectoryObject(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_createopen_dirobj_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up )
        return;
    if(!param->IsOpen)
        AddEvent(new CUniqueEvent_CreateDirectoryObject(
                 up, param->ThreadId, param->ObjectName, param->DesiredAccess,
                     param->ResultStatus, param->time, param->eventId));
    else
        AddEvent(new CUniqueEvent_OpenDirectoryObject(
                 up, param->ThreadId, param->ObjectName, param->DesiredAccess,
                     param->ResultStatus, param->time, param->eventId));
}

void CEventMgr::OnNtQueryDirectoryObject(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_query_dirobj_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up )
        return;
    AddEvent(new CUniqueEvent_QueryDirectoryObject(
                 up, param->ThreadId, param->ObjectName,
                 param->ResultStatus, param->time, param->eventId));
}

void CEventMgr::OnNtUserSetWindowsHook(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_setwindowshook_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    AddEvent(new CUniqueEvent_SetWindowsHook(
                 up, param->ThreadId,
                 param->HookThreadId, param->HookType,
                 param->HookProc, param->Flags,
                 param->Module, param->ModuleName,
                 param->ResultHHook, param->time, param->eventId
                 ));
}

void CEventMgr::OnNtUserFindWindow(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_findwindow_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    AddEvent(new CUniqueEvent_FindWindow(
                 up, param->ThreadId,
                 param->HwndParent, param->HwndChild,
                 param->ClassName, param->WindowName,
                 param->ResultHwnd, param->time, param->eventId
                 ));
}

void CEventMgr::OnNtUserInternalGetWindowText(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_getwindowtext_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    AddEvent(new CUniqueEvent_GetWindowText(
                 up, param->ThreadId,
                 param->Hwnd, param->MaxCount, param->WindowName,
                 param->ResultCount, param->time, param->eventId
                 ));
}

void CEventMgr::OnNtUserGetClassName(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_nt_getwindowclass_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    AddEvent(new CUniqueEvent_GetWindowClass(
                 up, param->ThreadId,
                 param->Hwnd, param->MaxCount, param->WindowClass,
                 param->ResultCount, param->time, param->eventId
                 ));
}

void CEventMgr::OnFsCreateFile(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_fs_create_file_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    AddEvent(new CUniqueEvent_CreateFile(
                 up, param->ThreadId, param->FilePath,
                 param->DesiredAccess, param->Disposition,
                 param->Options, param->ShareAccess,
                 param->Attributes, param->ResultStatus,
                 param->time, param->eventId));
}

void CEventMgr::OnFsCloseFile(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_fs_close_file_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    AddEvent(new CUniqueEvent_CloseFile(
                 up, param->ThreadId,
                 param->FilePath, param->ResultStatus,
                 param->time, param->eventId));
}

void CEventMgr::OnFsReadWriteFile(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_fs_readwrite_file_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    if(param->IsWrite)
        AddEvent(new CUniqueEvent_WriteFile(
                     up, param->ThreadId, param->FilePath,
                     param->Length, param->ByteOffset,
                     param->ResultStatus, param->time, param->eventId));
    else
        AddEvent(new CUniqueEvent_ReadFile(
                     up, param->ThreadId,
                     param->FilePath, param->Length, param->ByteOffset,
                     param->ResultStatus, param->time, param->eventId));
}

void CEventMgr::OnFsCreateFileMapping(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_fs_createfilemapping_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    AddEvent(new CUniqueEvent_CreateFileMapping(
                 up, param->ThreadId,
                 param->FilePath, param->SyncType, param->PageProtection,
                 param->ResultStatus, param->time, param->eventId));
}

void CEventMgr::OnFsQueryFileInformation(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_fs_queryfileinformation_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    std::wstring fileNameInfo;
    if(param->AllInfo.NameInformation.FileNameLength > 0){
        fileNameInfo = std::wstring(param->AllInfo.NameInformation.FileName, param->AllInfo.NameInformation.FileNameLength/sizeof(WCHAR));
    }

    AddEvent(new CUniqueEvent_QueryFileInformation(
                 up, param->ThreadId, param->FilePath,
                 param->QueryClass, &param->AllInfo, fileNameInfo.c_str(),
                 param->ResultStatus, param->time, param->eventId));
}

void CEventMgr::OnRgCreateOpenKey(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_reg_createopenkey_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    if(param->IsOpen)
        AddEvent(new CUniqueEvent_OpenKey(
                     up, param->ThreadId,
                     param->KeyPath, param->DesiredAccess,
                     param->ResultStatus, param->time, param->eventId));
    else
        AddEvent(new CUniqueEvent_CreateKey(
                     up, param->ThreadId,
                     param->KeyPath, param->DesiredAccess,
                     param->Disposition, param->CreateOptions,
                     param->ResultStatus, param->time, param->eventId));
}

void CEventMgr::OnRgSetValueKey(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_reg_setvaluekey_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    if( (const char *)param->CopyData + param->CopySize > (const char *)param + data.size())
        return;
    std::string binaryData((const char *)param->CopyData, param->CopySize);
    AddEvent(new CUniqueEvent_SetValueKey(
                 up, param->ThreadId,
                 param->KeyPath, param->ValueName, param->DataType, param->DataSize, binaryData,
                 param->ResultStatus, param->time, param->eventId));
}

void CEventMgr::OnRgQueryValueKey(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_reg_queryvaluekey_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    if( (const char *)param->CopyData + param->CopySize > (const char *)param + data.size())
        return;
    std::string binaryData((const char *)param->CopyData, param->CopySize);
    if( param->QueryClass == KeyValueBasicInformation && binaryData.size() >= offsetof(KEY_VALUE_BASIC_INFORMATION, Name)){
        AddEvent(new CUniqueEvent_QueryValueKey_BasicInformation(
                     up, param->ThreadId,
                     param->KeyPath, param->ValueName, param->QueryLength, binaryData,
                     param->ResultStatus, param->time, param->eventId));
    } else if( param->QueryClass == KeyValueFullInformation && binaryData.size() >= offsetof(KEY_VALUE_FULL_INFORMATION, Name)) {
        AddEvent(new CUniqueEvent_QueryValueKey_FullInformation(
                     up, param->ThreadId,
                     param->KeyPath, param->ValueName, param->QueryLength, binaryData,
                     param->ResultStatus, param->time, param->eventId));
    } else if( param->QueryClass == KeyValuePartialInformation && binaryData.size() >= offsetof(KEY_VALUE_PARTIAL_INFORMATION, Data)) {
        AddEvent(new CUniqueEvent_QueryValueKey_PartialInformation(
                     up, param->ThreadId,
                     param->KeyPath, param->ValueName, param->QueryLength, binaryData,
                     param->ResultStatus, param->time, param->eventId));
    } else  {
        AddEvent(new CUniqueEvent_QueryValueKey(
                     up, param->ThreadId,
                     param->KeyPath, param->ValueName, param->QueryClass, param->QueryLength, binaryData,
                     param->ResultStatus, param->time, param->eventId));
    }
}

void CEventMgr::OnRgQueryKey(std::string & data)
{
    if(!m_CaptureEnable)
        return;
    auto param = (svc_reg_querykey_data *)data.c_str();
    CUniqueProcess *up = m_ProcessMgr->Find(param->ProcessId);
    if (!up)
        return;
    if( (const char *)param->CopyData + param->CopySize > (const char *)param + data.size())
        return;
    std::string binaryData((const char *)param->CopyData, param->CopySize);
    if( param->QueryClass == KeyBasicInformation && binaryData.size() >= offsetof(KEY_BASIC_INFORMATION, Name)) {
        AddEvent(new CUniqueEvent_QueryKey_BasicInformation(
                     up, param->ThreadId,
                     param->KeyPath, param->QueryLength, binaryData,
                     param->ResultStatus, param->time, param->eventId));
    } else if( param->QueryClass == KeyNodeInformation && binaryData.size() >= offsetof(KEY_NODE_INFORMATION, Name)) {
        AddEvent(new CUniqueEvent_QueryKey_NodeInformation(
                     up, param->ThreadId,
                     param->KeyPath, param->QueryLength, binaryData,
                     param->ResultStatus, param->time, param->eventId));
    } else if( param->QueryClass == KeyFullInformation && binaryData.size() >= offsetof(KEY_FULL_INFORMATION, Class)) {
        AddEvent(new CUniqueEvent_QueryKey_FullInformation(
                     up, param->ThreadId,
                     param->KeyPath, param->QueryLength, binaryData,
                     param->ResultStatus, param->time, param->eventId));
    } else if( param->QueryClass == KeyNameInformation && binaryData.size() >= offsetof(KEY_NAME_INFORMATION, Name)) {
        AddEvent(new CUniqueEvent_QueryKey_NameInformation(
                     up, param->ThreadId,
                     param->KeyPath, param->QueryLength, binaryData,
                     param->ResultStatus, param->time, param->eventId));
    } else if( param->QueryClass == KeyCachedInformation && binaryData.size() >= sizeof(KEY_CACHED_INFORMATION)) {
        AddEvent(new CUniqueEvent_QueryKey_CachedInformation(
                     up, param->ThreadId,
                     param->KeyPath, param->QueryLength, binaryData,
                     param->ResultStatus, param->time, param->eventId));
    } else if( param->QueryClass == KeyVirtualizationInformation && binaryData.size() >= sizeof(KEY_VIRTUALIZATION_INFORMATION)) {
        AddEvent(new CUniqueEvent_QueryKey_VirtualizationInformation(
                     up, param->ThreadId,
                     param->KeyPath, param->QueryLength, binaryData,
                     param->ResultStatus, param->time, param->eventId));
    } else {
        AddEvent(new CUniqueEvent_QueryKey(
                     up, param->ThreadId,
                     param->KeyPath, param->QueryClass, param->QueryLength, binaryData,
                     param->ResultStatus, param->time, param->eventId));
    }
}

//CUniqueEvent_WithTargetFile::CUniqueEvent_WithTargetFile(CUniqueProcess * up, ULONG ThreadId, std::wstring & filePath, ULONG ResultStatus, ULONG64 KeSystemTime, ULONG64 EventId):
//	CUniqueEvent_WithStatusResult(up, ThreadId, ResultStatus, KeSystemTime, EventId)
//{
//	m_FilePath = m_StringMgr->GetString(filePath.c_str());
//}

//CUniqueEvent_WithTargetFile::CUniqueEvent_WithTargetFile(CUniqueProcess * up, ULONG ThreadId, LPCWSTR szFilePath, ULONG ResultStatus, ULONG64 KeSystemTime, ULONG64 EventId):
//	CUniqueEvent_WithStatusResult(up, ThreadId, ResultStatus, KeSystemTime, EventId)
//{
//	m_FilePath = m_StringMgr->GetString(szFilePath);
//}
//
//CUniqueEvent_WithStatusResult::CUniqueEvent_WithStatusResult(CUniqueProcess * up, ULONG ThreadId, ULONG ResultStatus, ULONG64 KeSystemTime, ULONG64 EventId):
//	CUniqueEvent(up, ThreadId, KeSystemTime, EventId)
//{
//	m_ResultStatus = ResultStatus;
//}
