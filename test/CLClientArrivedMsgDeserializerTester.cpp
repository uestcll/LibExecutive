#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLClientArrivedMsgDeserializer, Deserialize_Features_Test)
{
	CLLogger::WriteLogMsg("CLClientArrivedMsgDeserializer Test", 0);

	CLBufferManager bm;
	CLMessage *pMsg=(CLMessage *)1;
	CLIOVectors iov;
	CLClientArrivedMsgDeserializer pt;

	EXPECT_FALSE(pt.Deserialize(iov, &pMsg, bm).IsSuccess());
	EXPECT_TRUE(pMsg == 0);
	pMsg=(CLMessage *)1;

	long sock_addr = 2;
	EXPECT_TRUE(iov.PushBack((char *)(&sock_addr), 8).IsSuccess());
	EXPECT_TRUE(pt.Deserialize(iov, &pMsg, bm).IsSuccess());
	EXPECT_TRUE(pMsg != 0);

	CLClientArrivedMsg *pc = dynamic_cast<CLClientArrivedMsg *>(pMsg);
	EXPECT_TRUE(pc != 0);
	EXPECT_TRUE(((long)pc->GetSocket() == 2));

	delete pc;

	sock_addr = 0;
	EXPECT_FALSE(pt.Deserialize(iov, &pMsg, bm).IsSuccess());
	EXPECT_TRUE(pMsg == 0);
}