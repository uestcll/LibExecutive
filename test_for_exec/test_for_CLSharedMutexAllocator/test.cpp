#include <iostream>
#include <fcntl.h>
#include "LibExecutive.h"

using namespace std;

const int count = 100000;

static void *thread_for_CLSharedMutexAllocator_MultiProcess_MuiltThread(void *arg)
{
	const char *mutex_name = "mutex_for_CLSharedMutexAllocator_MultiProcess";

	long *p = (long *)arg;

	pthread_mutex_t *pmutex = NULL;

	for(int i = 0; i < count; i++)
	{
		pmutex = CLSharedObjectAllocator<CLSharedMutexImpl, pthread_mutex_t>::Get(mutex_name);

		{
			CLMutex mutex(pmutex);
			CLCriticalSection cs(&mutex);

			(*p) = (*p) + 1;
		}

		CLSharedObjectAllocator<CLSharedMutexImpl,pthread_mutex_t>::Release(mutex_name);
	}

	return 0;
}

int main(int argc, char* argv[])
{
	if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
	{
		cout << "in test initialize error" << endl;
		return 0;
	}

	try
	{
		const char *shared_name = "shared_resource_for_CLSharedMutexAllocator_MultiProcess";

		CLSharedMemory sm(shared_name, sizeof(long));
		long *p = (long *)sm.GetAddress();

		pthread_t tid;
		pthread_create(&tid, 0, thread_for_CLSharedMutexAllocator_MultiProcess_MuiltThread, p);

		thread_for_CLSharedMutexAllocator_MultiProcess_MuiltThread(p);

		pthread_join(tid, 0);

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