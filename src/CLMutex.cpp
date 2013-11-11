#include "CLMutex.h"
#include "CLLogger.h"
#include "CLMutexByPThread.h"
#include "CLMutexByRecordLocking.h"
#include "CLMutexByRecordLockingAndPThread.h"
#include "CLMutexBySharedPThread.h"
#include "DefinitionForConst.h"

CLMutex::CLMutex()
{
	m_pMutex = new CLMutexByPThread();

	if(!(m_pMutex->Initialize()).IsSuccess())
	{
		delete m_pMutex;
		CLLogger::WriteLogMsg("In CLMutex::CLMutex(), Initialize error", 0);
		throw "In CLMutex::CLMutex(), Initialize error";
	}
}

CLMutex::CLMutex(pthread_mutex_t *pMutex)
{
	m_pMutex = new CLMutexByPThread(pMutex);

	if(!(m_pMutex->Initialize()).IsSuccess())
	{
		delete m_pMutex;
		CLLogger::WriteLogMsg("In CLMutex::CLMutex() 2, Initialize error", 0);
		throw "In CLMutex::CLMutex(), Initialize error";
	}
}

CLMutex::CLMutex(const char *pstrFileName, int nType)
{
	if(nType == MUTEX_USE_RECORD_LOCK)
	{
		m_pMutex = new CLMutexByRecordLocking(pstrFileName);
	}
	else if(nType == MUTEX_USE_RECORD_LOCK_AND_PTHREAD)
	{
		m_pMutex = new CLMutexByRecordLockingAndPThread(pstrFileName);
	}
	else if(nType == MUTEX_USE_SHARED_PTHREAD)
	{
		m_pMutex = new CLMutexBySharedPThread(pstrFileName);
	}
	else
		throw "In CLMutex::CLMutex(), nType error";

	if(!(m_pMutex->Initialize()).IsSuccess())
	{
		delete m_pMutex;
		CLLogger::WriteLogMsg("In CLMutex::CLMutex() 3, Initialize error", 0);
		throw "In CLMutex::CLMutex(), Initialize error";
	}
}

CLMutex::CLMutex(const char *pstrFileName, pthread_mutex_t *pMutex)
{
	m_pMutex = new CLMutexByRecordLockingAndPThread(pstrFileName, pMutex);

	if(!(m_pMutex->Initialize()).IsSuccess())
	{
		delete m_pMutex;
		CLLogger::WriteLogMsg("In CLMutex::CLMutex() 4, Initialize error", 0);
		throw "In CLMutex::CLMutex(), Initialize error";
	}
}

CLMutex::~CLMutex()
{
	if(!(m_pMutex->Uninitialize().IsSuccess()))
		CLLogger::WriteLogMsg("In CLMutex::~CLMutex(), Uninitialize error", 0);

	delete m_pMutex;
}

CLStatus CLMutex::Lock()
{
	CLStatus s = m_pMutex->Lock();

	if(!s.IsSuccess())
		CLLogger::WriteLogMsg("In CLMutex::Lock(), Lock error", 0);

	return s;
}

CLStatus CLMutex::Unlock()
{
	CLStatus s = m_pMutex->Unlock();

	if(!s.IsSuccess())
		CLLogger::WriteLogMsg("In CLMutex::Unlock(), Unock error", 0);

	return s;
}

CLMutexInterface *CLMutex::GetMutexInterface()
{
	return m_pMutex;
}