#ifndef CLMutexByPThread_H
#define CLMutexByPThread_H

#include <pthread.h>
#include "CLMutexInterface.h"

class CLMutexByPThread : public CLMutexInterface
{
public:
	CLMutexByPThread();
	explicit CLMutexByPThread(pthread_mutex_t *pMutex);
	virtual ~CLMutexByPThread();

	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus Lock();
	virtual CLStatus Unlock();

	pthread_mutex_t *GetMutexPointer();

private:
	CLMutexByPThread(const CLMutexByPThread&);
	CLMutexByPThread& operator=(const CLMutexByPThread&);

private:
	pthread_mutex_t *m_pMutex;
	bool m_bNeededDestroy;
};

#endif