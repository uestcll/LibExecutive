#include <iostream>
#include <fcntl.h>
#include "LibExecutive.h"

using namespace std;

static const char *test_pipe_name = "test_for_CLMessageQueueByNamedPipe_PrivateQueueForSelfPostMsg";

int main(int argc, char* argv[])
{
	try
	{
		if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
		{
			cout << "Initialize error" << endl;
			return 0;
		}

		string str = "/tmp/";
		str += test_pipe_name;
		int fd = open(str.c_str(), O_WRONLY | O_NONBLOCK);

		char *p = new char[20];
		*((int *)p) = 16;
		*((long *)(p + 4)) = 1;
		*((int *)(p + 12)) = 2;
		*((int *)(p + 16)) = 3;
		write(fd, p, 20);
		{
			CLEvent event(test_pipe_name, true);
			event.Set();
		}
		delete [] p;

		p = new char[24];
		*((int *)p) = 20;
		*((long *)(p + 4)) = 2;
		*((long *)(p + 12)) = 4;
		*((int *)(p + 20)) = 6;
		write(fd, p, 24);
		{
			CLEvent event(test_pipe_name, true);
			event.Set();
		}
		delete [] p;

		close(fd);

		throw CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
			cout << "Destroy error" << endl;

		return 0;
	}

    return 0;
}