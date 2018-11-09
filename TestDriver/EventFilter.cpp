#include "ProcessMgr.h"
#include "EventMgr.h"
#include "EventFilter.h"
#include "util.h"
#include "nt.h"
#include<boost/algorithm/string.hpp>

void CEventFilter::Reference()
{
    InterlockedIncrement(&m_RefCount);
}

void CEventFilter::Dereference()
{
    if(0 == InterlockedDecrement(&m_RefCount))
        delete this;
}

CEventFilter_Hex64::CEventFilter_Hex64(ULONG64 HexValue, filter_rel Relation, bool bInclude) : CEventFilter(Relation, bInclude)
{
    m_HexValue = HexValue;
    m_DisplayValue = (boost::format("0x%1") % (FormatHexString(HexValue, 16)) ).str();
}

CEventFilter_PID::CEventFilter_PID(ULONG ProcessId, filter_rel Relation, bool bInclude) : CEventFilter_Number(ProcessId, Relation, bInclude)
{
}

CEventFilter_ProcessName::CEventFilter_ProcessName(std::string & val, filter_rel Relation, bool bInclude) : CEventFilter_String(val, Relation, bInclude)
{
}

CEventFilter_ProcessPath::CEventFilter_ProcessPath(std::string & val, filter_rel Relation, bool bInclude) : CEventFilter_String(val, Relation, bInclude)
{
}

CEventFilter_EventPath::CEventFilter_EventPath(std::string & val, filter_rel Relation, bool bInclude) : CEventFilter_String(val, Relation, bInclude)
{
}

CEventFilter_SessionId::CEventFilter_SessionId(ULONG SessionId, filter_rel Relation, bool bInclude) : CEventFilter_Number(SessionId, Relation, bInclude)
{
}

CEventFilter_Arch::CEventFilter_Arch(bool bIs64Bit, filter_rel Relation, bool bInclude) : CEventFilter_Binary(bIs64Bit, Relation, bInclude)
{
    m_DisplayValue = bIs64Bit ? "x64" : "x86";
}

CEventFilter_EventType::CEventFilter_EventType(EventType_t eventType, filter_rel Relation, bool bInclude) : CEventFilter(Relation, bInclude)
{
    m_EventType = eventType;

    m_DisplayValue = m_EventMgr->m_EventNames[eventType];
}

CEventFilter_EventClass::CEventFilter_EventClass(EventClass_t eventClass, filter_rel Relation, bool bInclude) : CEventFilter(Relation, bInclude)
{
    m_EventClass = eventClass;

    m_DisplayValue = m_EventMgr->m_EventClassNames[eventClass];
}

CEventFilter_BriefResult::CEventFilter_BriefResult(std::string & val, filter_rel Relation, bool bInclude) : CEventFilter_String(val, Relation, bInclude)
{
}

bool CEventFilter_PID::DoFilter(const CUniqueEvent *ev)
{
	if (m_Relation == FltRel_Is)
	{
        return ((int)ev->GetProcessId() == m_NumberValue) ? true : false;
	}
	else if (m_Relation == FltRel_IsNot)
	{
        return ((int)ev->GetProcessId() != m_NumberValue) ? true : false;
	}
	else if (m_Relation == FltRel_LargerThan)
	{
        return ((int)ev->GetProcessId() >= m_NumberValue) ? true : false;
	}
	else if (m_Relation == FltRel_SmallerThan)
	{
        return ((int)ev->GetProcessId() <= m_NumberValue) ? true : false;
	}
	//Should not go here

	return false;
}

bool CEventFilter_ProcessName::DoFilter(const CUniqueEvent *ev)
{
    std::string  str = ev->GetProcessName();
	std::string lowValue(m_StrValue);
	std::string lowStr(str);
	boost::to_lower(lowValue);
	boost::to_lower(lowStr);
    if (m_Relation == FltRel_Is)
    {
        return  lowStr  == lowValue ? true : false;
    }
    else if (m_Relation == FltRel_IsNot)
    {
        return lowStr != lowValue ? true : false;
    }
    else if (m_Relation == FltRel_Contain)
    {
		return (lowStr.find(lowValue) != lowStr.npos) ? true : false;
    }
    else if (m_Relation == FltRel_NotContain)
    {
        return (lowStr.find(lowValue) == lowStr.npos) ? true : false;
    }
	//Should not go here

	return false;
}

bool CEventFilter_ProcessPath::DoFilter(const CUniqueEvent *ev)
{
    std::string  str = ev->GetProcessPath();
	std::string lowValue(m_StrValue);
	std::string lowStr(str);
	boost::to_lower(lowValue);
	boost::to_lower(lowStr);

    if (m_Relation == FltRel_Is)
    {
        return (lowValue == lowStr) ? true : false;
    }
    else if (m_Relation == FltRel_IsNot)
    {
        return (lowValue != lowStr) ? true : false;
    }
    else if (m_Relation == FltRel_Contain)
    {
		return (lowStr.find(lowValue) != lowStr.npos) ? true : false;
    }
    else if (m_Relation == FltRel_NotContain)
    {
        return (lowStr.find(lowValue) == lowStr.npos) ? true : false;
    }
	//Should not go here

	return false;
}

bool CEventFilter_EventPath::DoFilter(const CUniqueEvent *ev)
{
    std::string  str = ev->GetEventPath();
	std::string lowValue(m_StrValue);
	std::string lowStr(str);
	boost::to_lower(lowValue);
	boost::to_lower(lowStr);

	if (m_Relation == FltRel_Is)
	{
		return (lowValue == lowStr) ? true : false;
	}
	else if (m_Relation == FltRel_IsNot)
	{
		return (lowValue != lowStr) ? true : false;
	}
	else if (m_Relation == FltRel_Contain)
	{
		return (lowStr.find(lowValue) != lowStr.npos) ? true : false;
	}
	else if (m_Relation == FltRel_NotContain)
	{
		return (lowStr.find(lowValue) == lowStr.npos) ? true : false;
	}
	//Should not go here

	return false;
}

bool CEventFilter_Arch::DoFilter(const CUniqueEvent *ev)
{
	if (m_Relation == FltRel_Is)
	{
        return (ev->GetUniqueProcess()->m_bIs64Bit == m_bBinary) ? true : false;
	}
	else if (m_Relation == FltRel_IsNot)
	{
        return (ev->GetUniqueProcess()->m_bIs64Bit != m_bBinary) ? true : false;
	}
	//Should not go here

	return false;
}

bool CEventFilter_SessionId::DoFilter(const CUniqueEvent *ev)
{
	if (m_Relation == FltRel_Is)
	{
        return ((int)ev->GetUniqueProcess()->m_SessionId == m_NumberValue) ? true : false;
	}
	else if (m_Relation == FltRel_IsNot)
	{
        return ((int)ev->GetUniqueProcess()->m_SessionId != m_NumberValue) ? true : false;
	}
	else if (m_Relation == FltRel_LargerThan)
	{
        return ((int)ev->GetUniqueProcess()->m_SessionId >= m_NumberValue) ? true : false;
	}
	else if (m_Relation == FltRel_SmallerThan)
	{
        return ((int)ev->GetUniqueProcess()->m_SessionId <= m_NumberValue) ? true : false;
	}
	//Should not go here

	return false;
}

bool CEventFilter_EventType::DoFilter(const CUniqueEvent *ev)
{
	if (m_Relation == FltRel_Is)
	{
		return (ev->GetEventType() == m_EventType) ? true : false;
	}
	else if (m_Relation == FltRel_IsNot)
	{
		return (ev->GetEventType() != m_EventType) ? true : false;
	}
	//Should not go here

	return false;
}

bool CEventFilter_EventClass::DoFilter(const CUniqueEvent *ev)
{
	if (m_Relation == FltRel_Is)
	{
		return (ev->GetEventClassify() == m_EventClass) ? true : false;
	}
	else if (m_Relation == FltRel_IsNot)
	{
		return (ev->GetEventClassify() != m_EventClass) ? true : false;
	}
	//Should not go here

	return false;
}

bool CEventFilter_BriefResult::DoFilter(const CUniqueEvent *ev)
{
    std::string  str;
    ev->GetBriefResult(str);
	std::string lowValue(m_StrValue);
	std::string lowStr(str);
	boost::to_lower(lowValue);
	boost::to_lower(lowStr);

	if (m_Relation == FltRel_Is)
	{
		return (lowValue == lowStr) ? true : false;
	}
	else if (m_Relation == FltRel_IsNot)
	{
		return (lowValue != lowStr) ? true : false;
	}
	else if (m_Relation == FltRel_Contain)
	{
		return (lowStr.find(lowValue) != lowStr.npos) ? true : false;
	}
	else if (m_Relation == FltRel_NotContain)
	{
		return (lowStr.find(lowValue) == lowStr.npos) ? true : false;
	}
    //Should not go here

    return false;
}
