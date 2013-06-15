#ifndef CLMutexByRecordLockingAndPThread_H
#define CLMutexByRecordLockingAndPThread_H

#include <pthread.h>
#include "CLMutexInterface.h"
#include "CLMutex.h"

class CLMutexByRecordLockingAndPThread : public CLMutexInterface
{
public:
	CLMutexByRecordLockingAndPThread(const char *pstrFileName);
	CLMutexByRecordLockingAndPThread(const char *pstrFileName, pthread_mutex_t *pMutex);
	virtual ~CLMutexByRecordLockingAndPThread();

	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus Lock();
	virtual CLStatus Unlock();

private:
	CLMutexByRecordLockingAndPThread(const CLMutexByRecordLockingAndPThread&);
	CLMutexByRecordLockingAndPThread& operator=(const CLMutexByRecordLockingAndPThread&);

private:
	CLMutex m_ThreadMutex;
	CLMutex m_ProcessMutex;
};

#endif