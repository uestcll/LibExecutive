#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLDataReceiverBySTLqueue, GetData_Features_Test)
{
	CLLogger::WriteLogMsg("CLDataReceiverBySTLqueue Test", 0);

	CLSTLqueue qt;
	CLDataReceiverBySTLqueue q_stl(&qt);

	CLIOVectors iov;
	long context = 1;

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	CLStatus s1 = q_stl.GetData(iov, &context);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);
	EXPECT_EQ(context, 0);
	context = 1;

	char buf[32];
	EXPECT_TRUE(iov.PushBack(buf, 32).IsSuccess());
	CLStatus s2 = q_stl.GetData(iov, &context);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, RECEIVED_ZERO_BYTE);
	EXPECT_EQ(context, 0);
	context = 1;

	for(unsigned long i = 0; i < 10; i++)
		qt.PushData(i);

	CLStatus s3 = q_stl.GetData(iov, &context);
	EXPECT_EQ(s3.m_clReturnCode, 32);
	for(int j = 0; j < 32; j++)
	{
		if(j == 8)
			EXPECT_EQ((int)buf[j], 1);
		else if(j == 16)
			EXPECT_EQ((int)buf[j], 2);
		else if(j == 24)
			EXPECT_EQ((int)buf[j], 3);
		else
			EXPECT_EQ((int)buf[j], 0);
	}
	EXPECT_EQ(context, 0);
}