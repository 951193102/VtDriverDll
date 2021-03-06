﻿#ifndef SYSMODULEMGR_H
#define SYSMODULEMGR_H


#include <Windows.h>
#include <dbghelp.h>
#include <string>
#include <vector>
#include <set>
#include"boost/boost/unordered_map.hpp"
class CUniqueProcess;

class CNamedFunc
{
public:
    CNamedFunc(std::string name, ULONG offset)
    {
        m_name = name;
        m_offset = offset;
    }

    std::string m_name;
    ULONG m_offset;
};
typedef std::vector<CNamedFunc> CFuncList;
typedef boost::unordered_map<std::string, ULONG> CFuncMap;

class CUniqueImage
{
public:
    CUniqueImage(ULONG ImageSize, LPCWSTR szImagePath, bool bIs64Bit, bool bIsSystemModule);
    //void* operator new(size_t size);
    //void operator delete(void*p);

    ULONG FindFunctionOffsetByName(std::string &name);
    ULONG FindClosestFunction(IN ULONG Offset, OUT std::string &name);

    ULONG m_ImageSize;
    std::string m_ImagePath;
    std::string m_FileName;

    bool m_bIs64Bit;
    bool m_bIsSystemModule;

    CFuncList m_NamedFunctions;
    CFuncMap m_NamedFuncMaps;

    bool m_bLoadedSymbol;
};

class CUniqueModule
{
public:
    CUniqueModule(ULONG64 ImageBase, CUniqueImage *Image);
    //void* operator new(size_t size);
    //void operator delete(void*p);

    ULONG64 FindFunctionAddressByName(std::string &name);
    ULONG FindClosestFunction(IN ULONG Offset, OUT std::string &name);

    ULONG64 m_ImageBase;
    CUniqueImage *m_UniqueImage;
};

class CImageFileInfo
{
public:
    CImageFileInfo();
    ULONG64 fileSize;
    USHORT fileVer[4];
    USHORT proVer[4];
    std::string fileDesc, productName, productVer, copyRights, companyName;
};

class CModuleWorker
{
public:
    void OnQueuedGetModuleInfo(CUniqueProcess *up, ULONG64 BaseAddress);
    void OnQueuedGetImageFileInfo(std::wstring ImagePath);

public:
	void QueuedAddModule(CUniqueProcess *up, ULONG64 ImageBase, ULONG ImageSize, std::wstring ImagePath, bool bIs64Bit, bool bIsSystem){}
	void QueuedAddImageFileInfo(std::wstring ImagePath, CImageFileInfo *info){}
};

//Only for loading symbols
class CSymbolWorker
{
public:
	void OnQueuedLoadImage(std::wstring ImagePath, bool bUseSymbol, bool bIsSystem);

public:
	void QueuedAddImage(std::wstring ImagePath, ULONG ImageSize, bool bIs64Bit, bool bIsSystem, CFuncList *funcList) {}
	void QueuedLoadImageComplete(std::wstring ImagePath, CUniqueImage *ui){}
};

typedef boost::unordered_map<std::wstring, CUniqueImage *> CImageMap;
typedef boost::unordered_map<std::wstring, CImageFileInfo *> CImageFileInfoMap;

class CModuleMgr 
{
    
public:
    explicit CModuleMgr();
    void Initialize(void);
    void Uninitialize(void);
    void LoadImageFromFile(std::wstring &ImagePath, bool bUseSymbol, bool bIsSystem);

    CUniqueImage *GetImage(std::wstring &ImagePath, ULONG ImageSize, bool bIs64Bit, bool bIsSystem);
    CUniqueImage *GetImage(std::wstring &ImagePath);

    CImageFileInfo *GetImageFileInfo(std::wstring &ImagePath);

    CImageMap m_ImageMap;
    CImageFileInfoMap m_ImageFileInfoMap;

    HANDLE m_moduleWorkerThread;
    HANDLE m_symbolWorkerThread;

    CUniqueImage *m_Image_ntoskrnl;
    CUniqueImage *m_Image_win32k;
    CUniqueImage *m_Image_win32kFull;

    std::wstring m_Path_ntoskrnl;
    std::wstring m_Path_win32k;
    std::wstring m_Path_win32kFull;
    std::wstring m_Path_fltmgr;

public:
    void QueuedLoadImage(std::wstring ImagePath, bool bUseSymbol, bool bIsSystem);
    void LoadingImageNotify(std::wstring moduleName);
    void LoadAllImageCompleteNotify(void){}

    void QueuedGetModuleInfo(CUniqueProcess *up, ULONG64 BaseAddress);
    void QueuedGetImageFileInfo(std::wstring ImagePath);
    void UpdateModuleNotify(CUniqueProcess *up, CUniqueModule *um){}
    void UpdateImageNotify(CUniqueImage *ui){}
    void UpdateImageFileInfoNotify(std::wstring ImagePath, CImageFileInfo *info){}
private :    
    void OnQueuedAddImage(std::wstring ImagePath, ULONG ImageSize, bool bIs64Bit, bool bIsSystem, CFuncList *funcList);
    void OnLoadImageComplete(std::wstring ImagePath, CUniqueImage *ui);

    void OnQueuedAddModule(CUniqueProcess *up, ULONG64 ImageBase, ULONG ImageSize, std::wstring ImagePath, bool bIs64Bit, bool bIsSystem);
    void OnQueuedAddImageFileInfo(std::wstring ImagePath, CImageFileInfo *info);
private:
    std::set<std::wstring> m_LoadImageQueue;
    std::set<std::wstring> m_LoadedImageQueue;
};
BOOL EnumPEExport(PUCHAR pBaseAddress, UINT uSize, CFuncList *funcList);
BOOL CALLBACK EnumSymCallBack(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext);
extern CModuleMgr *m_ModuleMgr;

#endif // SYSMODULEMGR_H
