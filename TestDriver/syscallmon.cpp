#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <shlwapi.h>
#include "syscallmon.h"
#include "driverloader.h"
#include "DriverWrapper.h"
//#include "ProcessMgr.h"
#include "EventMgr.h"
//#include "ModuleMgr.h"
//#include "SLingMgr.h"
#include "util.h"
#include "Protocol.h"
#include "symloaddialog.h"
#pragma comment(lib,"shlwapi")
#define STATUS_HV_FEATURE_UNAVAILABLE    ((NTSTATUS)0xC035001EL)

//CSyscallMon *m_SyscallMon;


CMonitorWorker::CMonitorWorker()
{
	m_hQuitEvent = nullptr;
}

void CMonitorWorker::run(void *para)
{

    HANDLE hEvents[2];
    static UCHAR buf[0x4000];
	CMonitorWorker * work = (CMonitorWorker *)para;
    //setPriority(HighestPriority);

    //WaitForMultipleObjects(2, hEvents, TRUE, INFINITE);

    OVERLAPPED ovlp;
    ovlp.hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    ovlp.Internal = 0;
    ovlp.InternalHigh = 0;
    ovlp.Pointer = 0;
    ovlp.Offset = 0;
    ovlp.OffsetHigh = 0;

    while (1)
    {
		if (WaitForSingleObject(work->m_hQuitEvent, 1) == WAIT_OBJECT_0)
			break;
        memset(buf, 0, 0x4000);
        HRESULT hres = m_Driver.Read(buf, 0x4000, NULL);
		//printf("recv\n");
        if (hres == HRESULT_FROM_WIN32(ERROR_IO_PENDING) || hres == S_OK)
        {
            DWORD signal = WAIT_OBJECT_0;
            if (hres == HRESULT_FROM_WIN32(ERROR_IO_PENDING))
            {
                DWORD signal = WaitForMultipleObjects(2, hEvents, FALSE, 1000);
                if (signal == (WAIT_OBJECT_0 + 1))//time to quit
                    break;
            }

            if(signal == WAIT_OBJECT_0){
                PUCHAR dataBuf = buf + 16;
				work->ParseMessage(dataBuf);
            }
        }
        else//if driver errors, wait and Ly again
        {
			if (WaitForSingleObject(work->m_hQuitEvent, 0) == WAIT_OBJECT_0)
				break;
        }
    }

    CloseHandle(ovlp.hEvent);
}

bool CMonitorWorker::ParseMessage(PUCHAR data)
{
	bool bParsed = true;
	svc_nop_data *header = (svc_nop_data *)data;

	m_EventMgr->Lock();

	//QSharedPointer<QByteArray> ba(new QByteArray((const char *)data, header->size));
	std::string ba((char *)data,header->size);/*(new char[header->size + 1]);
	memset(ba.get(), 0, header->size);
	memcpy(ba.get(), data,header->size);*/
	switch (header->protocol) {
	case svc_callstack:
		CallStack(ba);
		break;
	case svc_ps_create_thread:
		PsCreateThread(ba);
		break;
	case svc_ps_create_process:
		PsCreateProcess(ba);
		break;
	case svc_ps_load_image:
		PsLoadImage(ba);
		break;
	case svc_nt_load_driver:
		NtLoadDriver(ba);
		break;
	case svc_nt_query_systeminfo:
		NtQuerySystemInfo(ba);
		break;
	case svc_nt_open_process:
		NtOpenProcess(ba);
		break;
	case svc_nt_open_thread:
		NtOpenThread(ba);
		break;
	case svc_nt_terminate_process:
		NtTerminateProcess(ba);
		break;
	case svc_nt_alloc_virtual_mem:
		NtAllocateVirtualMemory(ba);
		break;
	case svc_nt_readwrite_virtual_mem:
		NtReadWriteVirtualMemory(ba);
		break;
	case svc_nt_protect_virtual_mem:
		NtProtectVirtualMemory(ba);
		break;
	case svc_nt_query_virtual_mem:
		NtQueryVirtualMemory(ba);
		break;
	case svc_nt_createopen_mutant:
		NtCreateOpenMutant(ba);
		break;
	case svc_nt_createopen_dirobj:
		NtCreateOpenDirectoryObject(ba);
		break;
	case svc_nt_query_dirobj:
		NtQueryDirectoryObject(ba);
		break;
	case svc_nt_setwindowshook:
		NtUserSetWindowsHook(ba);
		break;
	case svc_nt_findwindow:
		NtUserFindWindow(ba);
		break;
	case svc_nt_getwindowtext:
		NtUserInternalGetWindowText(ba);
		break;
	case svc_nt_getwindowclass:
		NtUserGetClassName(ba);
		break;
	case svc_fs_create_file:
		FsCreateFile(ba);
		break;
	case svc_fs_close_file:
		FsCloseFile(ba);
		break;
	case svc_fs_readwrite_file:
		FsReadWriteFile(ba);
		break;
	case svc_fs_createfilemapping:
		FsCreateFileMapping(ba);
		break;
	case svc_fs_queryfileinformation:
		FsQueryFileInformation(ba);
		break;
	case svc_reg_createopenkey:
		RgCreateOpenKey(ba);
		break;
	case svc_reg_setvaluekey:
		RgSetValueKey(ba);
		break;
	case svc_reg_queryvaluekey:
		RgQueryValueKey(ba);
		break;
	case svc_reg_querykey:
		RgQueryKey(ba);
		break;
	default:
		bParsed = false;
		break;
	}

	m_EventMgr->Unlock();

	return bParsed;
}

void CMonitorWorker::Quit(void)
{
	if (m_hQuitEvent != NULL) {
		CloseHandle(m_hQuitEvent);
		m_hQuitEvent = NULL;
	}
}

void CMonitorWorker::Uninitialize(void)
{
	if (m_hQuitEvent != NULL) {
		CloseHandle(m_hQuitEvent);
		m_hQuitEvent = NULL;
	}
}

void CMonitorWorker::Initialize(void)
{
	m_hQuitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

void CMonitorWorker::start(void)
{
	m_threadHandle = CreateThread(NULL,
		0,
		(LPTHREAD_START_ROUTINE)run,
		this,
		0,
		NULL);
	CloseHandle(m_threadHandle);
}

void CMonitorWorker::CallStack(std::string & data)
{
	m_EventMgr->OnCallStack(data);
}

void CMonitorWorker::PsCreateProcess(std::string & data)
{
	m_EventMgr->OnPsCreateProcess(data);
}

void CMonitorWorker::PsCreateThread(std::string & data)
{
	m_EventMgr->OnPsCreateThread(data);
}

void CMonitorWorker::PsLoadImage(std::string & data)
{
	m_EventMgr->OnPsLoadImage(data);
}

void CMonitorWorker::NtLoadDriver(std::string & data)
{
	m_EventMgr->OnNtLoadDriver(data);
}

void CMonitorWorker::NtQuerySystemInfo(std::string & data)
{
	m_EventMgr->OnNtQuerySystemInfo(data);
}

void CMonitorWorker::NtOpenProcess(std::string & data)
{
	m_EventMgr->OnNtOpenProcess(data);
}

void CMonitorWorker::NtOpenThread(std::string & data)
{
	m_EventMgr->OnNtOpenThread(data);
}

void CMonitorWorker::NtTerminateProcess(std::string & data)
{
	m_EventMgr->OnNtTerminateProcess(data);
}

void CMonitorWorker::NtAllocateVirtualMemory(std::string & data)
{
	m_EventMgr->OnNtAllocateVirtualMemory(data);
}

void CMonitorWorker::NtReadWriteVirtualMemory(std::string & data)
{
	m_EventMgr->OnNtReadWriteVirtualMemory(data);
}

void CMonitorWorker::NtProtectVirtualMemory(std::string & data)
{
	m_EventMgr->OnNtProtectVirtualMemory(data);
}

void CMonitorWorker::NtQueryVirtualMemory(std::string & data)
{
	m_EventMgr->OnNtQueryVirtualMemory(data);
}

void CMonitorWorker::NtCreateOpenMutant(std::string & data)
{
	m_EventMgr->OnNtCreateOpenMutant(data);
}

void CMonitorWorker::NtCreateOpenDirectoryObject(std::string & data)
{
	m_EventMgr->OnNtCreateOpenDirectoryObject(data);
}

void CMonitorWorker::NtQueryDirectoryObject(std::string & data)
{
	m_EventMgr->OnNtQueryDirectoryObject(data);
}

void CMonitorWorker::NtUserSetWindowsHook(std::string & data)
{
	m_EventMgr->OnNtUserSetWindowsHook(data);
}

void CMonitorWorker::NtUserFindWindow(std::string & data)
{
	m_EventMgr->OnNtUserFindWindow(data);
}

void CMonitorWorker::NtUserInternalGetWindowText(std::string & data)
{
	m_EventMgr->OnNtUserInternalGetWindowText(data);
}

void CMonitorWorker::NtUserGetClassName(std::string & data)
{
	m_EventMgr->OnNtUserGetClassName(data);
}

void CMonitorWorker::FsCreateFile(std::string & data)
{
	m_EventMgr->OnFsCreateFile(data);
}

void CMonitorWorker::FsCloseFile(std::string & data)
{
	//
	m_EventMgr->OnFsCloseFile(data);
}

void CMonitorWorker::FsReadWriteFile(std::string & data)
{
	m_EventMgr->OnFsReadWriteFile(data);
}

void CMonitorWorker::FsCreateFileMapping(std::string & data)
{
	m_EventMgr->OnFsCreateFileMapping(data);
}

void CMonitorWorker::FsQueryFileInformation(std::string & data)
{
	m_EventMgr->OnFsQueryFileInformation(data);
}

void CMonitorWorker::RgCreateOpenKey(std::string & data)
{
	m_EventMgr->OnRgCreateOpenKey(data);
}

void CMonitorWorker::RgSetValueKey(std::string & data)
{
	m_EventMgr->OnRgSetValueKey(data);
}

void CMonitorWorker::RgQueryValueKey(std::string & data)
{
	m_EventMgr->OnRgQueryValueKey(data);
}

void CMonitorWorker::RgQueryKey(std::string & data)
{
	m_EventMgr->OnRgQueryKey(data);
}




bool CSyscallMon::Initialize(void)
{
    m_hMutex = CreateMutex(NULL, TRUE, L"SyscallMonitorMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(m_hMutex);
        MessageBox(NULL, L"Fatal Error", L"Syscall Monitor is already running!", 0);
        return false;
    }

    if (!AdjustPrivilege(SE_DEBUG_NAME))
    {
        MessageBox(NULL, L"Fatal Error", L"Failed to get SE_DEBUG_NAME privilege!", 0);
        return false;
    }

    if (!AdjustPrivilege(SE_LOAD_DRIVER_NAME))
    {
        MessageBox(NULL, L"Fatal Error", L"Failed to get SE_LOAD_DRIVER_NAME privilege!", 0);
        return false;
    }

    TCHAR szDirectory[MAX_PATH];
    GetModuleFilePath(NULL, szDirectory, MAX_PATH);

    std::wstring drvFileName = IsAMD64() ? L"SyscallMon64.sys" : L"SyscallMon32.sys";
	std::wstring drvFilePath(szDirectory);
	drvFilePath += L"\\";
	drvFilePath += drvFileName;

    if (!PathFileExists(drvFilePath.c_str()))
    {
		std::wstring err = L"Could not found ";
		err += drvFilePath;
        MessageBox(NULL, L"Fatal Error", err.c_str(), 0);
        return false;
    }

    //m_ModuleMgr->Initialize();

  /*  SymLoadDialog *symLoadDialog = new SymLoadDialog();
    if(symLoadDialog->exec() != QDialog::Accepted)
        return false;*/

    //Load driver later...
	SymLoadDialog sy;
	sy.OnLoadAllImageCompleteNotify();
	std::wstring drvSymLink(L"\\??\\");
	drvSymLink += drvFilePath;
    conn_context_data conn;
    conn.txsb = 'TXSB';
    conn.ver = 1;

    if (!m_Driver.Connect(L"\\SyscallMonPort", &conn, sizeof(conn)))
    {
        if (!m_Driver.Install((LPCWSTR)drvSymLink.c_str(), L"SyscallMon", L"SyscallMon"))
        {
            if(STATUS_HV_FEATURE_UNAVAILABLE == m_Driver.m_ErrorCode)
            {
                MessageBox(NULL, L"Fatal Error", (L"Intel VT-x/EPT is not support or not enabled in your system!"), 0);
                return false;
            }
            MessageBox(NULL, L"Fatal Error", m_Driver.m_szErrorInfo, 0);
            return false;
        }
        if (!m_Driver.Connect(L"\\SyscallMonPort", &conn, sizeof(conn)))
        {
            MessageBox(NULL, L"Fatal Error", m_Driver.m_szErrorInfo, 0);
            return false;
        }
    }
    else
    {
        wcscpy(m_Driver.m_pServiceName, L"SyscallMon");
        wcscpy(m_Driver.m_pDisplayName, L"SyscallMon");
    }

    m_MonitorWorker.Initialize();

    

    SetCaptureEnable(true);

    m_ProcessMgr->Initialize();
    m_EventMgr->Initialize();
	m_MonitorWorker.start();
    return true;
}

void CSyscallMon::Uninitialize(void)
{
    m_MonitorWorker.Quit();
	m_MonitorWorker.wait();
    m_MonitorWorker.Uninitialize();

    m_Driver.Disconnect();
    m_Driver.Uninstall();

    //m_ModuleMgr->Uninitialize();
    //m_EventMgr->Uninitialize();
    //m_ProcessMgr->Uninitialize();

    if(m_hMutex != INVALID_HANDLE_VALUE)
        CloseHandle(m_hMutex);
}
