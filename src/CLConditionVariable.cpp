#include <string.h>
#include "CLConditionVariable.h"
#include "CLMutex.h"
#include "CLLogger.h"
#include "CLMutexByPThread.h"
#include "CLMutexBySharedPThread.h"
#include "CLSharedConditionVariableAllocator.h"

CLConditionVariable::CLConditionVariable()
{
	m_pConditionVariable = new pthread_cond_t;
	m_bNeededDestroy = true;
	m_strCondName = "";

	int  r = pthread_cond_init(m_pConditionVariable, 0);
	if(r != 0)
	{
		delete m_pConditionVariable;

		CLLogger::WriteLogMsg("In CLConditionVariable::CLConditionVariable(), pthread_cond_init error", r);
		throw "In CLConditionVariable::CLConditionVariable(), pthread_cond_init error";
	}
}

CLConditionVariable::CLConditionVariable(pthread_cond_t *pCond)
{
	if(pCond == 0)
		throw "In CLConditionVariable::CLConditionVariable(), pCond is 0";

	m_pConditionVariable = pCond;
	m_bNeededDestroy = false;
	m_strCondName = "";
}

CLConditionVariable::CLConditionVariable(const char* pstrCondName)
{
	m_strCondName = pstrCondName;
	m_bNeededDestroy = false;

	m_pConditionVariable = CLSharedConditionVariableAllocator::Get(pstrCondName);
	if(m_pConditionVariable == 0)
	{
		CLLogger::WriteLogMsg("In CLConditionVariable::CLConditionVariable(), CLSharedConditionVariableAllocator::Get error", 0);
		throw "In CLConditionVariable::CLConditionVariable(), CLSharedConditionVariableAllocator::Get error";
	}
}

CLConditionVariable::~CLConditionVariable()
{
	if(m_bNeededDestroy)
	{
		int r = pthread_cond_destroy(m_pConditionVariable);
		if(r != 0)
		{
			delete m_pConditionVariable;
			CLLogger::WriteLogMsg("In CLConditionVariable::~CLConditionVariable(), pthread_cond_destroy error", r);
		}

		delete m_pConditionVariable;
	}

	if(m_strCondName.length() != 0)
	{
		CLStatus s = CLSharedConditionVariableAllocator::Release(m_strCondName.c_str());
		if(!s.IsSuccess())
			CLLogger::WriteLogMsg("In CLConditionVariable::~CLConditionVariable(), CLSharedConditionVariableAllocator::Release error", 0);
	}
}

CLStatus CLConditionVariable::Wait(CLMutex *pMutex)
{
	if(pMutex == NULL)
		return CLStatus(-1, 0);

	CLMutexInterface *pInterface = pMutex->GetMutexInterface();
	CLMutexByPThread *p1 = dynamic_cast<CLMutexByPThread *>(pInterface);
	CLMutexBySharedPThread *p2 = dynamic_cast<CLMutexBySharedPThread *>(pInterface);

	int r = 0;
	if(p1 != NULL)
	{
		r = pthread_cond_wait(m_pConditionVariable, p1->GetMutexPointer());
	}
	else if(p2 != NULL)
	{
		r = pthread_cond_wait(m_pConditionVariable, p2->GetMutexPointer());
	}
	else
	{
		CLLogger::WriteLogMsg("In CLConditionVariable::Wait, pMutex error", 0);
		return CLStatus(-1, 0);
	}

	if(r != 0)
	{
		CLLogger::WriteLogMsg("In CLConditionVariable::Wait, pthread_cond_wait error", r);
		return CLStatus(-1, 0);
	}
	else
	{
		return CLStatus(0, 0);
	}
}

CLStatus CLConditionVariable::Wakeup()
{
	int r = pthread_cond_signal(m_pConditionVariable);
	if(r != 0)
	{
		CLLogger::WriteLogMsg("In CLConditionVariable::Wakeup, pthread_cond_signal error", r);
		return CLStatus(-1, 0);
	}
	else
	{
		return CLStatus(0, 0);
	}
}

CLStatus CLConditionVariable::WakeupAll()
{
	int r = pthread_cond_broadcast(m_pConditionVariable);
	if(r != 0)
	{
		CLLogger::WriteLogMsg("In CLConditionVariable::WakeupAll, pthread_cond_broadcast error", r);
		return CLStatus(-1, 0);
	}
	else
	{
		return CLStatus(0, 0);
	}
}