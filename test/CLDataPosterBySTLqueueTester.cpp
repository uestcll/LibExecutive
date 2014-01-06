#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLDataPosterBySTLqueue, PostData_Features_Test)
{
	CLSTLqueue q;
	CLDataPosterBySTLqueue dp(&q);

	CLStatus s1 = dp.PostData(0);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, DATA_POSTER_POST_ERROR);

	CLIOVectors iov;
	CLStatus s2 = dp.PostData(&iov);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, DATA_POSTER_POST_ERROR);

	long i = 34;
	EXPECT_TRUE(iov.PushBack((char *)(&i), 8).IsSuccess());
	CLStatus s3 = dp.PostData(&iov);
	EXPECT_TRUE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clReturnCode, 8);
}