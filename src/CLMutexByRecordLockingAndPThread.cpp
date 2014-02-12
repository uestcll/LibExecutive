#include "CLMutexByRecordLockingAndPThread.h"
#include "CLLogger.h"
#include "DefinitionForConst.h"

CLMutexByRecordLockingAndPThread::CLMutexByRecordLockingAndPThread(const char *pstrFileName) : m_ProcessMutex(pstrFileName, MUTEX_USE_RECORD_LOCK)
{
}

CLMutexByRecordLockingAndPThread::CLMutexByRecordLockingAndPThread(const char *pstrFileName, pthread_mutex_t *pMutex) : m_ThreadMutex(pMutex), m_ProcessMutex(pstrFileName, MUTEX_USE_RECORD_LOCK)
{
}

CLMutexByRecordLockingAndPThread::~CLMutexByRecordLockingAndPThread()
{
}

CLStatus CLMutexByRecordLockingAndPThread::Initialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMutexByRecordLockingAndPThread::Uninitialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMutexByRecordLockingAndPThread::Lock()
{
	if(!m_ThreadMutex.Lock().IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMutexByRecordLockingAndPThread::Lock(), m_ThreadMutex.Lock error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	if(!m_ProcessMutex.Lock().IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMutexByRecordLockingAndPThread::Lock(), m_ProcessMutex.Lock error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	return CLStatus(0, 0);
}

CLStatus CLMutexByRecordLockingAndPThread::Unlock()
{
	if(!m_ProcessMutex.Unlock().IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMutexByRecordLockingAndPThread::Lock(), m_ProcessMutex.Unlock error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	if(!m_ThreadMutex.Unlock().IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMutexByRecordLockingAndPThread::Lock(), m_ThreadMutex.Unlock error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	return CLStatus(0, 0);
}