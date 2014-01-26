#include <iostream>
#include <fcntl.h>
#include "LibExecutive.h"

using namespace std;

struct TestStructForCLMutex
{
	CLMutex *pmutex;
	int i;
};

int count = 100000;

static void *thread_for_record_and_pthread(void *arg)
{
	TestStructForCLMutex *p = (TestStructForCLMutex *)arg;

	for(int j = 0; j < count; j++)
	{
		p->pmutex->Lock();

		lseek(p->i, SEEK_SET, 0);
		long k = 0;
		read(p->i, &k, sizeof(long));

		lseek(p->i, SEEK_SET, 0);
		k++;
		write(p->i, &k, sizeof(long));

		p->pmutex->Unlock();
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
		int fd = open("/tmp/test_for_record_lock", O_RDWR);
		if(fd == -1)
		{
			cout << "open error" << endl;
			throw CLStatus(-1, 0);
		}

		CLMutex mutex("testforrecordlock", MUTEX_USE_RECORD_LOCK);

		TestStructForCLMutex sl;
		sl.i = fd;
		sl.pmutex = &mutex;

		thread_for_record_and_pthread(&sl);

		close(fd);

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