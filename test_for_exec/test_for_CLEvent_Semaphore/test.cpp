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
		CLEvent event("test_for_event_semaphore", true);
		
		event.Set();
		event.Set();
		event.Set();

		throw CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
			cout << "in test destroy error" << endl;
	}

    return 0;
}