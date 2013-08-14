#include <gtest/gtest.h>
#include "LibExecutive.h"

static bool bFlag1ForCLThread = false;
static bool bFlag2ForCLThread = false;

class CThreadFunctionForCLThread: public CLExecutiveFunctionProvider
{
public:
	CThreadFunctionForCLThread()
	{
	}

	virtual ~CThreadFunctionForCLThread()
	{
		bFlag1ForCLThread = true;
	}
	
	virtual CLStatus RunExecutiveFunction(void* pContext)
	{
		long i = (long)pContext;
		EXPECT_EQ(i, 2);
	}
};

class CLThreadDriv : public CLThread
{
public:
	CLThreadDriv(CLExecutiveFunctionProvider *p) : CLThread(p)
	{
	}

	CLThreadDriv(CLExecutiveFunctionProvider *p, bool b) : CLThread(p, b)
	{
	}

	virtual ~CLThreadDriv()
	{
		bFlag2ForCLThread = true;
	}
};

TEST(CLThread, Normal)
{
	CLThread *p = new CLThreadDriv(new CThreadFunctionForCLThread(), true);

	EXPECT_FALSE((p->WaitForDeath()).IsSuccess());

	EXPECT_TRUE((p->Run((void *)2)).IsSuccess());

	EXPECT_FALSE((p->Run((void *)2)).IsSuccess());

	EXPECT_TRUE((p->WaitForDeath()).IsSuccess());

	EXPECT_TRUE(bFlag1ForCLThread);
	EXPECT_TRUE(bFlag2ForCLThread);
}

TEST(CLThread, Normal2)
{
	bFlag1ForCLThread = false;
	bFlag2ForCLThread = false;
	
	CLThread *p = new CLThreadDriv(new CThreadFunctionForCLThread());

	EXPECT_TRUE((p->Run((void *)2)).IsSuccess());

	sleep(2);

	EXPECT_TRUE(bFlag1ForCLThread);
	EXPECT_TRUE(bFlag2ForCLThread);
}

TEST(CLThread, Normal3)
{
	try
	{
		CLThread *p = new CLThreadDriv(0);
		EXPECT_TRUE(false);
	}
	catch(const char *)
	{
		EXPECT_TRUE(true);
	}
}