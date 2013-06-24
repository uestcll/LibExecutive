#include <iostream>
#include <unistd.h>
#include "LibExecutive.h"

using namespace std;

int main(int argc, char* argv[])
{
	if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
	{
		cout << "in test initialize error" << endl;
		return 0;
	}

	sleep(30);

	for(int i = 0; i < 1000000; i++)
	{
		CLSharedMutexAllocator::Get("multithread_multiprocess_allocate_mutex_mutex");
		CLSharedMutexAllocator::Release("multithread_multiprocess_allocate_mutex_mutex");
	}

	sleep(30);

	if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
		cout << "in test destroy error" << endl;

    return 0;
}