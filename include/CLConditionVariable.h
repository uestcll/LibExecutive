#ifndef CLConditionVariable_H
#define CLConditionVariable_H

#include <pthread.h>
#include <string>
#include "CLStatus.h"

using namespace std;

class CLMutex;

class CLConditionVariable
{
public:
	/*
	构造函数和析构函数出错时，会抛出字符串类型异常
	*/
    CLConditionVariable();
	explicit CLConditionVariable(pthread_cond_t *pCond);
	CLConditionVariable(const char* pstrCondName);
	virtual ~CLConditionVariable();

	CLStatus Wait(CLMutex *pMutex);
	CLStatus Wakeup();
	CLStatus WakeupAll();

private:
	CLConditionVariable(const CLConditionVariable&);
	CLConditionVariable& operator=(const CLConditionVariable&);

private:
	pthread_cond_t *m_pConditionVariable;
	bool m_bNeededDestroy;

	string m_strCondName;
};

#endif