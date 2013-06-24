#include <gtest/gtest.h>
#include "CLStatus.h"

CLStatus f1()
{
	return CLStatus(0, 2);
}

TEST(CLStatus, ReturnCode)
{
	CLStatus s = f1();
	EXPECT_GE(s.m_clReturnCode, 0);
	EXPECT_TRUE(s.IsSuccess());
}

CLStatus f2()
{
	return CLStatus(-1, 2);
}

TEST(CLStatus,  ReturnCodeFailure)
{
	CLStatus s = f2();
	EXPECT_LT(s.m_clReturnCode, 0);
	EXPECT_FALSE(s.IsSuccess());
	EXPECT_EQ(s.m_clErrorCode, 2);
}
