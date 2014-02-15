#include <iostream>
#include <fcntl.h>
#include "LibExecutive.h"

using namespace std;

int main(int argc, char* argv[])
{
	if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
	{
		cout << "in test initialize error" << endl;
		return 0;
	}

	try
	{
		const char *name_cv = "test_conditoin_variable_for_CLSharedConditionVariableAllocator_MultiProcess";
		const char *name_mutex = "mutex_for_test_for_CLSharedConditionVariableAllocator_MultiProcess";

		CLSharedMemory *psm = new CLSharedMemory("test_for_CLSharedConditionVariableAllocator_MultiProcess", 16);
		long *p = (long *)(psm->GetAddress());
		long *flag = (long *)(((char *)p) + 8);

		CLMutex mutex(name_mutex, MUTEX_USE_SHARED_PTHREAD);

		{
			CLCriticalSection cs(&mutex);

			*flag = 1;

			*p = 5;
		}

		pthread_cond_t *pCV = CLSharedObjectAllocator<CLSharedConditionVariableImpl,pthread_cond_t>::Get(name_cv);

		CLConditionVariable cv(pCV);

		cv.Wakeup();

		CLSharedObjectAllocator<CLSharedConditionVariableImpl,pthread_cond_t>::Release(name_cv);

		delete psm;

		throw CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		{
			CLEvent event("test_for_event_auto");
			event.Set();
		}

		if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
			cout << "in test destroy error" << endl;
	}

    return 0;
}