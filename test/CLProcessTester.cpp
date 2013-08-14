#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLProcess, Exception)
{
	CLProcess *p = new CLProcess(new CLProcessFunctionForExec, true);

	EXPECT_FALSE((p->WaitForDeath()).IsSuccess());

	EXPECT_FALSE((p->Run((void *)"ok32")).IsSuccess());
}

TEST(CLProcess, Normal)
{
	CLProcess *p = new CLProcess(new CLProcessFunctionForExec, true);

	CLEvent event("test_for_event_auto");

	EXPECT_TRUE((p->Run((void *)"../test_for_exec/test_for_CLProcess/main")).IsSuccess());

	EXPECT_TRUE(event.Wait().IsSuccess());

	EXPECT_TRUE((p->WaitForDeath()).IsSuccess());
}

TEST(CLProcess, NoWait)
{
	CLProcess *p = new CLProcess(new CLProcessFunctionForExec);

	CLEvent event("test_for_event_auto");

	EXPECT_TRUE((p->Run((void *)"../test_for_exec/test_for_CLProcess/main")).IsSuccess());

	EXPECT_TRUE(event.Wait().IsSuccess());
}