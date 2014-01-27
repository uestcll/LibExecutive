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
		CLSharedMemory sm("test_for_shared_memory_normal");
		long *p = (long *)sm.GetAddress();

		if(*p != 5)
			cout << "*p, 5 error" << endl;

		if(sm.GetRefCount() != 2)
			cout << "sm.GetRefCount(), 2 error" << endl;

		*p = *p + 1;

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