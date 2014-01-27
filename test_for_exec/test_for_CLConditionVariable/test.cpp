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
		CLSharedMemory *psm = new CLSharedMemory("test_for_multi_process_for_shared_cond", 16);
		long *p = (long *)(psm->GetAddress());
		long *flag = (long *)(((char *)p) + 8);

		CLMutex mutex("mutex_for_test_for_multi_process_for_shared_cond", MUTEX_USE_SHARED_PTHREAD);

		{
			CLCriticalSection cs(&mutex);

			*flag = 1;

			*p = 5;
		}

		CLConditionVariable cv("test_conditoin_variable_for_multiprocess");

		cv.Wakeup();

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