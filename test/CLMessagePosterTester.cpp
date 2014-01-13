#include <gtest/gtest.h>
#include <signal.h>
#include <string.h>
#include "LibExecutive.h"

class CLProtocolEncapsulator_Test_For_CLMessagePoster : public CLProtocolEncapsulator
{
public:
	virtual CLStatus Initialize(uuid_t uuidOfCommObj)
	{
		return CLStatus(bSucc, 0);
	}

	virtual CLStatus Uninitialize()
	{
		return CLStatus(bSucc, 0);
	}

	virtual CLStatus Encapsulate(CLIOVectors *pIOVectors)
	{
		if(bSucc == 20)
		{
			char *p = new char [64*1024];
			memset(p, 0, 64*1024);
			EXPECT_TRUE(pIOVectors->PushBack(p, 64*1024, true).IsSuccess());
			return CLStatus(0, 0);
		}
		else
			return CLStatus(bSucc, 0);
	}

	long bSucc;
};

TEST(CLMessagePoster, Initialize_Feature_Test)
{
	CLLogger::WriteLogMsg("CLMessagePoster Test", 0);

	const char *strPipeName = "/tmp/NamedPipe_For_CLMessagePoster_Test";
	CLNamedPipe np(strPipeName, true);

	CLEvent *pEvent = new CLEvent;
	CLProtocolEncapsulator_Test_For_CLMessagePoster *pE = new CLProtocolEncapsulator_Test_For_CLMessagePoster;
	CLMessagePoster mp(new CLMsgToPointerSerializer, pE, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), pEvent);

	CLStatus s1 = mp.Initialize(0, 0);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, MSG_INITIALIZE_ERROR);

	pE->bSucc = -1;
	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	CLStatus s4 = mp.Initialize(new CLInitialDataPostChannelNotifier, 0);
	EXPECT_FALSE(s4.IsSuccess());
	EXPECT_EQ(s4.m_clErrorCode, MSG_INITIALIZE_ERROR);

	pE->bSucc = 0;
	CLStatus s2 = mp.Initialize(new CLInitialDataPostChannelNotifier, 0);
	EXPECT_TRUE(s2.IsSuccess());

	CLStatus s3 = mp.Initialize(new CLInitialDataPostChannelNotifier, 0);
	EXPECT_TRUE(s3.IsSuccess());
}

TEST(CLMessagePoster, Uninitialize_Feature_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLMessagePoster_Test";
	CLNamedPipe np(strPipeName, true);

	CLEvent *pEvent = new CLEvent;
	CLProtocolEncapsulator_Test_For_CLMessagePoster *pE = new CLProtocolEncapsulator_Test_For_CLMessagePoster;
	pE->bSucc = 0;
	CLMessagePoster mp(new CLMsgToPointerSerializer, pE, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), pEvent);

	CLStatus s1 = mp.Uninitialize(0);
	EXPECT_FALSE(s1.IsSuccess());

	EXPECT_TRUE(mp.Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	pE->bSucc = -1;
	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	CLStatus s2 = mp.Uninitialize(0);
	EXPECT_FALSE(s2.IsSuccess());

	pE->bSucc = 0;
	CLStatus s3 = mp.Uninitialize(0);
	EXPECT_TRUE(s3.IsSuccess());
}

TEST(CLMessagePoster, PostMessage_Parameters_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLMessagePoster_Test";
	CLNamedPipe np(strPipeName, true);

	CLEvent *pEvent = new CLEvent;
	CLProtocolEncapsulator_Test_For_CLMessagePoster *pE = new CLProtocolEncapsulator_Test_For_CLMessagePoster;
	pE->bSucc = 0;
	CLMessagePoster mp(new CLMsgToPointerSerializer, pE, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), pEvent);

	EXPECT_TRUE(mp.Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	CLStatus s1 = mp.PostMessage(0, 0, 0);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, MSG_POST_ERROR);

	CLStatus s2 = mp.PostMessage(0, new CLDataPostResultNotifier(true), 0);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, MSG_POST_ERROR);

	CLStatus s3 = mp.PostMessage(new CLMessage(1), new CLDataPostResultNotifier(true), 0);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, MSG_POST_ERROR);
}

TEST(CLMessagePoster, PostMessage_Features_Test)
{
	signal(SIGPIPE, SIG_IGN);
	const char *strPipeName = "/tmp/NamedPipe_For_CLMessagePoster_Test";
	CLNamedPipe *np = new CLNamedPipe(strPipeName, true);

	CLEvent *pEvent = new CLEvent;
	CLProtocolEncapsulator_Test_For_CLMessagePoster *pE = new CLProtocolEncapsulator_Test_For_CLMessagePoster;
	pE->bSucc = 0;
	CLMessagePoster mp(new CLMsgToPointerSerializer, pE, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), pEvent);

	EXPECT_TRUE(mp.Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	long i = 32;
	CLProtocolDataPoster *ppd = new CLProtocolDataPoster();
	CLStatus s1 = mp.PostMessage((CLMessage *)(i), new CLDataPostResultNotifier(), ppd);
	EXPECT_EQ(s1.m_clReturnCode, 0);
	EXPECT_EQ(s1.m_clErrorCode, 0);

	CLIOVectors iov;
	long j;
	EXPECT_TRUE(iov.PushBack((char *)(&j), 8).IsSuccess());
	EXPECT_TRUE(np->Read(iov).IsSuccess());
	EXPECT_EQ(j, i);

	pE->bSucc = -1;
	i = 32;
	ppd = new CLProtocolDataPoster();
	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	CLStatus s2 = mp.PostMessage((CLMessage *)(i), new CLDataPostResultNotifier(), ppd);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, MSG_POST_ERROR);

	delete np;
	pE->bSucc = 0;
	i = 32;
	ppd = new CLProtocolDataPoster();
	CLLogger::WriteLogMsg("The Following bug is produced on purpose 4", 0);
	CLStatus s3 = mp.PostMessage((CLMessage *)(i), new CLDataPostResultNotifier(), ppd);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, MSG_POST_ERROR);
}

TEST(CLMessagePoster, PostMessage_PENDING_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLMessagePoster_Test";
	CLNamedPipe *np = new CLNamedPipe(strPipeName, true);

	CLEvent *pEvent = new CLEvent;
	CLProtocolEncapsulator_Test_For_CLMessagePoster *pE = new CLProtocolEncapsulator_Test_For_CLMessagePoster;
	pE->bSucc = 0;
	CLMessagePoster mp(new CLMsgToPointerSerializer, pE, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), pEvent);

	EXPECT_TRUE(mp.Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	for(long i = 0; i < 8*1024; i++)
	{
		CLProtocolDataPoster *ppd = new CLProtocolDataPoster();
		CLStatus s1 = mp.PostMessage((CLMessage *)(i+1), new CLDataPostResultNotifier(), ppd);
		EXPECT_EQ(s1.m_clReturnCode, 0);
		EXPECT_EQ(s1.m_clErrorCode, 0);
	}

	CLProtocolDataPoster *ppd = new CLProtocolDataPoster();
	CLStatus s1 = mp.PostMessage((CLMessage *)(1), new CLDataPostResultNotifier(), ppd);
	EXPECT_EQ(s1.m_clReturnCode, -1);
	EXPECT_EQ(s1.m_clErrorCode, MSG_POST_PENDING);

	CLStatus s2 = mp.PostMessage((CLMessage *)(1), new CLDataPostResultNotifier(), ppd);
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, MSG_POST_ERROR);

	CLIOVectors iov;
	long j;
	EXPECT_TRUE(iov.PushBack((char *)(&j), 8).IsSuccess());
	for(long i = 0; i < 8*1024; i++)
	{
		EXPECT_TRUE(np->Read(iov).IsSuccess());
		EXPECT_EQ(j, i+1);
	}

	delete np;
}

TEST(CLMessagePoster, PostMessage_PARTIAL_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLMessagePoster_Test";
	CLNamedPipe *np = new CLNamedPipe(strPipeName, true);

	CLEvent *pEvent = new CLEvent;
	CLProtocolEncapsulator_Test_For_CLMessagePoster *pE = new CLProtocolEncapsulator_Test_For_CLMessagePoster;
	pE->bSucc = 20;
	CLMessagePoster mp(new CLMsgToPointerSerializer, pE, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), pEvent);

	EXPECT_TRUE(mp.Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	CLProtocolDataPoster *ppd = new CLProtocolDataPoster();
	CLStatus s1 = mp.PostMessage((CLMessage *)1, new CLDataPostResultNotifier(), ppd);
	EXPECT_EQ(s1.m_clReturnCode, -1);
	EXPECT_EQ(s1.m_clErrorCode, MSG_POST_PARTIAL);

	delete ppd;

	delete np;
}

TEST(CLMessagePoster, PostLeftMessage_SUCC_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLMessagePoster_Test";
	CLNamedPipe *np = new CLNamedPipe(strPipeName, true);

	CLEvent *pEvent = new CLEvent;
	CLProtocolEncapsulator_Test_For_CLMessagePoster *pE = new CLProtocolEncapsulator_Test_For_CLMessagePoster;
	pE->bSucc = 20;
	CLMessagePoster mp(new CLMsgToPointerSerializer, pE, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), pEvent);

	EXPECT_TRUE(mp.Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	CLStatus s3 = mp.PostLeftMessage(0);
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, MSG_POST_ERROR);

	CLProtocolDataPoster *ppd1 = new CLProtocolDataPoster();
	CLStatus s4 = mp.PostLeftMessage(ppd1);
	EXPECT_FALSE(s4.IsSuccess());
	EXPECT_EQ(s4.m_clErrorCode, MSG_POST_ERROR);

	CLProtocolDataPoster *ppd = new CLProtocolDataPoster();
	CLStatus s1 = mp.PostMessage((CLMessage *)1, new CLDataPostResultNotifier(), ppd);
	EXPECT_EQ(s1.m_clReturnCode, -1);
	EXPECT_EQ(s1.m_clErrorCode, MSG_POST_PARTIAL);

	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack(new char [4096], 4096, true).IsSuccess());
	EXPECT_TRUE(np->Read(iov).IsSuccess());

	CLStatus s2 = mp.PostLeftMessage(ppd);
	EXPECT_EQ(s2.m_clReturnCode, 0);
	EXPECT_EQ(s2.m_clErrorCode, 0);

	delete np;
}

TEST(CLMessagePoster, PostLeftMessage_PENDING_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLMessagePoster_Test";
	CLNamedPipe *np = new CLNamedPipe(strPipeName, true);

	CLEvent *pEvent = new CLEvent;
	CLProtocolEncapsulator_Test_For_CLMessagePoster *pE = new CLProtocolEncapsulator_Test_For_CLMessagePoster;
	pE->bSucc = 20;
	CLMessagePoster mp(new CLMsgToPointerSerializer, pE, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), pEvent);

	EXPECT_TRUE(mp.Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	CLProtocolDataPoster *ppd = new CLProtocolDataPoster();
	CLStatus s1 = mp.PostMessage((CLMessage *)1, new CLDataPostResultNotifier(), ppd);
	EXPECT_EQ(s1.m_clReturnCode, -1);
	EXPECT_EQ(s1.m_clErrorCode, MSG_POST_PARTIAL);

	CLStatus s2 = mp.PostLeftMessage(ppd);
	EXPECT_EQ(s2.m_clReturnCode, -1);
	EXPECT_EQ(s2.m_clErrorCode, MSG_POST_PENDING);

	delete ppd;

	delete np;
}

TEST(CLMessagePoster, PostLeftMessage_PARTIAL_Test)
{
	//...........
}