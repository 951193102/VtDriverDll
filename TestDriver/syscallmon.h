#ifndef SYSCALLMON_H
#define SYSCALLMON_H

#pragma once


#include <Windows.h>
#include<iostream>
#include<memory>
class m_EventMgr;
#define QSharedPointer  std::shared_ptr

class CMonitorWorker 
{
public:
	CMonitorWorker();
	static void run(void *para);
	bool ParseMessage(PUCHAR data);
	void Quit(void);
	void Uninitialize(void);
	void Initialize(void);
	void start(void);
	void wait()
	{
		WaitForSingleObject(m_hQuitEvent, -1);
	}
private:
    void CallStack( std::string & data);
    void PsCreateProcess( std::string & data);
    void PsCreateThread( std::string& data);
    void PsLoadImage( std::string& data);
    void NtLoadDriver( std::string& data);
    void NtQuerySystemInfo( std::string& data);
    void NtOpenProcess( std::string& data);
    void NtOpenThread( std::string& data);
    void NtTerminateProcess( std::string &data);
    void NtAllocateVirtualMemory( std::string& data);
    void NtReadWriteVirtualMemory( std::string &data);
    void NtProtectVirtualMemory( std::string &data);
    void NtQueryVirtualMemory( std::string& data);
    void NtCreateOpenMutant( std::string& data);
    void NtCreateOpenDirectoryObject( std::string& data);
    void NtQueryDirectoryObject( std::string& data);
    void NtUserSetWindowsHook( std::string& data);
    void NtUserFindWindow( std::string& data);
    void NtUserInternalGetWindowText( std::string& data);
    void NtUserGetClassName( std::string &data);
    void FsCreateFile( std::string &data);
    void FsCloseFile( std::string &data);
    void FsReadWriteFile( std::string &data);
    void FsCreateFileMapping( std::string &data);
    void FsQueryFileInformation( std::string &data);
    void RgCreateOpenKey( std::string& data);
    void RgSetValueKey( std::string &data);
    void RgQueryValueKey( std::string& data);
    void RgQueryKey( std::string &data);
private:
	HANDLE m_threadHandle;
	HANDLE m_hQuitEvent;

};

class  CSyscallMon
{

public:

    bool Initialize(void);
    void Uninitialize(void);

private:
    HANDLE m_hMutex;

    CMonitorWorker m_MonitorWorker;
};

#endif // SYSCALLMON_H
