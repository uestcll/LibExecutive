#include "CLCriticalSection.h"
#include "CLMutex.h"
#include "CLLogger.h"
#include "CLStatus.h"

CLCriticalSection::CLCriticalSection(CLMutex *pMutex)
{
	if(pMutex == 0)
		throw "In CLCriticalSection::CLCriticalSection(), pMutex == 0";
	
	m_pMutex = pMutex;

	CLStatus s = m_pMutex->Lock();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLCriticalSection::CLCriticalSection(), m_pMutex->Lock error", 0);
		throw "In CLCriticalSection::CLCriticalSection(), m_pMutex->Lock error";
	}
}

CLCriticalSection::~CLCriticalSection()
{
	CLStatus s = m_pMutex->Unlock();
	if(!s.IsSuccess())
		CLLogger::WriteLogMsg("In CLCriticalSection::~CLCriticalSection(), m_pMutex->Unlock error", 0);
}