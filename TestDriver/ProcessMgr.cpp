
#include <Windows.h>
#include "ProcessMgr.h"
#include "DriverWrapper.h"
#include "util.h"
#include "ps.h"
#include "nt.h"
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include<boost/format.hpp>
#include <boost\bind.hpp>
#include <functional>

CProcessMgr	g_CProcessMgr;
#define QPixmap PVOID
HICON GetIconFromFile(LPCTSTR szImageFileName);
QPixmap qt_fromHICON(HICON icon);

CProcessMgr *m_ProcessMgr = NULL;

CUniqueProcess::CUniqueProcess(ULONG ProcessId, ULONG ParentProcessId,
                               ULONG64 CreateTime, bool bIs64Bit,
                               ULONG SessionId, LPCWSTR szImagePath,
                               LPCWSTR szCommandLine, LPCWSTR szCurDirectory, QIcon *Icon)
{
	m_bAlive = true;
	m_bIs64Bit = bIs64Bit;
    m_ProcessId = ProcessId;
    m_ParentProcessId = ParentProcessId;;
    m_CreateTime = CreateTime;
    m_SessionId = SessionId;
	m_ImagePath = boost::locale::conv::from_utf(szImagePath, "GBK");; //std::string ::fromWCharArray(szImagePath);
	m_CommandLine = boost::locale::conv::from_utf(szCommandLine, "GBK");// std::string ::fromWCharArray(szCommandLine);
	m_CurDirectory = boost::locale::conv::from_utf(szCurDirectory, "GBK");// std::string ::fromWCharArray(szCurDirectory);
	m_ProcessName = boost::locale::conv::from_utf(szImagePath, "GBK");// std::string ::fromWCharArray(ExtractFileName(szImagePath));
	
    m_pParentProcess = NULL;
    m_Icon = Icon;
}

CUniqueProcess::~CUniqueProcess()
{
    for(size_t i = 0;i < m_ModuleList.size(); ++i)
        delete m_ModuleList[i];
}

bool CUniqueProcess::operator==(const CUniqueProcess &key)const
{
    if (key.m_ProcessId == this->m_ProcessId && key.m_CreateTime == this->m_CreateTime)
		return true;

	return false;
}

CUniqueModule *CUniqueProcess::GetModuleFromAddress(ULONG64 BaseAddress)
{
    for(size_t i = 0; i < m_ModuleList.size(); ++i)
    {
        CUniqueModule *um = m_ModuleList[i];
        if(BaseAddress >= um->m_ImageBase && BaseAddress <= um->m_ImageBase + um->m_UniqueImage->m_ImageSize)
            return um;
    }

    return NULL;
}

std::string  CUniqueProcess::GetDisplayName(void)
{
    std::string  str = m_ProcessName;

    if (IsAMD64() && !m_bIs64Bit)
        str += (" (*32)");

    return str;
}

std::string  CUniqueProcess::GetDisplayNameWithPID(void)
{
	return (boost::format("%d %s" ) % m_ProcessId % GetDisplayName() ).str();
    /*return std::string ("#%1 %2").arg(
                std::string ::number(m_ProcessId),
                GetDisplayName());*/
}


//Timer

CDelayProcessTimer::CDelayProcessTimer(CUniqueProcess *up, int delayAction)
{
    m_UniqueProcess = up;
    m_DelayAction = delayAction;
}

void CDelayProcessTimer::TimerExpired()
{
    if(m_DelayAction == 0)
    {
        m_ProcessMgr->UnfreshProcessItem(m_UniqueProcess);
    }
    else if(m_DelayAction == 1)
    {
        m_ProcessMgr->RemoveProcessItem(m_UniqueProcess);
    }
}

//Mgr

CProcessMgr::CProcessMgr()
{
    m_ProcessMgr = this;
    m_hReadyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_PsSystemProcess = NULL;
}

CProcessMgr::~CProcessMgr()
{
    if(m_hReadyEvent != NULL)
        CloseHandle(m_hReadyEvent);
}

void CProcessMgr::Initialize(void)
{
    FillList();

    //Ready to go
    SetEvent(m_hReadyEvent);
}

void CProcessMgr::Uninitialize(void)
{
    /*for(int i = 0 ;i < (int)m_List.size(); ++i)
    {
        delete m_List[i];
    }

    for(CUniqueIconMap::iterator itor = m_UniqueIconMap.begin(); itor != m_UniqueIconMap.end(); ++itor)
    {
        delete itor->second;
    }*/
}

CUniqueProcess *CProcessMgr::Find(ULONG dwProcessId, ULONG64 CreateTime)
{
	if (m_KeyMap.empty())
		return NULL;

	CUniqueProcessKey uk(dwProcessId, CreateTime);

	CProcessKeyMap::iterator itor = m_KeyMap.find(uk);
	if (itor == m_KeyMap.end())
		return NULL;

	return itor->second;
}

CUniqueProcess *CProcessMgr::Find(ULONG dwProcessId)
{
    if (m_IdMap.empty())
		return NULL;

	CProcessIdMap::iterator itor = m_IdMap.find(dwProcessId);
	if (itor == m_IdMap.end())
		return NULL;

    //not found? get the last one
    ULONG64 largestCreateTime = 0;
    CUniqueProcess *up, *lup = NULL;
    do
    {
        up = itor->second;
        if(up->m_bAlive)
            return up;
        if(up->m_CreateTime > largestCreateTime){
            largestCreateTime = up->m_CreateTime;
            lup = up;
        }
        itor++;
    } while (itor != m_IdMap.end());

    return lup;
}

void CProcessMgr::InsertProcess(CUniqueProcess *up)
{
	std::vector<CUniqueProcess *> aa;
	aa.push_back(up);
	m_List.push_back(up);

    CUniqueProcessKey upKey(up->m_ProcessId, up->m_CreateTime);
	m_KeyMap[upKey] = up;
    m_IdMap.insert(std::pair<ULONG, CUniqueProcess *>(up->m_ProcessId, up));
}

void CProcessMgr::FillParent(CUniqueProcess *up)
{
	CUniqueProcess *parent = Find(up->m_ParentProcessId);
    if (parent)
	{
		up->m_pParentProcess = parent;
        parent->m_ChildProcesses.push_back(up);
	}
}

void CProcessMgr::FillModules(CUniqueProcess *up)
{
    //TODO:Fill modules by enum PEB->ldr...?
    UNREFERENCED_PARAMETER(up);
}

QIcon *CProcessMgr::GetImageFileIcon(std::wstring &ImageFilePath)
{
    std::wstring UpperImageFilePath = boost::to_upper_copy(ImageFilePath);

    CUniqueIconMap::iterator itor = m_UniqueIconMap.find(UpperImageFilePath);

    if (itor == m_UniqueIconMap.end())
    {
        HICON hIcon = GetIconFromFile(ImageFilePath.c_str());
        if(hIcon)
        {
           /* QPixmap map = qt_fromHICON(hIcon);
            QIcon *qIcon = new QIcon(map);
            m_UniqueIconMap[UpperImageFilePath] = qIcon;
            DestroyIcon(hIcon);*/

            //return qIcon;
        }
    }
    else
    {
        return &itor->second;
    }

    return NULL;
}

void CProcessMgr::EnumSystemModuleProc(ULONG64 ImageBase, ULONG ImageSize, LPCWSTR szImagePath, int LoadOrderIndex)
{
    UNREFERENCED_PARAMETER(ImageSize);
    UNREFERENCED_PARAMETER(ImageBase);

    if(LoadOrderIndex == 0)
    {
        std::wstring NewImagePath;
        NormalizeFilePath(szImagePath, NewImagePath);

        ULONG ParentProcsesId;
        ULONG64 CreateTime;
        BOOLEAN Is64Bit;
        ULONG SessionId;
        GetProcessBaseInfo(4, &ParentProcsesId, &CreateTime, &Is64Bit, &SessionId);

        //System process must be in Session 0
		QIcon *icon = NULL;//= GetImageFileIcon(NewImagePath);

        CUniqueProcess *up = new CUniqueProcess(4, 0, CreateTime, Is64Bit ? true : false, SessionId, NewImagePath.c_str(), L"", L"", icon);
        up->m_ProcessName = "System";

        InsertProcess(up);
        m_PsSystemProcess = up;
    }
}

void CProcessMgr::EnumProcessProc(ULONG ProcessId)
{
    if(ProcessId <= 4)
        return;

    ULONG ParentProcessId, SessionId;
    ULONG64 CreateTime;
    BOOLEAN bIs64Bit;
    std::wstring wImagePath, wCommandLine, wCurDirectory;
    if (GetProcessBaseInfo(ProcessId, &ParentProcessId, &CreateTime, &bIs64Bit, &SessionId)
            && GetProcessImagePath(ProcessId, wImagePath))
    {
        GetProcessCmdLine(ProcessId, wCommandLine);
        GetProcessCurDir(ProcessId, wCurDirectory);
        QIcon *icon = GetImageFileIcon(wImagePath);
        CUniqueProcess *up = new CUniqueProcess(
                    ProcessId, ParentProcessId,
                    CreateTime, bIs64Bit ? true : false, SessionId,
                    wImagePath.c_str(), wCommandLine.c_str(), wCurDirectory.c_str(), icon);
        InsertProcess(up);
    }
}

void CProcessMgr::AddProcessItem(CUniqueProcess * up)
{
	//通知 进程创建
}

void CProcessMgr::KillProcessItem(CUniqueProcess * up)
{
	//进程退出 通知
}

void CProcessMgr::FillList(void)
{
    //System Process
    EnumSystemModules(boost::bind(&CProcessMgr::EnumSystemModuleProc, this, _1, _2, _3, _4));

    EnumProcesses(boost::bind(&CProcessMgr::EnumProcessProc, this, _1));

	for (size_t i = 0; i < m_List.size(); ++i)
	{
		CUniqueProcess *up = m_List[i];
		FillParent(up);
	}
}
