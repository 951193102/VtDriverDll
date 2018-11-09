#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include<string>
#include "nt.h"

std::string GetNTStatusCodeString(ULONG status)
{
	switch (status)
	{
    case 0x00000000:return "STATUS_SUCCESS";
    case 0x00000102:return "STATUS_TIMEOUT";
    case 0x00000103:return "STATUS_PENDING";
    case 0x00000104:return "STATUS_REPARSE";
    case 0x00000105:return "STATUS_MORE_ENTRIES";
    case 0x0000012A:return "STATUS_FILE_LOCKED_WITH_ONLY_READERS";
    case 0x0000012B:return "STATUS_FILE_LOCKED_WITH_WRITERS";
    case 0x80000005:return "STATUS_BUFFER_OVERFLOW";
    case 0x80000006:return "STATUS_NO_MORE_FILES";
    case 0x8000000D:return "STATUS_PARTIAL_COPY";
    case 0x8000001A:return "STATUS_NO_MORE_ENTRIES";
    case 0xC0000001:return "STATUS_UNSUCCESSFUL";
    case 0xC0000002:return "STATUS_NOT_IMPLEMENTED";
    case 0xC0000003:return "STATUS_INVALID_INFO_CLASS";
    case 0xC0000004:return "STATUS_INFO_LENGTH_MISMATCH";
    case 0xC0000005:return "STATUS_ACCESS_VIOLATION";
    case 0xC0000006:return "STATUS_IN_PAGE_ERROR";
    case 0xC0000007:return "STATUS_PAGEFILE_QUOTA";
    case 0xC0000008:return "STATUS_INVALID_HANDLE";
    case 0xC0000009:return "STATUS_BAD_INITIAL_STACK";
    case 0xC000000A:return "STATUS_BAD_INITIAL_PC";
    case 0xC000000B:return "STATUS_INVALID_CID";
    case 0xC000000C:return "STATUS_TIMER_NOT_CANCELED";
    case 0xC000000D:return "STATUS_INVALID_PARAMETER";
    case 0xC000000E:return "STATUS_NO_SUCH_DEVICE";
    case 0xC000000F:return "STATUS_NO_SUCH_FILE";
    case 0xC0000010:return "STATUS_INVALID_DEVICE_REQUEST";
    case 0xC0000011:return "STATUS_END_OF_FILE";
    case 0xC0000012:return "STATUS_WRONG_VOLUME";
    case 0xC0000013:return "STATUS_NO_MEDIA_IN_DEVICE";
    case 0xC0000014:return "STATUS_UNRECOGNIZED_MEDIA";
    case 0xC0000015:return "STATUS_NONEXISTENT_SECTOR";
    case 0xC0000016:return "STATUS_MORE_PROCESSING_REQUIRED";
    case 0xC0000017:return "STATUS_NO_MEMORY";
    case 0xC0000018:return "STATUS_CONFLICTING_ADDRESSES";
    case 0xC0000019:return "STATUS_NOT_MAPPED_VIEW";
    case 0xC000001A:return "STATUS_UNABLE_TO_FREE_VM";
    case 0xC000001B:return "STATUS_UNABLE_TO_DELETE_SECTION";
    case 0xC000001C:return "STATUS_INVALID_SYSTEM_SERVICE";
    case 0xC000001D:return "STATUS_ILLEGAL_INSTRUCTION";
    case 0xC000001E:return "STATUS_INVALID_LOCK_SEQUENCE";
    case 0xC000001F:return "STATUS_INVALID_VIEW_SIZE";
    case 0xC0000020:return "STATUS_INVALID_FILE_FOR_SECTION";
    case 0xC0000021:return "STATUS_ALREADY_COMMITTED";
    case 0xC0000022:return "STATUS_ACCESS_DENIED";
    case 0xC0000023:return "STATUS_BUFFER_TOO_SMALL";
    case 0xC0000030:return "STATUS_INVALID_PARAMETER_MIX";
    case 0xC0000031:return "STATUS_INVALID_QUOTA_LOWER";
    case 0xC0000032:return "STATUS_DISK_CORRUPT_ERROR";
    case 0xC0000033:return "STATUS_OBJECT_NAME_INVALID";
    case 0xC0000034:return "STATUS_OBJECT_NAME_NOT_FOUND";
    case 0xC0000035:return "STATUS_OBJECT_NAME_COLLISION";

    case 0xC0000039:return "STATUS_OBJECT_PATH_INVALID";
    case 0xC000003A:return "STATUS_OBJECT_PATH_NOT_FOUND";
    case 0xC000003B:return "STATUS_OBJECT_PATH_SYNTAX_BAD";
		
    case 0xC0000043:return "STATUS_SHARING_VIOLATION";
    case 0xC0000044:return "STATUS_QUOTA_EXCEEDED";
    case 0xC0000045:return "STATUS_INVALID_PAGE_PROTECTION";
    case 0xC0000046:return "STATUS_MUTANT_NOT_OWNED";
    case 0xC0000047:return "STATUS_SEMAPHORE_LIMIT_EXCEEDED";
    case 0xC0000048:return "STATUS_PORT_ALREADY_SET";
    case 0xC0000049:return "STATUS_SECTION_NOT_IMAGE";
    case 0xC000004A:return "STATUS_SUSPEND_COUNT_EXCEEDED";
    case 0xC000004B:return "STATUS_THREAD_IS_TERMINATING";

    case 0xC0000054:return "STATUS_FILE_LOCK_CONFLICT";

    case 0xC0000061:return "STATUS_PRIVILEGE_NOT_HELD";

    case 0xC0000096:return "STATUS_PRIVILEGED_INSTRUCTION";

    case 0xC000009A:return "STATUS_INSUFFICIENT_RESOURCES";

    case 0xC00000EF:return "STATUS_INVALID_PARAMETER_1";
    case 0xC00000F0:return "STATUS_INVALID_PARAMETER_2";
    case 0xC00000F1:return "STATUS_INVALID_PARAMETER_3";
    case 0xC00000F2:return "STATUS_INVALID_PARAMETER_4";
    case 0xC00000F3:return "STATUS_INVALID_PARAMETER_5";

    case 0xC0000106:return "STATUS_NAME_TOO_LONG";

    case 0xC01C0001:return"STATUS_FLT_NO_HANDLER_DEFINED";
    case 0xC01C0002:return"STATUS_FLT_CONTEXT_ALREADY_DEFINED";
    case 0xC01C0003:return"STATUS_FLT_INVALID_ASYNCHRONOUS_REQUEST";
    case 0xC01C0004:return"STATUS_FLT_DISALLOW_FAST_IO";
    case 0xC01C0005:return"STATUS_FLT_INVALID_NAME_REQUEST";
    case 0xC01C0006:return"STATUS_FLT_NOT_SAFE_TO_POST_OPERATION";
    case 0xC01C0007:return"STATUS_FLT_NOT_INITIALIZED";
    case 0xC01C0008:return"STATUS_FLT_FILTER_NOT_READY";
    case 0xC01C0009:return"STATUS_FLT_POST_OPERATION_CLEANUP";
    case 0xC01C000A:return"STATUS_FLT_INTERNAL_ERROR";
    case 0xC01C000B:return"STATUS_FLT_DELETING_OBJECT";
    case 0xC01C000C:return"STATUS_FLT_MUST_BE_NONPAGED_POOL";
    case 0xC01C000D:return"STATUS_FLT_DUPLICATE_ENTRY";
	}
    return "Unknown Status Code";
}

std::string GetCreateDispositionString(ULONG CreateDisposition)
{
	switch (CreateDisposition)
	{
	case FILE_SUPERSEDE:
        return "FILE_SUPERSEDE";
	case FILE_OPEN:
        return "FILE_OPEN";
	case FILE_CREATE:
        return "FILE_CREATE";
	case FILE_OPEN_IF:
        return "FILE_OPEN_IF";
	case FILE_OVERWRITE:
        return "FILE_OVERWRITE";
	case FILE_OVERWRITE_IF:
        return "FILE_OVERWRITE_IF";
	}
    return "Unknown CreateDisposition";
}

void GetShareAccessString(ULONG ShareAccess, std::string &sL)
{
	if (ShareAccess & FILE_SHARE_READ)
	{
        if(!sL.empty())
            sL.append(" | ");
        sL.append("FILE_SHARE_READ");
	}
	if (ShareAccess & FILE_SHARE_WRITE)
	{
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_SHARE_WRITE");
	}
	if (ShareAccess & FILE_SHARE_DELETE)
	{
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_SHARE_DELETE");
	}
}

void GetCommonDesiredAccess(ULONG &DesiredAccess, std::string &sL)
{
    if (DesiredAccess & STANDARD_RIGHTS_ALL)
    {
        DesiredAccess &= ~STANDARD_RIGHTS_ALL;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("STANDARD_RIGHTS_ALL");
    }
    if (DesiredAccess & GENERIC_READ)
    {
        DesiredAccess &= ~GENERIC_READ;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("GENERIC_READ");
    }
    if (DesiredAccess & GENERIC_WRITE)
    {
        DesiredAccess &= ~GENERIC_WRITE;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("GENERIC_WRITE");
    }
    if (DesiredAccess & GENERIC_EXECUTE)
    {
        DesiredAccess &= ~GENERIC_EXECUTE;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("GENERIC_EXECUTE");
    }
    if (DesiredAccess & STANDARD_RIGHTS_REQUIRED)
    {
        DesiredAccess &= ~STANDARD_RIGHTS_REQUIRED;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("STANDARD_RIGHTS_REQUIRED");
    }
    if (DesiredAccess & MAXIMUM_ALLOWED)
    {
        DesiredAccess &= ~MAXIMUM_ALLOWED;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("MAXIMUM_ALLOWED");
    }
    if (DesiredAccess & DELETE)
    {
        DesiredAccess &= ~DELETE;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("DELETE");
    }
    if (DesiredAccess & READ_CONTROL)
    {
        DesiredAccess &= ~READ_CONTROL;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("READ_CONTROL");
    }
    if (DesiredAccess & WRITE_DAC)
    {
        DesiredAccess &= ~WRITE_DAC;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("WRITE_DAC");
    }
    if (DesiredAccess & WRITE_OWNER)
    {
        DesiredAccess &= ~WRITE_OWNER;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("WRITE_OWNER");
    }
    if (DesiredAccess & SYNCHRONIZE)
    {
        DesiredAccess &= ~SYNCHRONIZE;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("SYNCHRONIZE");
    }
	if (DesiredAccess != 0)
	{
		char para[MAX_PATH] = { 0 };
		sprintf(para, "Unknown (%0X)", DesiredAccess);
		sL = para;
	}
    //sL = QObject::L("Unknown (0x%1)").arg(FormatHexString(DesiredAccess, 0));
}

void GetCreateFileDesiredAccessString(ULONG DesiredAccess, std::string &sL)
{
    if (DesiredAccess & FILE_READ_DATA)
    {
        DesiredAccess &= ~FILE_READ_DATA;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("FILE_READ_DATA");
    }
    if (DesiredAccess & FILE_READ_ATTRIBUTES)
    {
        DesiredAccess &= ~FILE_READ_ATTRIBUTES;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_READ_ATTRIBUTES");
    }
    if (DesiredAccess & FILE_READ_EA)
    {
        DesiredAccess &= ~FILE_READ_EA;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_READ_EA");
    }
    if (DesiredAccess & FILE_WRITE_DATA)
    {
        DesiredAccess &= ~FILE_WRITE_DATA;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_WRITE_DATA");
    }
    if (DesiredAccess & FILE_WRITE_ATTRIBUTES)
    {
        DesiredAccess &= ~FILE_WRITE_ATTRIBUTES;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_WRITE_ATTRIBUTES");
    }
    if (DesiredAccess & FILE_WRITE_EA)
    {
        DesiredAccess &= ~FILE_WRITE_EA;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_WRITE_EA");
    }
    if (DesiredAccess & FILE_APPEND_DATA)
    {
        DesiredAccess &= ~FILE_APPEND_DATA;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_APPEND_DATA");
    }
    if (DesiredAccess & FILE_EXECUTE)
    {
        DesiredAccess &= ~FILE_EXECUTE;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_EXECUTE");
    }

    GetCommonDesiredAccess(DesiredAccess, sL);
}

void GetCreateFileOptionsString(ULONG Options, std::string &sL)
{
    if (Options & FILE_DIRECTORY_FILE)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_DIRECTORY_FILE");
    }
    if (Options & FILE_NON_DIRECTORY_FILE)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_NON_DIRECTORY_FILE");
    }
    if (Options & FILE_WRITE_THROUGH)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_WRITE_THROUGH");
    }
    if (Options & FILE_SEQUENTIAL_ONLY)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_SEQUENTIAL_ONLY");
    }
    if (Options & FILE_RANDOM_ACCESS)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_RANDOM_ACCESS");
    }
    if (Options & FILE_NO_INTERMEDIATE_BUFFERING)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_NO_INTERMEDIATE_BUFFERING");
    }
    if (Options & FILE_SYNCHRONOUS_IO_ALERT)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_SYNCHRONOUS_IO_ALERT");
    }
    if (Options & FILE_SYNCHRONOUS_IO_NONALERT)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_SYNCHRONOUS_IO_NONALERT");
    }
    if (Options & FILE_CREATE_TREE_CONNECTION)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_CREATE_TREE_CONNECTION");
    }
    if (Options & FILE_COMPLETE_IF_OPLOCKED)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_COMPLETE_IF_OPLOCKED");
    }
    if (Options & FILE_NO_EA_KNOWLEDGE)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_NO_EA_KNOWLEDGE");
    }
    if (Options & FILE_OPEN_REPARSE_POINT)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_OPEN_REPARSE_POINT");
    }
    if (Options & FILE_DELETE_ON_CLOSE)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_DELETE_ON_CLOSE");
    }
    if (Options & FILE_OPEN_BY_FILE_ID)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_OPEN_BY_FILE_ID");
    }
    if (Options & FILE_OPEN_FOR_BACKUP_INTENT)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_OPEN_FOR_BACKUP_INTENT");
    }
    if (Options & FILE_RESERVE_OPFILTER)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_RESERVE_OPFILTER");
    }
    if (Options & FILE_OPEN_REQUIRING_OPLOCK)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_OPEN_REQUIRING_OPLOCK");
    }
    if (Options & FILE_SESSION_AWARE)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_SESSION_AWARE");
    }
}

void GetPageProtectionString(ULONG PageProtection, std::string &sL)
{
	if (PageProtection & PAGE_EXECUTE)
	{
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PAGE_EXECUTE");
	}
	if (PageProtection & PAGE_EXECUTE_READ)
	{
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PAGE_EXECUTE_READ");
	}
	if (PageProtection & PAGE_EXECUTE_READWRITE)
	{
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PAGE_EXECUTE_READWRITE");
	}
	if (PageProtection & PAGE_WRITECOPY)
	{
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PAGE_WRITECOPY");
	}
	if (PageProtection & PAGE_READWRITE)
	{
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PAGE_READWRITE");
	}
	if (PageProtection & PAGE_READONLY)
	{
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PAGE_READONLY");
	}
	if (PageProtection & PAGE_NOACCESS)
	{
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PAGE_NOACCESS");
	}
	if (PageProtection & PAGE_GUARD)
	{
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PAGE_GUARD");
	}
	if (PageProtection & PAGE_NOCACHE)
	{
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PAGE_NOCACHE");
	}
	if (PageProtection & PAGE_WRITECOMBINE)
	{
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PAGE_WRITECOMBINE");
	}
}

void GetProcessDesiredAccessString(ULONG DesiredAccess, std::string &sL)
{
	if (DesiredAccess == PROCESS_ALL_ACCESS)
	{
        sL = "PROCESS_ALL_ACCESS";
		return;
	}
	if (DesiredAccess & PROCESS_TERMINATE)
	{
        DesiredAccess &= ~PROCESS_TERMINATE;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_TERMINATE");
	}
	if (DesiredAccess & PROCESS_CREATE_THREAD)
	{
        DesiredAccess &= ~PROCESS_CREATE_THREAD;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_CREATE_THREAD");
	}
	if (DesiredAccess & PROCESS_SET_SESSIONID)
	{
        DesiredAccess &= ~PROCESS_SET_SESSIONID;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_SET_SESSIONID");
	}
	if (DesiredAccess & PROCESS_VM_OPERATION)
	{
        DesiredAccess &= ~PROCESS_VM_OPERATION;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_VM_OPERATION");
	}
	if (DesiredAccess & PROCESS_VM_READ)
	{
        DesiredAccess &= ~PROCESS_VM_READ;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_VM_READ");
	}
	if (DesiredAccess & PROCESS_VM_WRITE)
	{
        DesiredAccess &= ~PROCESS_VM_WRITE;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_VM_WRITE");
	}
	if (DesiredAccess & PROCESS_DUP_HANDLE)
	{
        DesiredAccess &= ~PROCESS_DUP_HANDLE;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_DUP_HANDLE");
	}
	if (DesiredAccess & PROCESS_CREATE_PROCESS)
	{
        DesiredAccess &= ~PROCESS_CREATE_PROCESS;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_CREATE_PROCESS");
	}
	if (DesiredAccess & PROCESS_SET_QUOTA)
	{
        DesiredAccess &= ~PROCESS_SET_QUOTA;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_SET_QUOTA");
	}
	if (DesiredAccess & PROCESS_SET_INFORMATION)
	{
        DesiredAccess &= ~PROCESS_SET_INFORMATION;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_SET_INFORMATION");
	}
	if (DesiredAccess & PROCESS_QUERY_INFORMATION)
	{
        DesiredAccess &= ~PROCESS_QUERY_INFORMATION;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_QUERY_INFORMATION");
	}
	if (DesiredAccess & PROCESS_SUSPEND_RESUME)
	{
        DesiredAccess &= ~PROCESS_SUSPEND_RESUME;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_SUSPEND_RESUME");
	}
	if (DesiredAccess & PROCESS_QUERY_LIMITED_INFORMATION)
	{
        DesiredAccess &= ~PROCESS_QUERY_LIMITED_INFORMATION;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_QUERY_LIMITED_INFORMATION");
	}
	if (DesiredAccess & PROCESS_SET_LIMITED_INFORMATION)
	{
        DesiredAccess &= ~PROCESS_SET_LIMITED_INFORMATION;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("PROCESS_SET_LIMITED_INFORMATION");
	}

    GetCommonDesiredAccess(DesiredAccess, sL);
}

void GetThreadDesiredAccessString(ULONG DesiredAccess, std::string &sL)
{
    if (DesiredAccess == THREAD_ALL_ACCESS)
    {
        sL = "THREAD_ALL_ACCESS";
        return;
    }
    if (DesiredAccess & THREAD_TERMINATE)
    {
        DesiredAccess &= ~THREAD_TERMINATE;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("THREAD_TERMINATE");
    }
    if (DesiredAccess & THREAD_SUSPEND_RESUME)
    {
        DesiredAccess &= ~THREAD_SUSPEND_RESUME;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("THREAD_SUSPEND_RESUME");
    }
    if (DesiredAccess & THREAD_GET_CONTEXT)
    {
        DesiredAccess &= ~THREAD_GET_CONTEXT;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("THREAD_GET_CONTEXT");
    }
    if (DesiredAccess & THREAD_SET_CONTEXT)
    {
        DesiredAccess &= ~THREAD_SET_CONTEXT;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("THREAD_SET_CONTEXT");
    }
    if (DesiredAccess & THREAD_QUERY_INFORMATION)
    {
        DesiredAccess &= ~THREAD_QUERY_INFORMATION;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("THREAD_QUERY_INFORMATION");
    }
    if (DesiredAccess & THREAD_SET_INFORMATION)
    {
        DesiredAccess &= ~THREAD_SET_INFORMATION;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("THREAD_SET_INFORMATION");
    }
    if (DesiredAccess & THREAD_SET_THREAD_TOKEN)
    {
        DesiredAccess &= ~THREAD_SET_THREAD_TOKEN;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("THREAD_SET_THREAD_TOKEN");
    }
    if (DesiredAccess & THREAD_IMPERSONATE)
    {
        DesiredAccess &= ~THREAD_IMPERSONATE;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("THREAD_IMPERSONATE");
    }
    if (DesiredAccess & THREAD_DIRECT_IMPERSONATION)
    {
        DesiredAccess &= ~THREAD_DIRECT_IMPERSONATION;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("THREAD_DIRECT_IMPERSONATION");
    }
    if (DesiredAccess & THREAD_SET_LIMITED_INFORMATION)
    {
        DesiredAccess &= ~THREAD_SET_LIMITED_INFORMATION;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("THREAD_SET_LIMITED_INFORMATION");
    }
    if (DesiredAccess & THREAD_QUERY_LIMITED_INFORMATION)
    {
        DesiredAccess &= ~THREAD_QUERY_LIMITED_INFORMATION;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("THREAD_QUERY_LIMITED_INFORMATION");
    }
    if (DesiredAccess & THREAD_RESUME)
    {
        DesiredAccess &= ~THREAD_RESUME;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("THREAD_RESUME");
    }

    GetCommonDesiredAccess(DesiredAccess, sL);
}

std::string GetMemoryInformationClassString(ULONG QueryClass)
{
    switch(QueryClass)
    {
    case MemoryBasicInformationEx:
        return "MemoryBasicInformation";
    case MemoryWorkingSetInformation:
        return "MemoryWorkingSetInformation";
    case MemoryMappedFilenameInformation:
        return "MemoryMappedFilenameInformation";
    case MemoryRegionInformation:
        return "MemoryRegionInformation";
    case MemoryWorkingSetExInformation:
        return "MemoryWorkingSetExInformation";
    }
    return "Unknown MemoryInformationClass";
}

std::string FormatHexString(ULONG64 addr, int width)
{
	char widthStr[10] = { 0 };
	char szPara[MAX_PATH] = { 0 };
	sprintf(widthStr, "%%%d", width);
	strcat(widthStr, "X");
	sprintf(szPara, widthStr, addr);
	return szPara;
    //return std::string("%1").arg(addr, width, 16, QChar('0')).toUpper();
}

std::string GetMemoryStateString(ULONG State)
{
    switch(State)
    {
    case MEM_COMMIT:
        return "MEM_COMMIT";
    case MEM_FREE:
        return "MEM_FREE";
    case MEM_RESERVE:
        return "MEM_RESERVE";
    default:
        return "Unknown MemoryState";
    }
}

std::string GetMemoryTypeString(ULONG Type)
{
    switch(Type)
    {
    case MEM_IMAGE:
        return "MEM_IMAGE";
    case MEM_MAPPED:
        return "MEM_MAPPED";
    case MEM_PRIVATE:
        return "MEM_PRIVATE";
    default:
        return "";
    }
}

void NormalizeFilePath(LPCWSTR szFilePath, std::wstring &normalized)
{
    if (!_wcsnicmp(szFilePath, L"\\SystemRoot\\", _ARRAYSIZE(L"\\SystemRoot\\") - 1))
    {
        TCHAR szSystemDirectory[MAX_PATH];
        GetSystemWindowsDirectory(szSystemDirectory, MAX_PATH);
        normalized = szSystemDirectory;
        normalized += L"\\";
        normalized += (LPCWSTR)(szFilePath + _ARRAYSIZE(L"\\SystemRoot\\") - 1);
    }
    else if(!_wcsnicmp(szFilePath, L"\\??\\", _ARRAYSIZE(L"\\??\\") - 1))
    {
        normalized = (LPCWSTR)(szFilePath + _ARRAYSIZE(L"\\??\\") - 1);
    }
    else
    {
        normalized = szFilePath;
    }
}

std::string GetWindowsHookTypeString(int type)
{
    switch (type)
    {
    case WH_MSGFILTER:
        return "WH_MSGFILTER";
    case WH_JOURNALRECORD:
        return "WH_JOURNALRECORD";
    case WH_JOURNALPLAYBACK:
        return "WH_JOURNALPLAYBACK";
    case WH_KEYBOARD:
        return "WH_KEYBOARD";
    case WH_GETMESSAGE:
        return "WH_GETMESSAGE";
    case WH_CALLWNDPROC:
        return "WH_CALLWNDPROC";
    case WH_CBT:
        return "WH_CBT";
    case WH_SYSMSGFILTER:
        return "WH_SYSMSGFILTER";
    case WH_MOUSE:
        return "WH_MOUSE";
    case WH_DEBUG:
        return "WH_DEBUG";
    case WH_SHELL:
        return "WH_SHELL";
    case WH_FOREGROUNDIDLE:
        return "WH_FOREGROUNDIDLE";
    case WH_CALLWNDPROCRET:
        return "WH_CALLWNDPROCRET";
    case WH_KEYBOARD_LL:
        return "WH_KEYBOARD_LL";
    case WH_MOUSE_LL:
        return "WH_MOUSE_LL";
    }
    return "Unknown hook type";
}

std::string GetFileInformationClass(int queryClass)
{
    switch (queryClass)
    {
    case FileAccessInformation:
        return "FileAccessInformation";
    case FileAlignmentInformation:
        return "FileAlignmentInformation";
    case FileAllInformation:
        return "FileAllInformation";
    case FileAttributeTagInformation:
        return "FileAttributeTagInformation";
    case FileBasicInformation:
        return "FileBasicInformation";
    case FileEaInformation:
        return "FileEaInformation";
    case FileInternalInformation:
        return "FileInternalInformation";
    case FileIoPriorityHintInformation:
        return "FileIoPriorityHintInformation";
    case FileModeInformation:
        return "FileModeInformation";
    case FileNameInformation:
        return "FileNameInformation";
    case FileNetworkOpenInformation:
        return "FileNetworkOpenInformation";
    case FilePositionInformation:
        return "FilePositionInformation";
    case FileStandardInformation:
        return "FileStandardInformation";
    }
	char szPara[MAX_PATH] = { 0 };
	sprintf(szPara, "Unknown query class %d", queryClass);
	return szPara;
    //return "Unknown query class %1").arg(queryClass);
}

void GetFileAttributesString(ULONG FileAtLibutes, std::string &sL)
{
    if (FileAtLibutes & FILE_ATTRIBUTE_ARCHIVE)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_ARCHIVE");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_COMPRESSED)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_COMPRESSED");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_DEVICE)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_DEVICE");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_DIRECTORY)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_DIRECTORY");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_ENCRYPTED)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_ENCRYPTED");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_HIDDEN)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_HIDDEN");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_NORMAL)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_NORMAL");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_NOT_CONTENT_INDEXED");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_OFFLINE)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_OFFLINE");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_READONLY)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_READONLY");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_REPARSE_POINT)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_REPARSE_POINT");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_SPARSE_FILE)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_SPARSE_FILE");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_SYSTEM)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_SYSTEM");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_TEMPORARY)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_TEMPORARY");
    }
    if (FileAtLibutes & FILE_ATTRIBUTE_VIRTUAL)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("FILE_ATTRIBUTE_VIRTUAL");
    }
}

void GetKeyDesiredAccessString(ULONG DesiredAccess, std::string &sL)
{
    if (DesiredAccess == KEY_READ)
    {
        sL = "KEY_READ";
        return;
    }
    if (DesiredAccess == KEY_WRITE)
    {
        sL = "KEY_WRITE";
        return;
    }
    if (DesiredAccess == KEY_ALL_ACCESS)
    {
        sL = "KEY_ALL_ACCESS";
        return;
    }

    if (DesiredAccess & KEY_QUERY_VALUE)
    {
        DesiredAccess &= ~KEY_QUERY_VALUE;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("KEY_QUERY_VALUE");
    }
    if (DesiredAccess & KEY_SET_VALUE)
    {
        DesiredAccess &= ~KEY_SET_VALUE;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("KEY_SET_VALUE");
    }
    if (DesiredAccess & KEY_CREATE_SUB_KEY)
    {
        DesiredAccess &= ~KEY_CREATE_SUB_KEY;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("KEY_CREATE_SUB_KEY");
    }
    if (DesiredAccess & KEY_ENUMERATE_SUB_KEYS)
    {
        DesiredAccess &= ~KEY_ENUMERATE_SUB_KEYS;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("KEY_ENUMERATE_SUB_KEYS");
    }
    if (DesiredAccess & KEY_CREATE_LINK)
    {
        DesiredAccess &= ~KEY_CREATE_LINK;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("KEY_CREATE_LINK");
    }
    if (DesiredAccess & KEY_NOTIFY)
    {
        DesiredAccess &= ~KEY_NOTIFY;
        if (!sL.empty())
            sL.append(" | ");
        sL.append("KEY_NOTIFY");
    }

    GetCommonDesiredAccess(DesiredAccess, sL);
}

void GetCreateKeyOptionsString(ULONG Options, std::string &sL)
{
    if (Options & REG_OPTION_VOLATILE)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("REG_OPTION_VOLATILE");
    }
    if (Options & REG_OPTION_NON_VOLATILE)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("REG_OPTION_NON_VOLATILE");
    }
    if (Options & REG_OPTION_CREATE_LINK)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("REG_OPTION_CREATE_LINK");
    }
    if (Options & REG_OPTION_BACKUP_RESTORE)
    {
        if (!sL.empty())
            sL.append(" | ");
        sL.append("REG_OPTION_BACKUP_RESTORE");
    }
}

std::string GetCreateKeyDispositionString(ULONG CreateDisposition)
{
    switch (CreateDisposition)
    {
    case REG_CREATED_NEW_KEY:
        return "REG_CREATED_NEW_KEY";
    case REG_OPENED_EXISTING_KEY:
        return "REG_OPENED_EXISTING_KEY";
    }
    return "";
}

std::string NumberToCurrencyString(ULONG64 f)
{
	char szPara[MAX_PATH] = { 0 };
	_itoa(f, szPara, sizeof(szPara));
	std::string sLF(szPara);// = std::string::number((qlonglong)f);
    int pos = sLF.size() - 6;
    while (pos > 0)
    {
        sLF = sLF.insert(pos, ",");
        pos -= 3;
    }
    return sLF;
}

std::string FormatFileSizeString(ULONG64 fileSize)
{
    std::string sL;
	char szPara[MAX_PATH] = { 0 };
    if(fileSize <= 1024ULL)
    {
		sprintf(szPara, "%LLd Bytes", fileSize);
		return szPara;
        //return QObject::L("%1 Bytes").arg(fileSize);
    }
    else if(fileSize <= 1024ULL * 1024ULL)
    {
		sprintf(szPara, "%f KB", fileSize / (double)(1024ULL));
        //sL += QObject::L("%1 KB").arg(fileSize / (double)(1024ULL), 0, 'f', 3);
    }
    else if(fileSize <= 1024ULL * 1024ULL * 1024ULL)
    {
		sprintf(szPara, "%f MB", fileSize / (double)(1024ULL * 1024ULL));
        //sL += QObject::L("%1 MB").arg(fileSize / (double)(1024ULL * 1024ULL), 0, 'f', 3);
    }
    else if(fileSize <= 1024ULL * 1024ULL * 1024ULL * 1024ULL)
    {
		sprintf(szPara, "%f GB", fileSize / (double)(1024ULL * 1024ULL * 1024ULL));
        //sL += QObject::L("%1 GB").arg(fileSize / (double)(1024ULL * 1024ULL * 1024ULL), 0, 'f', 3);
    }
    else if(fileSize <= 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL)
    {
		sprintf(szPara, "%f TB", fileSize / (double)(1024ULL * 1024ULL * 1024ULL * 1024ULL));
       // sL += QObject::L("%1 TB").arg(fileSize / (double)(1024ULL * 1024ULL * 1024ULL * 1024ULL), 0, 'f', 3);
    }
	sprintf(szPara, "%s Bytes", NumberToCurrencyString(fileSize).c_str());
    //sL += QObject::L(" (%1 Bytes)").arg(NumberToCurrencyString(fileSize));
    return sL;
}

std::string GetRegistryKeyDataType(ULONG DataType)
{
	switch (DataType) {
	case REG_NONE: return "REG_NONE";
	case REG_SZ: return "REG_SZ";
	case REG_EXPAND_SZ: return "REG_SZ";

	case REG_BINARY: return "REG_BINARY";
	case REG_DWORD: return "REG_DWORD";
	case REG_DWORD_BIG_ENDIAN: return "REG_DWORD_BIG_ENDIAN";
	case REG_LINK: return "REG_LINK";
	case REG_MULTI_SZ: return "REG_MULTI_SZ";

	case REG_RESOURCE_LIST: return "REG_RESOURCE_LIST";
	case REG_FULL_RESOURCE_DESCRIPTOR: return "REG_FULL_RESOURCE_DESCRIPTOR";
	case REG_RESOURCE_REQUIREMENTS_LIST: return "REG_RESOURCE_REQUIREMENTS_LIST";

	case REG_QWORD: return "REG_QWORD";
	}
	char szPara[MAX_PATH] = { 0 };
	sprintf(szPara, "Unknown DataType(%d)", DataType);
	return szPara;
    //return std::string("Unknown DataType(%1)").arg(DataType);
}

int32_t swapInt32(int32_t value)
{
     return ((value & 0x000000FF) << 24) |
               ((value & 0x0000FF00) << 8) |
               ((value & 0x00FF0000) >> 8) |
               ((value & 0xFF000000) >> 24) ;
}

std::string GetRegistryQueryValueKeyClass(ULONG QueryClass)
{
    switch(QueryClass){
        case KeyValueBasicInformation: return "KeyValueBasicInformation";
        case KeyValueFullInformation: return "KeyValueFullInformation";
        case KeyValuePartialInformation: return "KeyValuePartialInformation";

        case KeyValueFullInformationAlign64: return "KeyValueFullInformationAlign64";
        case KeyValuePartialInformationAlign64: return "KeyValuePartialInformationAlign64";
    }
	char szPara[MAX_PATH] = { 0 };
	sprintf(szPara, "Unknown QueryClass(%d)", QueryClass);
	return szPara;
    //return std::string("Unknown QueryClass(%1)").arg(QueryClass);
}

std::string GetRegistryQueryKeyClass(ULONG QueryClass)
{
    switch(QueryClass){
        case KeyBasicInformation: return "KeyBasicInformation";
        case KeyNodeInformation: return "KeyNodeInformation";
        case KeyFullInformation: return "KeyFullInformation";
        case KeyNameInformation: return "KeyNameInformation";
        case KeyCachedInformation: return "KeyCachedInformation";
        case KeyFlagsInformation: return "KeyFlagsInformation";
        case KeyVirtualizationInformation: return "KeyVirtualizationInformation";
        case KeyHandleTagsInformation: return "KeyHandleTagsInformation";
        case KeyTrustInformation: return "KeyTrustInformation";
    }
	char szPara[MAX_PATH] = { 0 };
	sprintf(szPara, "Unknown QueryClass(%d)", QueryClass);
	return szPara;
    //return std::string("Unknown QueryClass(%1)").arg(QueryClass);
}

void GetMutantDesiredAccessString(ULONG DesiredAccess, std::string &sL)
{
    if (DesiredAccess == MUTANT_ALL_ACCESS)
    {
        sL = "MUTANT_ALL_ACCESS";
        return;
    }
    if (DesiredAccess & MUTANT_QUERY_STATE)
    {
        DesiredAccess &= ~MUTANT_QUERY_STATE;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("MUTANT_QUERY_STATE");
    }
    GetCommonDesiredAccess(DesiredAccess, sL);
}

void GetDirectoryObjectDesiredAccessString(ULONG DesiredAccess, std::string &sL)
{
    if (DesiredAccess == DIRECTORY_ALL_ACCESS)
    {
        sL = "DIRECTORY_ALL_ACCESS";
        return;
    }
    if (DesiredAccess & DIRECTORY_QUERY)
    {
        DesiredAccess &= ~DIRECTORY_QUERY;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("DIRECTORY_QUERY");
    }
    if (DesiredAccess & DIRECTORY_TRAVERSE)
    {
        DesiredAccess &= ~DIRECTORY_TRAVERSE;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("DIRECTORY_TRAVERSE");
    }
    if (DesiredAccess & DIRECTORY_CREATE_OBJECT)
    {
        DesiredAccess &= ~DIRECTORY_CREATE_OBJECT;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("DIRECTORY_CREATE_OBJECT");
    }
    if (DesiredAccess & DIRECTORY_CREATE_SUBDIRECTORY)
    {
        DesiredAccess &= ~DIRECTORY_CREATE_SUBDIRECTORY;
        if(!sL.empty())
            sL.append(" | ");
        sL.append("DIRECTORY_CREATE_SUBDIRECTORY");
    }
    GetCommonDesiredAccess(DesiredAccess, sL);
}

//BOOL GetPEInfo(PBYTE pBuf, ULONG uSize, ULONG &ImageSize, bool &bIs64Bit)
//{
//	PIMAGE_DOS_HEADER   pDosHdr = NULL;
//	PIMAGE_NT_HEADERS32 pNTHdr32 = NULL;
//	PIMAGE_NT_HEADERS64 pNTHdr64 = NULL;
//	PIMAGE_FILE_HEADER  pFileHdr = NULL;
//
//	if (NULL == pBuf || 0 == uSize)
//		return FALSE;
//
//	pDosHdr = (PIMAGE_DOS_HEADER)pBuf;
//	if (IsBadReadPtr(pDosHdr, sizeof(IMAGE_DOS_HEADER)))
//		return FALSE;
//	if (IMAGE_DOS_SIGNATURE != pDosHdr->e_magic)
//		return FALSE;
//
//	pNTHdr32 = (PIMAGE_NT_HEADERS32)((PBYTE)pDosHdr + pDosHdr->e_lfanew);
//
//	if (IsBadReadPtr(pNTHdr32, sizeof(IMAGE_NT_HEADERS32)))
//		return FALSE;
//	if (IMAGE_NT_SIGNATURE != pNTHdr32->Signature)
//		return FALSE;
//
//	pFileHdr = &pNTHdr32->FileHeader;
//
//	if (IsBadReadPtr(pFileHdr, sizeof(IMAGE_FILE_HEADER)))
//		return FALSE;
//
//	if (pFileHdr->Machine == IMAGE_FILE_MACHINE_AMD64)
//		bIs64Bit = true;
//	else
//		bIs64Bit = false;
//
//	if (bIs64Bit)
//	{
//		pNTHdr64 = (PIMAGE_NT_HEADERS64)((PBYTE)pDosHdr + pDosHdr->e_lfanew);
//
//		if (IsBadReadPtr(pNTHdr64, sizeof(IMAGE_NT_HEADERS64)))
//			return FALSE;
//
//		ImageSize = pNTHdr64->OptionalHeader.SizeOfImage;
//	}
//	else
//	{
//		ImageSize = pNTHdr32->OptionalHeader.SizeOfImage;
//	}
//
//	return TRUE;
//}