#include <gtest/gtest.h>
#include "LibExecutive.h"

static int g_bfornameserver = 0;

static int g_bforpst = 0;

class CLFalseCommunication : public CLExecutiveCommunication
{
public:
	virtual CLStatus PostExecutiveMessage(CLMessage *pMessage)
	{
		g_bforpst++;
		return CLStatus(0, 0);
	}

	virtual ~CLFalseCommunication()
	{
		g_bfornameserver++;
	}
};


TEST(CLExecutiveNameServer, normal)
{
	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	CLExecutiveNameServer *p1 = CLExecutiveNameServer::GetInstance();

	EXPECT_EQ((long)p, (long)p1);

	CLLogger::WriteLogMsg("The following is testing", 0);

	EXPECT_TRUE((p->Register("1", new CLFalseCommunication)).IsSuccess());
	EXPECT_FALSE((p->Register("1", 0)).IsSuccess());
	EXPECT_TRUE((p->Register("11", new CLFalseCommunication)).IsSuccess());
	EXPECT_TRUE((p->Register("111", new CLFalseCommunication)).IsSuccess());

	CLExecutiveCommunication *c1 = p->GetCommunicationPtr("1");
	p->ReleaseCommunicationPtr("1");

	CLExecutiveCommunication *c2 = p->GetCommunicationPtr("1");
	p->ReleaseCommunicationPtr("1");

	EXPECT_EQ((long)c1, (long)c2);

	EXPECT_EQ((long)(p->GetCommunicationPtr("2")), 0);

	EXPECT_FALSE((p->ReleaseCommunicationPtr("3")).IsSuccess());

	p->ReleaseCommunicationPtr("1");
	p->ReleaseCommunicationPtr("11");
	p->ReleaseCommunicationPtr("111");

	EXPECT_EQ(g_bfornameserver, 3);
	g_bfornameserver = 0;

	EXPECT_EQ((long)(p->GetCommunicationPtr("1")), 0);
}

TEST(CLExecutiveNameServer, ReleaseCommunicationPtr)
{
	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_FALSE(p->ReleaseCommunicationPtr(0).IsSuccess());
	EXPECT_FALSE(p->ReleaseCommunicationPtr("").IsSuccess());
	EXPECT_FALSE(p->ReleaseCommunicationPtr("1").IsSuccess());
}

TEST(CLExecutiveNameServer, GetCommunicationPtr)
{
	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_EQ(p->GetCommunicationPtr(0), (CLExecutiveCommunication *)0);
	EXPECT_EQ(p->GetCommunicationPtr(""), (CLExecutiveCommunication *)0);
	EXPECT_EQ(p->GetCommunicationPtr("3423"), (CLExecutiveCommunication *)0);
}

TEST(CLExecutiveNameServer, Register)
{
	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_FALSE(p->Register(0, 0).IsSuccess());
	EXPECT_FALSE(p->Register("", 0).IsSuccess());
	EXPECT_FALSE(p->Register("DSDFSD", 0).IsSuccess());

	EXPECT_FALSE(p->Register(0, new CLFalseCommunication).IsSuccess());
	EXPECT_TRUE(p->Register("SDFdfd", new CLFalseCommunication).IsSuccess());

	EXPECT_TRUE(p->ReleaseCommunicationPtr("SDFdfd").IsSuccess());

	EXPECT_EQ(g_bfornameserver, 2);
	g_bfornameserver = 0;

	EXPECT_TRUE(p->Register("23", new CLFalseCommunication).IsSuccess());

	EXPECT_FALSE(p->Register("23", new CLFalseCommunication).IsSuccess());

	EXPECT_TRUE(p->ReleaseCommunicationPtr("23").IsSuccess());

	EXPECT_EQ(g_bfornameserver, 2);
	g_bfornameserver = 0;
}

static int g_bforpstmsgfornameserver = 0;

class CLTestMsgForNameServer : public CLMessage
{
public:
	CLTestMsgForNameServer(int id) : CLMessage(id)
	{
	}

	virtual ~CLTestMsgForNameServer()
	{
		g_bforpstmsgfornameserver++;
	}
};

class CLFalseCommunication2 : public CLExecutiveCommunication
{
public:
	virtual CLStatus PostExecutiveMessage(CLMessage *pMessage)
	{
		delete pMessage;
		return CLStatus(-1, 0);
	}
};

TEST(CLExecutiveNameServer, PostExecutiveMessage)
{
	EXPECT_FALSE(CLExecutiveNameServer::PostExecutiveMessage(0, 0).IsSuccess());
	EXPECT_FALSE(CLExecutiveNameServer::PostExecutiveMessage(0, new CLTestMsgForNameServer(9)).IsSuccess());
	EXPECT_FALSE(CLExecutiveNameServer::PostExecutiveMessage("", new CLTestMsgForNameServer(9)).IsSuccess());
	EXPECT_FALSE(CLExecutiveNameServer::PostExecutiveMessage("2323", new CLTestMsgForNameServer(9)).IsSuccess());

	EXPECT_EQ(g_bforpstmsgfornameserver, 3);
	g_bforpstmsgfornameserver = 0;

	CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
	EXPECT_NE(p, (CLExecutiveNameServer *)0);

	EXPECT_TRUE((p->Register("111", new CLFalseCommunication)).IsSuccess());

	CLMessage *pm = new CLTestMsgForNameServer(2);
	EXPECT_TRUE(CLExecutiveNameServer::PostExecutiveMessage("111", pm).IsSuccess());

	EXPECT_TRUE(p->ReleaseCommunicationPtr("111").IsSuccess());

	EXPECT_EQ(g_bforpst, 1);
	g_bforpst = 0;

	EXPECT_EQ(g_bfornameserver, 1);
	g_bfornameserver = 0;

	delete pm;

	EXPECT_EQ(g_bforpstmsgfornameserver, 1);
	g_bforpstmsgfornameserver = 0;

	EXPECT_TRUE((p->Register("111", new CLFalseCommunication2)).IsSuccess());
	EXPECT_FALSE(CLExecutiveNameServer::PostExecutiveMessage("111", new CLTestMsgForNameServer(3)).IsSuccess());
	EXPECT_TRUE(p->ReleaseCommunicationPtr("111").IsSuccess());
	
	EXPECT_EQ(g_bforpstmsgfornameserver, 1);
	g_bforpstmsgfornameserver = 0;

	CLLogger::WriteLogMsg("The testing end", 0);
}