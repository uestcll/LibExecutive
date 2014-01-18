#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLExecutiveNameServer, Register_Features_Test)
{
	CLLogger::WriteLogMsg("CLExecutiveNameServer Test", 0);

	const char *strPipeName = "/tmp/NamedPipe_For_CLExecutiveNameServer_Test";
	CLNamedPipe np(strPipeName, true);
	CLMessagePoster *mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);

	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_FALSE(p->Register(0, 0).IsSuccess());

	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	EXPECT_FALSE(p->Register(0, mp).IsSuccess());

	mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);
	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	EXPECT_TRUE(p->Register("SDFdfd", mp).IsSuccess());

	EXPECT_TRUE(p->ReleaseCommunicationPtr("SDFdfd").IsSuccess());

	mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);
	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	EXPECT_TRUE(p->Register("23", mp).IsSuccess());

	mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);

	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	EXPECT_FALSE(p->Register("23", mp).IsSuccess());

	EXPECT_TRUE(p->ReleaseCommunicationPtr("23").IsSuccess());
}

TEST(CLExecutiveNameServer, GetCommunicationPtr)
{
	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_EQ(p->GetCommunicationPtr(0), (CLMessagePoster *)0);
	EXPECT_EQ(p->GetCommunicationPtr(""), (CLMessagePoster *)0);
	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	EXPECT_EQ(p->GetCommunicationPtr("3423"), (CLMessagePoster *)0);

	const char *strPipeName = "/tmp/NamedPipe_For_CLExecutiveNameServer_Test";
	CLNamedPipe np(strPipeName, true);
	CLMessagePoster *mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);
	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());
	EXPECT_TRUE(p->Register("SDFdfd", mp).IsSuccess());

	CLMessagePoster *qq = p->GetCommunicationPtr("SDFdfd");
	EXPECT_TRUE(qq == mp);
	EXPECT_TRUE(p->ReleaseCommunicationPtr("SDFdfd").IsSuccess());

	EXPECT_TRUE(p->ReleaseCommunicationPtr("SDFdfd").IsSuccess());
}

TEST(CLExecutiveNameServer, ReleaseCommunicationPtr)
{
	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_FALSE(p->ReleaseCommunicationPtr(0).IsSuccess());
	EXPECT_FALSE(p->ReleaseCommunicationPtr("").IsSuccess());

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	EXPECT_FALSE(p->ReleaseCommunicationPtr("1").IsSuccess());
}

TEST(CLExecutiveNameServer, PostExecutiveMessage_Parameters_Test)
{
	CLStatus s1 = CLExecutiveNameServer::PostExecutiveMessage(0, 0);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	CLStatus s2 = CLExecutiveNameServer::PostExecutiveMessage(0, new CLMessage(9));
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);

	CLStatus s3 = CLExecutiveNameServer::PostExecutiveMessage("", new CLMessage(9));
	EXPECT_FALSE(s3.IsSuccess());
	EXPECT_EQ(s3.m_clErrorCode, NORMAL_ERROR);

	CLLogger::WriteLogMsg("The Following bug is produced on purpose 2", 0);
	CLStatus s4 = CLExecutiveNameServer::PostExecutiveMessage("2323", new CLMessage(9));
	EXPECT_FALSE(s4.IsSuccess());
	EXPECT_EQ(s4.m_clErrorCode, NORMAL_ERROR);
}

TEST(CLExecutiveNameServer, PostExecutiveMessage_Features_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLExecutiveNameServer_Test";
	CLNamedPipe np(strPipeName, true);
	CLMessagePoster *mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);

	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	EXPECT_TRUE(p->Register("SDFdfd1", mp).IsSuccess());
	
	CLMessage *pmsg1 = new CLMessage(1);
	EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("SDFdfd1", pmsg1, true).IsSuccess());
	CLIOVectors iov;
	long j;
	EXPECT_TRUE(iov.PushBack((char *)(&j), 8).IsSuccess());
	EXPECT_TRUE(np.Read(iov).IsSuccess());
	EXPECT_EQ(j, (long)pmsg1);

	EXPECT_TRUE(p->ReleaseCommunicationPtr("SDFdfd1").IsSuccess());
}