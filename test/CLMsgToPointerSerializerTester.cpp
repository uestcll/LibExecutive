#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLMsgToPointerSerializer, Serialize_Features_Test)
{
	CLMsgToPointerSerializer dd;
	CLMessage *pMsg = (CLMessage *)1;
	CLIOVectors iov;

	EXPECT_FALSE(dd.Serialize(0, &iov).IsSuccess());
	EXPECT_FALSE(dd.Serialize(pMsg, 0).IsSuccess());

	EXPECT_TRUE(dd.Serialize(pMsg, &iov).IsSuccess());

	long p;
	EXPECT_TRUE(iov.ReadBlock(0, (char *)(&p), 8).IsSuccess());
	EXPECT_TRUE(p == (long)pMsg);
}