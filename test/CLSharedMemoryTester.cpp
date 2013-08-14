#include <gtest/gtest.h>
#include "LibExecutive.h"
#include <unistd.h>
#include <sys/wait.h>

TEST(CLSharedMemory, Normal)
{
	pid_t pid = fork();
	if(pid == 0)
	{
		sleep(2);

		{
			CLSharedMemory sm("test_for_shared_memory_normal");
			long *p = (long *)sm.GetAddress();
			EXPECT_EQ(*p, 5);
			EXPECT_EQ(sm.GetRefCount(), 2);

			*p = *p + 1;
		}

		CLLibExecutiveInitializer::Destroy();
		_exit(0);
	}

	CLSharedMemory sm("test_for_shared_memory_normal", sizeof(long));
	long *p = (long *)sm.GetAddress();
	*p = 5;
	EXPECT_EQ(sm.GetRefCount(), 1);

	sleep(5);

	EXPECT_EQ(*p, 6);
	EXPECT_EQ(sm.GetRefCount(), 1);
}