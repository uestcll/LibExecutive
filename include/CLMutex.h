#ifndef CLMutex_H
#define CLMutex_H

#include <pthread.h>
#include "CLStatus.h"
#include "CLMutexInterface.h"

#define MUTEX_USE_RECORD_LOCK 0
#define MUTEX_USE_RECORD_LOCK_AND_PTHREAD 1
#define MUTEX_USE_SHARED_PTHREAD 2

class CLMutex
{
public:
	/*
	构造函数和析构函数出错时，会抛出字符串类型异常
	*/
	CLMutex();
	//需要保证pMutex指向的互斥量已经被初始化了
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