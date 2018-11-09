#include "registry.h"
#include "nt.h"
#include"boost/format.hpp"
#define USE_DL_PREFIX
#include "dlmalloc.h"

extern size_t g_total;

CRegKeyValue::CRegKeyValue(ULONG Type, ULONG Size, LPCWSTR szValueName)
{
    m_Type = Type;
    m_Size = Size;
    m_ValueName = m_StringMgr->GetString(szValueName);
}

void* CRegKeyValue::operator new(size_t size)
{
    g_total += size;
    //OutputDebugString((LPCWSTR)std::string("CRegKeyValue alloc %1 / %2 bytes\n").arg(size).arg(g_total).utf16());
    return dlmalloc(size);
}

void CRegKeyValue::operator delete(void*p)
{
    dlfree(p);
}

//RegKeyValueBinary

CRegKeyValueBinary::CRegKeyValueBinary(ULONG Type, ULONG Size, LPCWSTR szValueName, std::string &BinaryData)
    : CRegKeyValue(Type, Size, szValueName)
{
    m_Data = BinaryData;
}

void CRegKeyValueBinary::PrintFull(std::string &str) const
{
    for(int i = 0;i < m_Data.size(); ++i) {
		str += ((boost::format("%02X") % (BYTE)m_Data.at(i))).str();
        //str += std::string("%1").arg((BYTE)m_Data.at(i), 2, 16, QChar('0')).toUpper();
        if(i != m_Data.size() - 1) {
            str += (i != 0 && i % 8 == 7) ? "\n" : " ";
        }
    }

    if((ULONG)m_Data.size() < m_Size)
        str += ("\n...More");
}

bool CRegKeyValueBinary::NeedBreak(void) const
{
    return true;
}

//CRegKeyValueString

CRegKeyValueString::CRegKeyValueString(ULONG Type, ULONG Size, LPCWSTR szValueName, std::string &BinaryData)
    : CRegKeyValue(Type, Size, szValueName)
{
    m_More = ((ULONG)BinaryData.size() < Size);

	const char * raw = (const char *)BinaryData.c_str();
    for(int i = 0, j = 0;i < BinaryData.size() / 2; ++i)
    {
        if(raw[i] == L'\0' || i == BinaryData.size() / 2 - 1)
        {
            std::string str((const char *)&raw[j], i - j);
            m_Data.push_back(str);

            j = i+1;
        }
    }
}

void CRegKeyValueString::PrintFull(std::string &str) const
{
    if(m_Data.size() == 1){
        str += m_Data.front();
    }else{
		auto iter = m_Data.begin();
		for (; iter != m_Data.end(); ++iter) {
			str +=(*iter);
			str += "\n";
		}
        /*for(int i = 0;i < m_Data.size(); ++i){
            str += m_Data.at(i);
            str += "\n";
        }*/
    }
    if(m_More)
        str += (" ...More");
}

bool CRegKeyValueString::NeedBreak(void) const
{
    return (m_Data.size() > 1);
}

//RegKeyValueNumber

CRegKeyValueNumber::CRegKeyValueNumber(ULONG Type, ULONG Size, LPCWSTR szValueName, std::string &BinaryData)
    : CRegKeyValue(Type, Size, szValueName)
{
    ULONG64 NumberData = 0;
    if(BinaryData.size() >= sizeof(ULONG64)) {
        memcpy(&NumberData, BinaryData.c_str(), sizeof(ULONG64));
    } else if(BinaryData.size() >= sizeof(DWORD)) {
        if(Type == REG_DWORD){
            memcpy(&NumberData, BinaryData.c_str(), sizeof(DWORD));
        }else{
            ULONG temp;
            memcpy(&temp, BinaryData.c_str(), sizeof(DWORD));
            NumberData = swapInt32(temp);
        }
    }

    m_Data = NumberData;
}

void CRegKeyValueNumber::PrintFull(std::string &str) const
{
	str += (boost::format("%1% (0x%2%)") % m_Data % FormatHexString(m_Data, m_Data > 0xffffffff ? 16 : 8)).str();
    //str += QObject::tr("%1 (0x%2)").arg(std::string::number(m_Data), FormatHexString(m_Data, m_Data > 0xffffffff ? 16 : 8));
}

bool CRegKeyValueNumber::NeedBreak(void) const
{
    return false;
}
