#include <gtest/gtest.h>
#include <string.h>
#include "LibExecutive.h"

static bool btestformlmfeft = false;
static bool btestformlmfeft2 = false;
static bool btestformlmfeft3 = false;

class CLMyObserverforExecutiveFunctionTest : public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void* pContext)
	{
		return CLStatus(0, 0);
	}
};

class CLMyMsgLoopManagerForExecutiveFunctionTest : public CLMessageLoopManager
{
public:
	CLMyMsgLoopManagerForExecutiveFunctionTest(CLMessageObserver *pMessageObserver) : CLMessageLoopManager(pMessageObserver)
	{
	}

	virtual ~CLMyMsgLoopManagerForExecutiveFunctionTest()
	{
		btestformlmfeft = true;
	}

	virtual CLStatus EnterMessageLoop(void *pContext)
	{
		btestformlmfeft2 = true;

		if((long)pContext == 2)
			btestformlmfeft3 = true;

		return CLStatus(0, 0);
	}

protected:
	virtual CLStatus Initialize()
	{
		return CLStatus(0, 0);
	}
	
	virtual CLStatus Uninitialize()
	{
		return CLStatus(0, 0);
	}
	
	virtual CLMessage* WaitForMessage()
	{
		return 0;
	}
	
	virtual CLStatus DispatchMessage(CLMessage *pMessage)
	{
		return CLStatus(0, 0);
	}
		
};

TEST(CLExecutiveFunctionForMsgLoop, parameter_null)
{
    try
    {
    	CLThread *p = new CLThread(new CLExecutiveFunctionForMsgLoop(0), true);
    }
	catch(const char *str)
	{
		if(strcmp(str, "In CLExecutiveFunctionForMsgLoop::CLExecutiveFunctionForMsgLoop(), pMsgLoopManager error") == 0)
		{
			EXPECT_TRUE(true);
		}
		else
		{
			EXPECT_TRUE(false);
		}
	}
}

TEST(CLExecutiveFunctionForMsgLoop, deconstructor_and_entermsgloop)
{
	CLThread *p = new CLThread(new CLExecutiveFunctionForMsgLoop(new CLMyMsgLoopManagerForExecutiveFunctionTest(new CLMyObserverforExecutiveFunctionTest)), true);
	EXPECT_TRUE((p->Run((void *)2)).IsSuccess());

	EXPECT_TRUE((p->WaitForDeath()).IsSuccess());

	EXPECT_TRUE(btestformlmfeft);
	EXPECT_TRUE(btestformlmfeft2);
	EXPECT_TRUE(btestformlmfeft3);
}