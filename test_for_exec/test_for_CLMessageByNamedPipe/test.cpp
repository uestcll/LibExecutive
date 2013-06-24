#include <iostream>
#include <fcntl.h>
#include "LibExecutive.h"

using namespace std;

int main(int argc, char* argv[])
{
	try
	{
		if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
		{
			cout << "Initialize error" << endl;
			return 0;
		}

		sleep(2);

		int fd = open("/tmp/test_pipe", O_WRONLY);
		if(fd == -1)
			throw CLStatus(-1, 0);

		for(int i = 0; i < 10; i++)
		{
			if(i % 2 == 0)
			{
				int length = 16;
				write(fd, &length, 4);
				long cmd = 1;
				write(fd, &cmd, 8);
				write(fd, &i, 4);
				write(fd, &i, 4);

				CLEvent event("test_pipe", true);
				event.Set();
			}
			else
			{
				int length = 20;
				write(fd, &length, 4);
				long cmd = 2;
				write(fd, &cmd, 8);
				long i_i = i;
				write(fd, &i_i, 8);
				write(fd, &i, 4);

				CLEvent event("test_pipe", true);
				event.Set();
			}
		}

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