#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLProcess, Run_Parameters_Test)
{
	CLLogger::WriteLogDirectly("CLProcess Test", 0);

	CLProcess *p1 = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_FALSE((p1->Run((void *)0)).IsSuccess());

	CLProcess *p2 = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_FALSE((p2->Run((void *)"")).IsSuccess());
}

TEST(CLProcess, Run_Features_Test)
{
	CLProcess *p = new CLProcess(new CLProcessFunctionForExec);

	CLEvent event("test_for_event_auto");

	EXPECT_TRUE((p->Run((void *)"../test_for_exec/test_for_CLProcess/main")).IsSuccess());

	EXPECT_TRUE(event.Wait().IsSuccess());
}