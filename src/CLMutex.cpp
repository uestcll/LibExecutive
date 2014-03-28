#include "CLMutex.h"
#include "CLLogger.h"
#include "CLMutexByPThread.h"
#include "CLMutexByRecordLocking.h"
#include "CLMutexByRecordLockingAndPThread.h"
#include "CLMutexBySharedPThread.h"

CLMutex::CLMutex()
{
	m_pMutex = new CLMutexByPThread();

	if(!(m_pMutex->Initialize()).IsSuccess())
	{
		delete m_pMutex;
		throw "In CLMutex::CLMutex(), Initialize error";
	}
}

CLMutex::CLMutex(pthread_mutex_t *pMutex)
{
	m_pMutex = new CLMutexByPThread(pMutex);

	if(!(m_pMutex->Initialize()).IsSuccess())
	{
		delete m_pMutex;
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
		throw "In CLMutex::CLMutex(), Initialize error";
	}
}

CLMutex::CLMutex(const char *pstrFileName, pthread_mutex_t *pMutex)
{
	m_pMutex = new CLMutexByRecordLockingAndPThread(pstrFileName, pMutex);

	if(!(m_pMutex->Initialize()).IsSuccess())
	{
		delete m_pMutex;
		throw "In CLMutex::CLMutex(), Initialize error";
	}
}

CLMutex::~CLMutex()
{
	m_pMutex->Uninitialize();
	delete m_pMutex;
}

CLStatus CLMutex::Lock()
{
	return m_pMutex->Lock();
}

CLStatus CLMutex::Unlock()
{
	return m_pMutex->Unlock();
}

CLMutexInterface *CLMutex::GetMutexInterface()
{
	return m_pMutex;
}