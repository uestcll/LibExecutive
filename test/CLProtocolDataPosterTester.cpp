#include <gtest/gtest.h>
#include <signal.h>
#include <string.h>
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

TEST(CLProtocolDataPoster, PostProtocolData_POSTERROR_Test)
{
	signal(SIGPIPE, SIG_IGN);

	CLProtocolDataPoster *pd = new CLProtocolDataPoster;

	CLEvent e;
	CLNamedPipe *preader = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", true);
	CLNamedPipe *pwriter = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", false);
	EXPECT_TRUE(pd->SetParameters(new CLDataPosterByNamedPipe(pwriter), new CLDataPostResultNotifier(false), &e).IsSuccess());
	delete preader;

	CLIOVectors *piov = new CLIOVectors;
	long i = 32;
	EXPECT_TRUE(piov->PushBack((char *)(&i), 8).IsSuccess());
	CLLogger::WriteLogMsg("3 The Following bug is produced on purpose 3", 0);
	CLStatus s1 = pd->PostProtocolData(piov);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, DATA_POSTER_POST_ERROR);

	delete pwriter;
}

TEST(CLProtocolDataPoster, PostProtocolData_POSTPENDING_Test)
{
	CLProtocolDataPoster *pd = new CLProtocolDataPoster;

	CLEvent e;
	CLNamedPipe *preader = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", true);
	CLNamedPipe *pwriter = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", false);
	EXPECT_TRUE(pd->SetParameters(new CLDataPosterByNamedPipe(pwriter), new CLDataPostResultNotifier(false), &e).IsSuccess());
	
	CLIOVectors *piov = new CLIOVectors;
	char *pbuf = new char[64 * 1024];
	memset(pbuf, 0, 64 * 1024);
	EXPECT_TRUE(piov->PushBack(pbuf, 64*1024, true).IsSuccess());
	CLStatus s1 = pd->PostProtocolData(piov);
	EXPECT_TRUE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clReturnCode, 0);

	pd = new CLProtocolDataPoster;
	EXPECT_TRUE(pd->SetParameters(new CLDataPosterByNamedPipe(pwriter), new CLDataPostResultNotifier(false), &e).IsSuccess());

	piov = new CLIOVectors;
	long i = 32;
	EXPECT_TRUE(piov->PushBack((char *)(&i), 8).IsSuccess());
	CLStatus s2 = pd->PostProtocolData(piov);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, DATA_POSTER_POST_PENDING);

	delete pd;

	delete pwriter;
	delete preader;
}

TEST(CLProtocolDataPoster, PostProtocolData_POSTPARTIAL_Test)
{
	CLProtocolDataPoster *pd = new CLProtocolDataPoster;

	CLEvent e;
	CLNamedPipe *preader = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", true);
	CLNamedPipe *pwriter = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", false);
	EXPECT_TRUE(pd->SetParameters(new CLDataPosterByNamedPipe(pwriter), new CLDataPostResultNotifier(false), &e).IsSuccess());

	CLIOVectors *piov = new CLIOVectors;
	char *pbuf = new char[64 * 1024 + 20];
	memset(pbuf, 0, 64 * 1024 + 20);
	EXPECT_TRUE(piov->PushBack(pbuf, 64*1024+20, true).IsSuccess());
	CLStatus s1 = pd->PostProtocolData(piov);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, DATA_POSTER_POST_PARTIAL);

	delete pd;

	delete pwriter;
	delete preader;
}

TEST(CLProtocolDataPoster, PostLeftProtocolData_POSTPENDING_Test)
{
	CLProtocolDataPoster *pd = new CLProtocolDataPoster;

	CLStatus s1 = pd->PostLeftProtocolData();
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, DATA_POSTER_POST_ERROR);

	CLEvent e;
	CLNamedPipe *preader = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", true);
	CLNamedPipe *pwriter = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", false);
	EXPECT_TRUE(pd->SetParameters(new CLDataPosterByNamedPipe(pwriter), new CLDataPostResultNotifier(false), &e).IsSuccess());

	CLIOVectors *piov = new CLIOVectors;
	char *pbuf = new char[64 * 1024 + 20];
	for(int i = 0; i < 64 * 1024 + 20; i++)
	{
		if(i % 2 == 0)
			pbuf[i] = 0;
		else
			pbuf[i] = 1;
	}
	EXPECT_TRUE(piov->PushBack(pbuf, 64*1024+20, true).IsSuccess());
	CLStatus s2 = pd->PostProtocolData(piov);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, DATA_POSTER_POST_PARTIAL);

	CLStatus s3 = pd->PostLeftProtocolData();
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, DATA_POSTER_POST_PENDING);

	CLIOVectors iov;
	char *p = new char [64 * 1024];
	EXPECT_TRUE(iov.PushBack(p, 64 * 1024, true).IsSuccess());
	EXPECT_TRUE(preader->Read(iov).IsSuccess());
	for(int i = 0; i < 64 * 1024; i++)
	{
		if(i % 2 == 0)
		{
			EXPECT_TRUE(p[i] == 0);
		}
		else
		{
			EXPECT_TRUE(p[i] == 1);
		}
	}

	CLStatus s4 = pd->PostLeftProtocolData();
	EXPECT_EQ(s4.m_clReturnCode, 0);
	EXPECT_EQ(s4.m_clErrorCode, 0);

	memset(p, 0, 20);

	EXPECT_TRUE(preader->Read(iov).IsSuccess());
	for(int i = 0; i < 20; i++)
	{
		if(i % 2 == 0)
		{
			EXPECT_TRUE(p[i] == 0);
		}
		else
		{
			EXPECT_TRUE(p[i] == 1);
		}
	}

	delete pwriter;
	delete preader;
}

TEST(CLProtocolDataPoster, PostLeftProtocolData_POSTERROR_Test)
{
	CLProtocolDataPoster *pd = new CLProtocolDataPoster;

	CLEvent e;
	CLNamedPipe *preader = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", true);
	CLNamedPipe *pwriter = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", false);
	EXPECT_TRUE(pd->SetParameters(new CLDataPosterByNamedPipe(pwriter), new CLDataPostResultNotifier(false), &e).IsSuccess());

	CLIOVectors *piov = new CLIOVectors;
	char *pbuf = new char[64 * 1024 + 20];
	for(int i = 0; i < 64 * 1024 + 20; i++)
	{
		if(i % 2 == 0)
			pbuf[i] = 0;
		else
			pbuf[i] = 1;
	}
	EXPECT_TRUE(piov->PushBack(pbuf, 64*1024+20, true).IsSuccess());
	CLStatus s1 = pd->PostProtocolData(piov);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, DATA_POSTER_POST_PARTIAL);

	delete preader;

	CLLogger::WriteLogMsg("3 The Following bug is produced on purpose 3", 0);

	CLStatus s2 = pd->PostLeftProtocolData();
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, DATA_POSTER_POST_ERROR);

	delete pwriter;
}

TEST(CLProtocolDataPoster, PostLeftProtocolData_POSTSUCC_Test)
{
	CLProtocolDataPoster *pd = new CLProtocolDataPoster;

	CLEvent e;
	CLNamedPipe *preader = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", true);
	CLNamedPipe *pwriter = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", false);
	EXPECT_TRUE(pd->SetParameters(new CLDataPosterByNamedPipe(pwriter), new CLDataPostResultNotifier(false), &e).IsSuccess());

	CLIOVectors *piov = new CLIOVectors;
	char *pbuf = new char[64 * 1024 + 20];
	for(int i = 0; i < 64 * 1024 + 20; i++)
	{
		if(i % 2 == 0)
			pbuf[i] = 0;
		else
			pbuf[i] = 1;
	}
	EXPECT_TRUE(piov->PushBack(pbuf, 64*1024+20, true).IsSuccess());
	CLStatus s1 = pd->PostProtocolData(piov);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, DATA_POSTER_POST_PARTIAL);

	CLIOVectors iov;
	char *p = new char [64 * 1024];
	EXPECT_TRUE(iov.PushBack(p, 64 * 1024, true).IsSuccess());
	EXPECT_TRUE(preader->Read(iov).IsSuccess());
	for(int i = 0; i < 64 * 1024; i++)
	{
		if(i % 2 == 0)
		{
			EXPECT_TRUE(p[i] == 0);
		}
		else
		{
			EXPECT_TRUE(p[i] == 1);
		}
	}

	CLStatus s2 = pd->PostLeftProtocolData();
	EXPECT_EQ(s2.m_clReturnCode, 0);
	EXPECT_EQ(s2.m_clErrorCode, 0);

	CLIOVectors iov1;
	p = new char [20];
	EXPECT_TRUE(iov1.PushBack(p, 20, true).IsSuccess());
	EXPECT_TRUE(preader->Read(iov1).IsSuccess());
	for(int i = 0; i < 20; i++)
	{
		if(i % 2 == 0)
		{
			EXPECT_TRUE(p[i] == 0);
		}
		else
		{
			EXPECT_TRUE(p[i] == 1);
		}
	}

	delete pwriter;
	delete preader;
}

TEST(CLProtocolDataPoster, PostLeftProtocolData_POSTPARTIAL_Test)
{
	CLProtocolDataPoster *pd = new CLProtocolDataPoster;

	CLEvent e;
	CLNamedPipe *preader = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", true);
	CLNamedPipe *pwriter = new CLNamedPipe("/tmp/NamedPipe_for_CLProtocolDataPosterTest", false);
	EXPECT_TRUE(pd->SetParameters(new CLDataPosterByNamedPipe(pwriter), new CLDataPostResultNotifier(false), &e).IsSuccess());

	CLIOVectors *piov = new CLIOVectors;
	char *pbuf = new char[64 * 1024 * 3];
	for(int i = 0; i < 64 * 1024 * 3; i++)
	{
		if(i % 2 == 0)
			pbuf[i] = 0;
		else
			pbuf[i] = 1;
	}
	EXPECT_TRUE(piov->PushBack(pbuf, 64*1024*3, true).IsSuccess());
	CLStatus s1 = pd->PostProtocolData(piov);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, DATA_POSTER_POST_PARTIAL);

	CLIOVectors iov;
	char *p = new char [64 * 1024];
	EXPECT_TRUE(iov.PushBack(p, 64 * 1024, true).IsSuccess());
	EXPECT_TRUE(preader->Read(iov).IsSuccess());
	for(int i = 0; i < 64 * 1024; i++)
	{
		if(i % 2 == 0)
		{
			EXPECT_TRUE(p[i] == 0);
		}
		else
		{
			EXPECT_TRUE(p[i] == 1);
		}
	}

	CLStatus s2 = pd->PostLeftProtocolData();
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, DATA_POSTER_POST_PARTIAL);
	EXPECT_TRUE(preader->Read(iov).IsSuccess());
	for(int i = 0; i < 64 * 1024; i++)
	{
		if(i % 2 == 0)
		{
			EXPECT_TRUE(p[i] == 0);
		}
		else
		{
			EXPECT_TRUE(p[i] == 1);
		}
	}

	CLStatus s3 = pd->PostLeftProtocolData();
	EXPECT_EQ(s3.m_clReturnCode, 0);
	EXPECT_EQ(s3.m_clErrorCode, 0);
	EXPECT_TRUE(preader->Read(iov).IsSuccess());
	for(int i = 0; i < 64 * 1024; i++)
	{
		if(i % 2 == 0)
		{
			EXPECT_TRUE(p[i] == 0);
		}
		else
		{
			EXPECT_TRUE(p[i] == 1);
		}
	}
	
	delete pwriter;
	delete preader;
}

TEST(CLProtocolDataPoster, STL_PostProtocolData_Features_Test)
{
	CLProtocolDataPoster *pd = new CLProtocolDataPoster;
	CLEvent e;
	CLSTLqueue *q = new CLSTLqueue;
	
	EXPECT_TRUE(pd->SetParameters(new CLDataPosterBySTLqueue(q), new CLDataPostResultNotifier(false), &e).IsSuccess());

	CLIOVectors *piov = new CLIOVectors;
	long i = 32;
	EXPECT_TRUE(piov->PushBack((char *)(&i), 8).IsSuccess());
	CLStatus s1 = pd->PostProtocolData(piov);
	EXPECT_TRUE(s1.IsSuccess());

	EXPECT_TRUE(e.Wait().IsSuccess());

	long j = 0;
	CLIOVectors iov1;
	EXPECT_TRUE(iov1.PushBack((char *)(&j), 8).IsSuccess());
	EXPECT_TRUE(q->PopData(iov1).IsSuccess());
	EXPECT_EQ(j, 32);

	delete q;
}