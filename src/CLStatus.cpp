#include "CLStatus.h"

CLStatus::CLStatus(long lReturnCode, long lErrorCode) : m_clReturnCode(m_lReturnCode), m_clErrorCode(m_lErrorCode)
{
	m_lReturnCode = lReturnCode;
	m_lErrorCode = lErrorCode;
}

CLStatus::~CLStatus()
{
}

CLStatus::CLStatus(const CLStatus& status) : m_clReturnCode(m_lReturnCode), m_clErrorCode(m_lErrorCode)
{
	m_lReturnCode = status.m_lReturnCode;
	m_lErrorCode = status.m_lErrorCode;
}

bool CLStatus::IsSuccess()
{
	if(m_clReturnCode >= 0)
		return true;
	else
		return false;
}