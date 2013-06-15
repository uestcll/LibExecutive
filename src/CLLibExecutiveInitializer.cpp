#include "CLLibExecutiveInitializer.h"
#include "CLLogger.h"
#include "CLExecutiveNameServer.h"
#include "CLSharedMutexAllocator.h"
#include "CLSharedConditionVariableAllocator.h"
#include "CLSharedEventAllocator.h"

pthread_mutex_t CLLibExecutiveInitializer::m_MutexForInitializer = PTHREAD_MUTEX_INITIALIZER;

bool CLLibExecutiveInitializer::m_bInitialized = false;
bool CLLibExecutiveInitializer::m_bDestroyed = false;

CLStatus CLLibExecutiveInitializer::Initialize()
{
	int r = pthread_mutex_lock(&m_MutexForInitializer);
	if(r != 0)
	{
		if((m_bInitialized) && (!m_bDestroyed))
			CLLogger::WriteLogMsg("In CLLibExecutiveInitializer::Initialize(), pthread_mutex_lock error", r);

		return CLStatus(-1, r);
	}

	if(m_bInitialized)
	{
		r = pthread_mutex_unlock(&m_MutexForInitializer);
		if((r != 0) && (!m_bDestroyed))
			CLLogger::WriteLogMsg("In CLLibExecutiveInitializer::Initialize(), pthread_mutex_unlock error", r);

		return CLStatus(-1, r);
	}

	try
	{
		CLStatus s = CLLogger::Create();
		if(!s.IsSuccess())
			throw s;

		CLStatus s2 = CLSharedMutexAllocator::Create();
		if(!s2.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLLibExecutiveInitializer::Initialize(), CLSharedMutexAllocator::Create error", 0);
			throw s2;
		}

		CLStatus s3 = CLSharedConditionVariableAllocator::Create();
		if(!s3.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLLibExecutiveInitializer::Initialize(), CLSharedConditionVariableAllocator::Create error", 0);
			throw s3;
		}

		CLStatus s4 = CLSharedEventAllocator::Create();
		if(!s4.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLLibExecutiveInitializer::Initialize(), CLSharedEventAllocator::Create error", 0);
			throw s4;
		}

		CLStatus s1 = CLExecutiveNameServer::Create();
		if(!s1.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLLibExecutiveInitializer::Initialize(), CLExecutiveNameServer::Create error", 0);
			throw s1;
		}

		m_bInitialized = true;
		throw CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		r = pthread_mutex_unlock(&m_MutexForInitializer);
		if(r != 0)
			return CLStatus(-1, r);

		return s;
	}
	catch(...)
	{
		r = pthread_mutex_unlock(&m_MutexForInitializer);
		if(r != 0)
			return CLStatus(-1, r);

		return CLStatus(-1, 0);
	}
}

CLStatus CLLibExecutiveInitializer::Destroy()
{
	int r = pthread_mutex_lock(&m_MutexForInitializer);
	if(r != 0)
	{
		if((m_bInitialized) && (!m_bDestroyed))
			CLLogger::WriteLogMsg("In CLLibExecutiveInitializer::Destroy(), pthread_mutex_lock error", r);

		return CLStatus(-1, r);
	}

	try
	{
		if((!m_bInitialized) || (m_bDestroyed))
			throw CLStatus(-1, 0);

		CLStatus s1 = CLExecutiveNameServer::Destroy();
		if(!s1.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLLibExecutiveInitializer::Destroy(), CLExecutiveNameServer::Destroy error", 0);
			throw s1;
		}

		CLStatus s4 = CLSharedEventAllocator::Destroy();
		if(!s4.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLLibExecutiveInitializer::Destroy(), CLSharedEventAllocator::Destroy error", 0);
			throw s4;
		}

		CLStatus s3 = CLSharedConditionVariableAllocator::Destroy();
		if(!s3.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLLibExecutiveInitializer::Destroy(), CLSharedConditionVariableAllocator::Destroy error", 0);
			throw s3;
		}

		CLStatus s2 = CLSharedMutexAllocator::Destroy();
		if(!s2.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLLibExecutiveInitializer::Destroy(), CLSharedMutexAllocator::Destroy error", 0);
			throw s2;
		}
	
		CLStatus s = CLLogger::Destroy();
		if(!s.IsSuccess())
			throw s;

		m_bDestroyed = true;
		throw CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		r = pthread_mutex_unlock(&m_MutexForInitializer);
		if(r != 0)
			return CLStatus(-1, r);

		return s;
	}
	catch(...)
	{
		r = pthread_mutex_unlock(&m_MutexForInitializer);
		if(r != 0)
			return CLStatus(-1, r);

		return CLStatus(-1, 0);
	}
}