#include <string.h>
#include <pthread.h>
#include "CLMutexBySharedPThread.h"
#include "CLLogger.h"
#include "CLSharedObjectAllocator.h"

CLMutexBySharedPThread::CLMutexBySharedPThread(const char *pstrMutexName)
{
	if((pstrMutexName == 0) || (strlen(pstrMutexName) == 0))
		throw "In CLMutexBySharedPThread::CLMutexBySharedPThread(), pstrMutexName error";

	m_strMutexName = pstrMutexName;

	m_pMutex = CLSharedObjectAllocator<CLSharedMutexImpl, pthread_mutex_t>::Get(pstrMutexName);
	if(m_pMutex == 0)
	{
		CLLogger::WriteLogMsg("In CLMutexBySharedPThread::CLMutexBySharedPThread(), Get error", 0);
		throw "In CLMutexBySharedPThread::CLMutexBySharedPThread(), Get error";
	}
}

CLMutexBySharedPThread::~CLMutexBySharedPThread()
{
	if(!CLSharedObjectAllocator<CLSharedMutexImpl, pthread_mutex_t>::Release(m_strMutexName.c_str()).IsSuccess())
		CLLogger::WriteLogMsg("In CLMutexBySharedPThread::~CLMutexBySharedPThread(), Release error", 0);
}

CLStatus CLMutexBySharedPThread::Initialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMutexBySharedPThread::Uninitialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMutexBySharedPThread::Lock()
{
	int r = pthread_mutex_lock(m_pMutex);
	if(r != 0)
	{
		CLLogger::WriteLogMsg("In CLMutexBySharedPThread::Lock(), pthread_mutex_lock error", r);
		return CLStatus(-1, NORMAL_ERROR);
	}

	return CLStatus(0, 0);
}

CLStatus CLMutexBySharedPThread::Unlock()
{
	int r = pthread_mutex_unlock(m_pMutex);
	if(r != 0)
	{
		CLLogger::WriteLogMsg("In CLMutexBySharedPThread::Unlock(), pthread_mutex_unlock error", r);
		return CLStatus(-1, NORMAL_ERROR);
	}

	return CLStatus(0, 0);
}

pthread_mutex_t *CLMutexBySharedPThread::GetMutexPointer()
{
	return m_pMutex;
}