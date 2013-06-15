#ifndef CLMutexBySharedPThread_H
#define CLMutexBySharedPThread_H

#include <pthread.h>
#include <string>
#include "CLMutexInterface.h"

using namespace std;

class CLMutexBySharedPThread : public CLMutexInterface
{
public:
	CLMutexBySharedPThread(const char *pstrMutexName);
	virtual ~CLMutexBySharedPThread();

	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus Lock();
	virtual CLStatus Unlock();

	pthread_mutex_t *GetMutexPointer();

private:
	CLMutexBySharedPThread(const CLMutexBySharedPThread&);
	CLMutexBySharedPThread& operator=(const CLMutexBySharedPThread&);

private:
	pthread_mutex_t *m_pMutex;
	string m_strMutexName;
};

#endif