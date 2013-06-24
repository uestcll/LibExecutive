#include <iostream>
#include "LibExecutive.h"

using namespace std;

/*
int main(int argc, char* argv[])
{
	if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
	{
		cout << "in test initialize error" << endl;
		return 0;
	}

	CLSharedMemory sm("shared_space_for_shared_mutex_allocator");
	long p = (long)sm.GetAddress();

	long p1 = (long)CLSharedMutexAllocator::Get("mutex_CLSharedMutexAllocator_datavrify_1");

	CLSharedMutexAllocator::Release("mutex_CLSharedMutexAllocator_datavrify_1");

	if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
		cout << "in test destroy error" << endl;

	return 0;
}
*/

int main(int argc, char* argv[])
{
	if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
	{
		cout << "in test initialize error" << endl;
		return 0;
	}

	CLSharedMemory sm("test_for_shared_mutex_Exec", sizeof(long));
	long *p = (long *)sm.GetAddress();

	CLSharedMutexAllocator *palloc = CLSharedMutexAllocator::GetInstance();
	if(palloc == 0)
	{
		cout << "palloc ooooooo" << endl;
		return 0;
	}

	pthread_mutex_t *pmutex = palloc->GetSharedMutex("shared_mutex_for_test_shared_mutex_alloc_Exec");

	for(int i = 0; i < 1000000; i++)
	{
		CLMutex mutex(pmutex);
		CLCriticalSection cs(&mutex);

		(*p) = (*p) + 1;
	}

	if(!palloc->ReleaseSharedMutex("shared_mutex_for_test_shared_mutex_alloc_Exec").IsSuccess())
		cout << "realease error" << endl;

	if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
		cout << "in test destroy error" << endl;

    return 0;
}