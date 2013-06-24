#include <iostream>
#include "LibExecutive.h"

using namespace std;

int main(int argc, char* argv[])
{
	if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
	{
		cout << "in test initialize error" << endl;
		return 0;
	}

	sleep(2);

	{
		CLEvent event("test_for_event_auto");
		event.Set();
	}

	if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
		cout << "in test destroy error" << endl;

    return 0;
}