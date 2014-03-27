#include <gtest/gtest.h>
#include <string>
#include <stdio.h>
#include "LibExecutive.h"

using namespace std;

TEST(CLExecutiveNameServer, Register_Features_Test)
{
	CLLogger::WriteLogMsg("CLExecutiveNameServer Test", 0);

	const char *strPipeName = "/tmp/NamedPipe_For_CLExecutiveNameServer_Test";
	CLNamedPipe np(strPipeName, true);
	CLMessagePoster *mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);

	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_FALSE(p->Register(0, np.GetUuid(), 0).IsSuccess());
	
	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	EXPECT_FALSE(p->Register(0, np.GetUuid(), mp).IsSuccess());

	mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);
	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	EXPECT_TRUE(p->Register("SDFdfd", np.GetUuid(), mp).IsSuccess());

	EXPECT_TRUE(p->ReleaseCommunicationPtr("SDFdfd").IsSuccess());

	mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);
	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	EXPECT_TRUE(p->Register("23", np.GetUuid(), mp).IsSuccess());

	mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);

	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	EXPECT_FALSE(p->Register("23", np.GetUuid(), mp).IsSuccess());

	EXPECT_TRUE(p->ReleaseCommunicationPtr("23").IsSuccess());
}

TEST(CLExecutiveNameServer, UUID_Register_Features_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLExecutiveNameServer_Test";
	CLNamedPipe np(strPipeName, true);
	CLMessagePoster *mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);

	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_FALSE(p->Register(np.GetUuid(), 0).IsSuccess());

	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	EXPECT_TRUE(p->Register(np.GetUuid(), mp).IsSuccess());

	CLMessagePoster *pp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);
	EXPECT_TRUE(pp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	EXPECT_FALSE(p->Register(np.GetUuid(), pp).IsSuccess());

	EXPECT_TRUE(p->ReleaseCommunicationPtr(np.GetUuid()).IsSuccess());
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
	EXPECT_TRUE(p->Register("SDFdfd", np.GetUuid(), mp).IsSuccess());

	CLMessagePoster *qq = p->GetCommunicationPtr("SDFdfd");
	EXPECT_TRUE(qq == mp);
	EXPECT_TRUE(p->ReleaseCommunicationPtr("SDFdfd").IsSuccess());

	EXPECT_TRUE(p->ReleaseCommunicationPtr("SDFdfd").IsSuccess());
}

TEST(CLExecutiveNameServer, UUID_GetCommunicationPtr)
{
	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);
	
	CLUuid u1;

	EXPECT_EQ(p->GetCommunicationPtr(u1), (CLMessagePoster *)0);

	const char *strPipeName = "/tmp/NamedPipe_For_CLExecutiveNameServer_Test";
	CLNamedPipe np(strPipeName, true);
	CLMessagePoster *mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);
	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());
	EXPECT_TRUE(p->Register(np.GetUuid(), mp).IsSuccess());

	CLMessagePoster *qq = p->GetCommunicationPtr(np.GetUuid());
	EXPECT_TRUE(qq == mp);
	EXPECT_TRUE(p->ReleaseCommunicationPtr(np.GetUuid()).IsSuccess());

	EXPECT_TRUE(p->ReleaseCommunicationPtr(np.GetUuid()).IsSuccess());
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

TEST(CLExecutiveNameServer, UUID_ReleaseCommunicationPtr)
{
	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	CLLogger::WriteLogMsg("The Following bug is produced on purpose", 0);

	CLUuid u1;
	EXPECT_FALSE(p->ReleaseCommunicationPtr(u1).IsSuccess());
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

	CLLogger::WriteLogMsg("2 The Following bug is produced on purpose 2", 0);
	CLStatus s4 = CLExecutiveNameServer::PostExecutiveMessage("2323", new CLMessage(9));
	EXPECT_FALSE(s4.IsSuccess());
	EXPECT_EQ(s4.m_clErrorCode, NORMAL_ERROR);
}

TEST(CLExecutiveNameServer, UUID_PostExecutiveMessage_Parameters_Test)
{
	CLUuid u1;

	CLStatus s1 = CLExecutiveNameServer::PostExecutiveMessage(u1, 0);
	EXPECT_FALSE(s1.IsSuccess());
	EXPECT_EQ(s1.m_clErrorCode, NORMAL_ERROR);

	CLLogger::WriteLogMsg("2 The Following bug is produced on purpose 2", 0);

	CLStatus s2 = CLExecutiveNameServer::PostExecutiveMessage(u1, new CLMessage(9));
	EXPECT_FALSE(s2.IsSuccess());
	EXPECT_EQ(s2.m_clErrorCode, NORMAL_ERROR);
}

TEST(CLExecutiveNameServer, PostExecutiveMessage_Features_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLExecutiveNameServer_Test";
	CLNamedPipe np(strPipeName, true);
	CLMessagePoster *mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);

	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	EXPECT_TRUE(p->Register("SDFdfd1", np.GetUuid(), mp).IsSuccess());
	
	CLMessage *pmsg1 = new CLMessage(1);
	EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("SDFdfd1", pmsg1, true).IsSuccess());
	CLIOVectors iov;
	long j;
	EXPECT_TRUE(iov.PushBack((char *)(&j), 8).IsSuccess());
	EXPECT_TRUE(np.Read(iov).IsSuccess());
	EXPECT_EQ(j, (long)pmsg1);

	EXPECT_TRUE(p->ReleaseCommunicationPtr("SDFdfd1").IsSuccess());
}

TEST(CLExecutiveNameServer, UUID_PostExecutiveMessage_Features_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLExecutiveNameServer_Test";
	CLNamedPipe np(strPipeName, true);
	CLMessagePoster *mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);

	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	EXPECT_TRUE(p->Register(np.GetUuid(), mp).IsSuccess());

	CLMessage *pmsg1 = new CLMessage(1);
	EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(np.GetUuid(), pmsg1, true).IsSuccess());
	CLIOVectors iov;
	long j;
	EXPECT_TRUE(iov.PushBack((char *)(&j), 8).IsSuccess());
	EXPECT_TRUE(np.Read(iov).IsSuccess());
	EXPECT_EQ(j, (long)pmsg1);

	EXPECT_TRUE(p->ReleaseCommunicationPtr(np.GetUuid()).IsSuccess());
}

TEST(CLExecutiveNameServer, MsgReceiver_Features_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLExecutiveNameServer_Test";
	
	CLDataReceiver *pTemp = new CLDataReceiverByNamedPipe(strPipeName);
	CLUuid u1 = pTemp->GetUuid();
	CLMessageReceiver *mr = new CLMessageReceiver(&u1, new CLBufferManager(), pTemp, new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer);
	
	CLMessagePoster *mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);
	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_TRUE(p->Register("SDFdfd1", u1, mp, mr).IsSuccess());

	for(int i = 1; i < 40960 / 8; i++)
	{
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("SDFdfd1", new CLMessage(i)).IsSuccess());
	}

	EXPECT_TRUE(p->ReleaseCommunicationPtr("SDFdfd1").IsSuccess());
}

TEST(CLExecutiveNameServer, UUID_MsgReceiver_Features_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLExecutiveNameServer_Test";

	CLDataReceiver *pTemp = new CLDataReceiverByNamedPipe(strPipeName);
	CLUuid u1 = pTemp->GetUuid();
	CLMessageReceiver *mr = new CLMessageReceiver(&u1, new CLBufferManager(), pTemp, new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer);

	CLMessagePoster *mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);
	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_TRUE(p->Register(u1, mp, mr).IsSuccess());

	for(int i = 1; i < 40960 / 8; i++)
	{
		EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage(u1, new CLMessage(i)).IsSuccess());
	}

	EXPECT_TRUE(p->ReleaseCommunicationPtr(u1).IsSuccess());
}

TEST(CLExecutiveNameServer, MsgReceiver_Register_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLExecutiveNameServer_Test";

	CLDataReceiver *pTmp = new CLDataReceiverByNamedPipe(strPipeName);
	CLUuid u1 = pTmp->GetUuid();

	CLMessageReceiver *mr = new CLMessageReceiver(&u1, new CLBufferManager(), pTmp, new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer);

	CLMessagePoster *mp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);
	EXPECT_TRUE(mp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());

	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_FALSE(p->Register("", u1, mp, mr).IsSuccess());
}

TEST(CLExecutiveNameServer, UUID_MsgReceiver_Register_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLExecutiveNameServer_Test";

	CLDataReceiver *pTmp = new CLDataReceiverByNamedPipe(strPipeName);
	CLUuid u1 = pTmp->GetUuid();

	CLMessageReceiver *mr = new CLMessageReceiver(&u1, new CLBufferManager(), pTmp, new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer);

	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_FALSE(p->Register(u1, 0, mr).IsSuccess());
}

struct SLTesterForCLExecutiveNameServer
{
	string strName;
	CLUuid u1;
	CLMessagePoster *pp;
};

class CLThreadForCLExecutiveNameServerTest : public CLExecutiveFunctionProvider
{
public:
	virtual CLStatus RunExecutiveFunction(void* pContext)
	{
		SLTesterForCLExecutiveNameServer *pp = (SLTesterForCLExecutiveNameServer *)pContext;

		CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
		EXPECT_NE(p, (CLExecutiveNameServer *)0);

		EXPECT_TRUE(p->Register(pp->strName.c_str(), pp->u1, pp->pp).IsSuccess());
	}
};

TEST(CLExecutiveNameServer, MultiThread_Test)
{
	const char *strPipeName = "/tmp/NamedPipe_For_CLExecutiveNameServer_Test";
	CLNamedPipe np(strPipeName, true);

	SLTesterForCLExecutiveNameServer a[30];
	for(int i = 0; i < 30; i++)
	{
		char buf[10];
		sprintf(buf, "%d", i);
		a[i].strName = buf;
		a[i].u1 = CLUuid();
		a[i].pp = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelByNamedPipeMaintainer(strPipeName), 0);
		EXPECT_TRUE(a[i].pp->Initialize(new CLInitialDataPostChannelNotifier, 0).IsSuccess());
	}

	for(int i = 0; i < 30; i++)
	{
		CLThread *p = new CLThread(new CLThreadForCLExecutiveNameServerTest, false);
		EXPECT_TRUE(p->Run(&(a[i])).IsSuccess());
	}

	sleep(10);

	for(int i = 0; i < 30; i++)
	{
		CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
		EXPECT_NE(p, (CLExecutiveNameServer *)0);

		EXPECT_EQ(p->GetCommunicationPtr(a[i].strName.c_str()), a[i].pp);

		EXPECT_TRUE(p->ReleaseCommunicationPtr(a[i].strName.c_str()).IsSuccess());
		EXPECT_TRUE(p->ReleaseCommunicationPtr(a[i].strName.c_str()).IsSuccess());
	}
}