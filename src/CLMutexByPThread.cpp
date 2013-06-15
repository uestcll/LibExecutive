#include "CLMutexByPThread.h"
#include "CLLogger.h"

CLMutexByPThread::CLMutexByPThread()
{
	m_pMutex = new pthread_mutex_t;

	m_bNeededDestroy = true;

	int  r = pthread_mutex_init(m_pMutex, 0);
	if(r != 0)
	{
		delete m_pMutex;

		CLLogger::WriteLogMsg("In CLMutexByPThread::CLMutexByPThread(), pthread_mutex_init error", r);
		throw "In CLMutexByPThread::CLMutexByPThread(), pthread_mutex_init error";
	}
}

CLMutexByPThread::CLMutexByPThread(pthread_mutex_t *pMutex)
{
	if(pMutex == 0)
		throw "In CLMutexByPThread::CLMutexByPThread(), pMutex is 0";

	m_pMutex = pMutex;
	m_bNeededDestroy = false;
}

CLMutexByPThread::~CLMutexByPThread()
{
	if(m_bNeededDestroy)
	{
		int r = pthread_mutex_destroy(m_pMutex);
		if(r != 0)
			CLLogger::WriteLogMsg("In CLMutexByPThread::~CLMutexByPThread(), pthread_mutex_destroy error", r);

		delete m_pMutex;
	}
}

CLStatus CLMutexByPThread::Initialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMutexByPThread::Uninitialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMutexByPThread::Lock()
{
	int r = pthread_mutex_lock(m_pMutex);
	if(r != 0)
	{
		CLLogger::WriteLogMsg("In CLMutexByPThread::Lock(), pthread_mutex_lock error", r);
		return CLStatus(-1, 0);
	}
	
	return CLStatus(0, 0);
}

CLStatus CLMutexByPThread::Unlock()
{
	int r = pthread_mutex_unlock(m_pMutex);
	if(r != 0)
	{
		CLLogger::WriteLogMsg("In CLMutexByPThread::Unlock(), pthread_mutex_unlock error", r);
		return CLStatus(-1, 0);
	}
		
	return CLStatus(0, 0);
}

pthread_mutex_t *CLMutexByPThread::GetMutexPointer()
{
	return m_pMutex;
}