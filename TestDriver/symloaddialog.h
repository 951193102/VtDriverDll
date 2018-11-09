#ifndef SYMLOADDIALOG_H
#define SYMLOADDIALOG_H

#include "ModuleMgr.h"
#include<string>
#include<Windows.h>
//class CNamedFunc
//{
//public:
//	CNamedFunc(std::string name, ULONG offset)
//	{
//		m_name = name;
//		m_offset = offset;
//	}
//
//	std::string m_name;
//	ULONG m_offset;
//};


class SymLoadDialog
{

public:
    enum LoadState
    {
        Idle = 0,
        LoadingSymbol,
        Loaded,
    };
	
public:
    explicit SymLoadDialog();
    ~SymLoadDialog();
    //void closeEvent(QCloseEvent *e);
	void OnLoadAllImageCompleteNotify(void);
    void EnumSystemModuleProc(ULONG64 ImageBase, ULONG ImageSize, LPCWSTR szImagePath, int LoadOrderIndex);
private:
    LoadState m_LoadState;
	std::wstring m_Path_win32k;
	std::wstring m_Path_win32kFull;
	std::wstring m_Path_fltmgr;
};

#endif // SYMLOADDIALOG_H
