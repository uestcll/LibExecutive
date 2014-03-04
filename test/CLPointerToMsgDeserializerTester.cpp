#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLPointerToMsgDeserializer, Deserialize_Features_Test)
{
	CLLogger::WriteLogMsg("CLPointerToMsgDeserializer Test", 0);

	CLBufferManager bm;
	CLMessage *pMsg=(CLMessage *)1;
	CLIOVectors iov;
	CLPointerToMsgDeserializer pt;

	EXPECT_FALSE(pt.Deserialize(iov, &pMsg, bm, 0).IsSuccess());
	EXPECT_TRUE(pMsg == 0);
	pMsg=(CLMessage *)1;

	char buf[8];
	for(int i = 0; i < 8; i++)
		buf[i] = 0;

	EXPECT_TRUE(iov.PushBack(buf, 8).IsSuccess());
	EXPECT_FALSE(pt.Deserialize(iov, &pMsg, bm, 0).IsSuccess());
	EXPECT_TRUE(pMsg == 0);
	pMsg=(CLMessage *)1;

	buf[0] = 10;
	EXPECT_TRUE(pt.Deserialize(iov, &pMsg, bm, 0).IsSuccess());
	EXPECT_TRUE(pMsg == (CLMessage *)10);
}