#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLProcess, Normal)
{
	CLProcess *p = new CLProcess(new CLProcessFunctionForExec);

	CLEvent event("test_for_event_auto");

	EXPECT_TRUE((p->Run((void *)"../test_for_exec/test_for_CLProcess/main")).IsSuccess());

	EXPECT_TRUE(event.Wait().IsSuccess());
}