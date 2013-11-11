#ifndef CLMutex_H
#define CLMutex_H

#include <pthread.h>
#include "CLStatus.h"
#include "CLMutexInterface.h"

class CLMutex
{
public:
	CLMutex();
	explicit CLMutex(pthread_mutex_t *pMutex);
	CLMutex(const char *pstrFileName, int nType);
	CLMutex(const char *pstrFileName, pthread_mutex_t *pMutex);
	virtual ~CLMutex();

	CLStatus Lock();
	CLStatus Unlock();

	CLMutexInterface *GetMutexInterface();

private:
	CLMutex(const CLMutex&);
	CLMutex& operator=(const CLMutex&);

private:
	CLMutexInterface *m_pMutex;
};

#endif