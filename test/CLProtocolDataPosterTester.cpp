#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLProtocolDataPoster, SetParameters_Features_Test)
{
	CLLogger::WriteLogMsg("CLProtocolDataPoster Test", 0);

	{
		CLProtocolDataPoster pd;
	}

	CLProtocolDataPoster pd;
	EXPECT_FALSE(pd.IsSetParameters());

	CLNamedPipe *pnp = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", true);
	EXPECT_TRUE(pd.SetParameters(new CLDataPosterByNamedPipe(pnp), new CLDataPostResultNotifier(false), 0).IsSuccess());
	EXPECT_TRUE(pd.IsSetParameters());

	EXPECT_FALSE(pd.SetParameters(new CLDataPosterByNamedPipe(pnp), new CLDataPostResultNotifier(false), 0).IsSuccess());
	EXPECT_FALSE(pd.SetParameters(0, new CLDataPostResultNotifier(false), 0).IsSuccess());
	EXPECT_FALSE(pd.SetParameters(new CLDataPosterByNamedPipe(pnp), 0, 0).IsSuccess());
	EXPECT_FALSE(pd.SetParameters(0, 0, 0).IsSuccess());
	
	delete pnp;
}

TEST(CLProtocolDataPoster, PostProtocolData_Parameter_Test)
{
	CLProtocolDataPoster *pd = new CLProtocolDataPoster;

	CLStatus s1 = pd->PostProtocolData(0);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, DATA_POSTER_POST_ERROR);

	CLNamedPipe *preader = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", true);
	CLNamedPipe *pwriter = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", false);
	EXPECT_TRUE(pd->SetParameters(new CLDataPosterByNamedPipe(pwriter), new CLDataPostResultNotifier(false), 0).IsSuccess());

	CLStatus s2 = pd->PostProtocolData(0);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, DATA_POSTER_POST_ERROR);

	CLIOVectors *piov = new CLIOVectors;
	CLStatus s3 = pd->PostProtocolData(piov);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, DATA_POSTER_POST_ERROR);
	
	delete pwriter;
	delete preader;
	delete pd;
}

TEST(CLProtocolDataPoster, PostProtocolData_Features_Test)
{
	CLProtocolDataPoster *pd = new CLProtocolDataPoster;

	CLEvent e;
	CLNamedPipe *preader = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", true);
	CLNamedPipe *pwriter = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", false);
	EXPECT_TRUE(pd->SetParameters(new CLDataPosterByNamedPipe(pwriter), new CLDataPostResultNotifier(false), &e).IsSuccess());

	CLIOVectors *piov = new CLIOVectors;
	long i = 32;
	EXPECT_TRUE(piov->PushBack((char *)(&i), 8).IsSuccess());
	CLStatus s1 = pd->PostProtocolData(piov);
	EXPECT_TRUE(s1.IsSuccess());

	EXPECT_TRUE(e.Wait().IsSuccess());

	long j = 0;
	CLIOVectors iov1;
	EXPECT_TRUE(iov1.PushBack((char *)(&j), 8).IsSuccess());
	EXPECT_TRUE(preader->Read(iov1).IsSuccess());
	EXPECT_EQ(j, 32);

	delete pwriter;
	delete preader;
}