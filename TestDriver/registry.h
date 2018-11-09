#ifndef REGISTRY_H
#define REGISTRY_H

#include<string>
#include<list>
#include <Windows.h>
#include "StringMgr.h"

class CRegKeyValue
{
public:
    CRegKeyValue(ULONG Type, ULONG Size, LPCWSTR szValueName);
    void* operator new(size_t size);
    void operator delete(void*p);
    ULONG GetType(void) const{return m_Type;}
    ULONG GetSize(void) const{return m_Size;}
    std::string GetValueName(void) const{return m_ValueName.GetQString();}

    virtual void PrintFull(std::string &str) const = 0;
    virtual bool NeedBreak(void) const = 0;
    virtual bool IsDummy(void) const {return false;}
protected:
    ULONG m_Type;
    ULONG m_Size;
    CUniqueString m_ValueName;
};

class CRegKeyValueBinary : public CRegKeyValue
{
public:
    CRegKeyValueBinary(ULONG Type, ULONG Size, LPCWSTR szValueName, std::string &BinaryData);

    virtual void PrintFull(std::string &str) const;
    virtual bool NeedBreak(void) const;
private:
    std::string m_Data;
};

class CRegKeyValueString : public CRegKeyValue
{
public:
    CRegKeyValueString(ULONG Type, ULONG Size, LPCWSTR szValueName, std::string &BinaryData);

    virtual void PrintFull(std::string &str) const;
    virtual bool NeedBreak(void) const;
private:
    std::list<std::string> m_Data;
    bool m_More;
};

class CRegKeyValueNumber : public CRegKeyValue
{
public:
    CRegKeyValueNumber(ULONG Type, ULONG Size, LPCWSTR szValueName, std::string &BinaryData);

    virtual void PrintFull(std::string &str) const;
    virtual bool NeedBreak(void) const;
private:
    ULONG64 m_Data;
};


#endif // REGISTRY_H
