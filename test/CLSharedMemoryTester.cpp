#include <gtest/gtest.h>
#include "LibExecutive.h"
#include <unistd.h>
#include <sys/wait.h>

TEST(CLSharedMemory, Normal)
{
	CLLogger::WriteLogMsg("CLSharedMemory Test", 0);

	CLSharedMemory sm("test_for_shared_memory_normal", sizeof(long));
	long *p = (long *)sm.GetAddress();
	*p = 5;
	EXPECT_EQ(sm.GetRefCount(), 1);

	CLEvent event("test_for_event_auto");

	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLSharedMemory/main").IsSuccess());

	EXPECT_TRUE(event.Wait().IsSuccess());

	EXPECT_EQ(*p, 6);
	EXPECT_EQ(sm.GetRefCount(), 1);
}