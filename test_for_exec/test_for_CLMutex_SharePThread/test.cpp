#include <iostream>
#include <fcntl.h>
#include "LibExecutive.h"

using namespace std;

int count = 100000;

static void *thread_for_shared_mutex_bypthread(void *arg)
{
	long *pg = (long *)arg;

	for(int i = 0; i < count; i++)
	{
		CLMutex mutex("mutex_for_CLMutex_SharedMutexByPthread", MUTEX_USE_SHARED_PTHREAD);
		CLCriticalSection cs(&mutex);

		(*pg) = (*pg) + 1;
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
		CLSharedMemory *psm = new CLSharedMemory("test_for_CLMutex_SharedMutexByPthread", 8);
		long *pg = (long *)(psm->GetAddress());

		pthread_t tid;
		pthread_create(&tid, 0, thread_for_shared_mutex_bypthread, pg);

		thread_for_shared_mutex_bypthread(pg);

		pthread_join(tid, 0);

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