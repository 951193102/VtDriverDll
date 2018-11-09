#define _CRT_SECURE_NO_WARNINGS
#include "symloaddialog.h"
#include "DriverWrapper.h"
#include<dbghelp.h>
#include<vector>
//#include "ps.h"
#include "nt.h"
#include "util.h"
#include"Protocol.h"

#pragma comment(lib,"dbghelp.lib")
SymLoadDialog::SymLoadDialog()
{
    m_LoadState = LoadState::Idle;
}

SymLoadDialog::~SymLoadDialog()
{
}

//void SymLoadDialog::closeEvent(QCloseEvent *e)
//{
    /*if(m_LoadState != LoadState::Loaded)
    {
        e->ignore();
        return;
    }*/

    //SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
//}

void SymLoadDialog::EnumSystemModuleProc(ULONG64 ImageBase, ULONG ImageSize, LPCWSTR szImagePath, int LoadOrderIndex)
{
    UNREFERENCED_PARAMETER(ImageBase);
    UNREFERENCED_PARAMETER(ImageSize);

    if(LoadOrderIndex == 0)
    {
        std::wstring path;
        NormalizeFilePath(szImagePath, path);
       /* m_ModuleMgr->m_Path_ntoskrnl = path;
        m_ModuleMgr->LoadImageFromFile(path, ui->checkBox_ntoskrnl->isChecked(), true);*/
        return;
    }

    if(!_wcsicmp(szImagePath, L"\\SystemRoot\\System32\\win32k.sys"))
    {
        std::wstring path;
        NormalizeFilePath(szImagePath, path);
        m_Path_win32k = path;
        //m_ModuleMgr->LoadImageFromFile(path, ui->checkBox_win32k->isChecked(), true);
        return;
    }

    if(!_wcsicmp(szImagePath, L"\\SystemRoot\\System32\\win32kFull.sys"))
    {
        std::wstring path;
        NormalizeFilePath(szImagePath, path);
        m_Path_win32kFull = path;
        //m_ModuleMgr->LoadImageFromFile(path, ui->checkBox_win32kFull->isChecked(), true);
        return;
    }

    if(!_wcsicmp(szImagePath, L"\\SystemRoot\\System32\\drivers\\fltmgr.sys"))
    {
        std::wstring path;
        NormalizeFilePath(szImagePath, path);
        m_Path_fltmgr = path;
        //m_ModuleMgr->LoadImageFromFile(path, ui->checkBox_fltmgr->isChecked(), true);
        return;
    }
}

//void SymLoadDialog::on_pushButton_load_clicked()
//{
//    m_LoadState = LoadState::LoadingSymbol;
//
//    ui->checkBox_ntoskrnl->setEnabled(false);
//    ui->checkBox_win32k->setEnabled(false);
//    ui->checkBox_win32kFull->setEnabled(false);
//    ui->checkBox_ntdll->setEnabled(false);
//    ui->checkBox_kernel32->setEnabled(false);
//    ui->checkBox_kernelBase->setEnabled(false);
//    ui->checkBox_user32->setEnabled(false);
//    ui->checkBox_fltmgr->setEnabled(false);
//
//    ui->pushButton_load->setEnabled(false);
//
//    //drivers...
//    EnumSystemModules(boost::bind(&SymLoadDialog::EnumSystemModuleProc, this, _1, _2, _3, _4));
//
//    std::wstring path;
//
//    NormalizeFilePath(L"\\SystemRoot\\System32\\ntdll.dll", path);
//    m_ModuleMgr->LoadImageFromFile(path, ui->checkBox_ntdll->isChecked(), false);
//
//    NormalizeFilePath(L"\\SystemRoot\\System32\\kernel32.dll", path);
//    m_ModuleMgr->LoadImageFromFile(path, ui->checkBox_kernel32->isChecked(), false);
//
//    NormalizeFilePath(L"\\SystemRoot\\System32\\kernelBase.dll", path);
//    m_ModuleMgr->LoadImageFromFile(path, ui->checkBox_kernelBase->isChecked(), false);
//
//    NormalizeFilePath(L"\\SystemRoot\\System32\\user32.dll", path);
//    m_ModuleMgr->LoadImageFromFile(path, ui->checkBox_user32->isChecked(), false);
//
//    if(IsAMD64())
//    {
//        NormalizeFilePath(L"\\SystemRoot\\SysWOW64\\ntdll.dll", path);
//        m_ModuleMgr->LoadImageFromFile(path, ui->checkBox_ntdll->isChecked(), false);
//
//        NormalizeFilePath(L"\\SystemRoot\\SysWOW64\\kernel32.dll", path);
//        m_ModuleMgr->LoadImageFromFile(path, ui->checkBox_kernel32->isChecked(), false);
//
//        NormalizeFilePath(L"\\SystemRoot\\SysWOW64\\kernelBase.dll", path);
//        m_ModuleMgr->LoadImageFromFile(path, ui->checkBox_kernelBase->isChecked(), false);
//
//        NormalizeFilePath(L"\\SystemRoot\\SysWOW64\\user32.dll", path);
//        m_ModuleMgr->LoadImageFromFile(path, ui->checkBox_user32->isChecked(), false);
//    }
//}


typedef std::vector<CNamedFunc> CFuncList;


//BOOL CALLBACK EnumSymCallBack(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext)
//{
//	UNREFERENCED_PARAMETER(SymbolSize);
//
//	CFuncList *funcList = (CFuncList *)UserContext;
//
//	if (pSymInfo->Flags & SYMFLAG_PUBLIC_CODE)
//	{
//		funcList->emplace_back(
//			CNamedFunc(std::string(pSymInfo->Name, pSymInfo->NameLen), pSymInfo->Address - pSymInfo->ModBase)
//		);
//	}
//
//	return TRUE;
//}

//BOOL EnumPEExport(PUCHAR pBaseAddress, UINT uSize, CFuncList *funcList)
//{
//	UNREFERENCED_PARAMETER(uSize);
//
//	ULONG uExportSize = 0;
//	PIMAGE_EXPORT_DIRECTORY pImageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)
//		ImageDirectoryEntryToData(pBaseAddress, FALSE, IMAGE_DIRECTORY_ENTRY_EXPORT, &uExportSize);
//	if (IsBadReadPtr(pImageExportDirectory, sizeof(IMAGE_EXPORT_DIRECTORY)))
//		return FALSE;
//
//	DWORD dwNumberOfNames = pImageExportDirectory->NumberOfNames;
//	DWORD *pAddressOfFunction = (DWORD*)ImageRvaToVa(ImageNtHeader(pBaseAddress), pBaseAddress, pImageExportDirectory->AddressOfFunctions, NULL);
//	DWORD *pAddressOfNames = (DWORD*)ImageRvaToVa(ImageNtHeader(pBaseAddress), pBaseAddress, pImageExportDirectory->AddressOfNames, NULL);
//	WORD *pAddressOfNameOrdinals = (WORD*)ImageRvaToVa(ImageNtHeader(pBaseAddress), pBaseAddress, pImageExportDirectory->AddressOfNameOrdinals, NULL);
//
//	if (IsBadReadPtr(pAddressOfNames, sizeof(DWORD) * dwNumberOfNames))
//		return FALSE;
//
//	for (size_t i = 0; i < dwNumberOfNames; i++)
//	{
//		char *strFunction = (char *)ImageRvaToVa(ImageNtHeader(pBaseAddress), pBaseAddress, pAddressOfNames[pAddressOfNameOrdinals[i]], NULL);
//		if (!strFunction)
//			break;
//
//		CNamedFunc func(std::string(strFunction), pAddressOfFunction[i]);
//		funcList->push_back(func);
//	}
//
//	return TRUE;
//}

void SymLoadDialog::OnLoadAllImageCompleteNotify(void)
{
	ULONG ImageSize = 0;
	bool bSuccess = false;
	bool bIs64Bit = false;
	std::wstring ImagePath;
	if (IsAMD64())
	{
		ImagePath=(L"C:\\Windows\\SysWOW64\\win32kfull.sys");
	}
	else {
		ImagePath = (L"C:\\Windows\\System32\\win32kfull.sys");
	}
	CFuncList *funcList = new CFuncList;
	HANDLE hhFile = CreateFile(ImagePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hhFile != INVALID_HANDLE_VALUE)
	{
		ULONG uSize = GetFileSize(hhFile, 0);
		if (0 != uSize)
		{
			//Get PE information from file mapping...
			HANDLE hMapping = CreateFileMapping(hhFile, NULL, PAGE_READONLY, 0, 0, NULL);
			if (NULL != hMapping)
			{
				PBYTE pBuf = (PBYTE)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
				if (NULL != pBuf)
				{
					if (GetPEInfo(pBuf, uSize, ImageSize, bIs64Bit))
					{
						bool bSymbolAvailable = false;
						//if (bUseSymbol)
						{
							DWORD64 dwBase = SymLoadModule64(GetCurrentProcess(), hhFile, NULL, NULL, (DWORD64)NULL, uSize);
							if (dwBase)
							{
								SymEnumSymbols(GetCurrentProcess(), dwBase, 0, EnumSymCallBack, funcList);
								SymUnloadModule64(GetCurrentProcess(), dwBase);
							}
							if (!funcList->empty())
								bSymbolAvailable = true;
						}
						if (!bSymbolAvailable)
							EnumPEExport(pBuf, uSize, funcList);

						bSuccess = true;
					}
				}
				UnmapViewOfFile(pBuf);
			}
			CloseHandle(hMapping);
		}
		CloseHandle(hhFile);
	}

    //Write symbol file...
    std::wstring symPath;
    NormalizeFilePath(L"C:\\WINDOWS\\SyscallMonSymbol.dat", symPath);
    HANDLE hFile = CreateFile(symPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
	auto ret = GetLastError();
    if(hFile != INVALID_HANDLE_VALUE)
    {
        symbol_file_data data = {0};

        data.txsb = 'TXSB';
        data.ver = SYMBOL_FILE_VERSION;
        //data.EnableVmx = ui->checkBox_vmx->isChecked() ? TRUE : FALSE;

        //CUniqueImage *pWin32k = (m_ModuleMgr->m_Image_win32kFull) ? m_ModuleMgr->m_Image_win32kFull : m_ModuleMgr->m_Image_win32k;
        if(!funcList->empty())
        {
			auto iter = funcList->begin();
			for (; iter!= funcList->end(); ++iter)
			{
				if (iter->m_name == "NtUserSetWindowsHookEx") {
					data.NtUserSetWindowsHookExOffset = iter->m_offset;
				}
				else if (iter->m_name == "NtUserSetWindowsHookAW") {
					data.NtUserSetWindowsHookAWOffset = iter->m_offset;
				}
				else if (iter->m_name == "NtUserFindWindowEx") {
					data.NtUserFindWindowExOffset = iter->m_offset;
				}
				else if (iter->m_name == "NtUserInternalGetWindowText") {
					data.NtUserInternalGetWindowTextOffset = iter->m_offset;
				}
				else if (iter->m_name == "NtUserGetClassName") {
					data.NtUserGetClassNameOffset = iter->m_offset;
				}
			}
        }
        else
        {
            OutputDebugStringW(symPath.c_str());
        }

        DWORD dwWrite = 0;
        WriteFile(hFile, &data, sizeof(data), &dwWrite, NULL);
        CloseHandle(hFile);
    }

    m_LoadState = LoadState::Loaded;
}
