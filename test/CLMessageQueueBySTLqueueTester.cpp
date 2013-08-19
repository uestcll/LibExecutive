#include <gtest/gtest.h>
#include "LibExecutive.h"

static int gmsgdes = 0;

class CLMessage1ForCLMessageQueueBySTLqueue : public CLMessage
{
public:
	CLMessage1ForCLMessageQueueBySTLqueue(int j) : CLMessage(j)
	{
		i = j;
	}

	virtual ~CLMessage1ForCLMessageQueueBySTLqueue()
	{
		gmsgdes++;
	}
	
	int i;
};

TEST(CLMessageQueueBySTLqueue, Normal)
{
	{
		CLMessageQueueBySTLqueue q;
		EXPECT_TRUE((q.PushMessage(new CLMessage1ForCLMessageQueueBySTLqueue(2))).IsSuccess());
		EXPECT_TRUE((q.PushMessage(new CLMessage1ForCLMessageQueueBySTLqueue(3))).IsSuccess());
		EXPECT_TRUE((q.PushMessage(new CLMessage1ForCLMessageQueueBySTLqueue(4))).IsSuccess());

		CLMessage1ForCLMessageQueueBySTLqueue * p1 = (CLMessage1ForCLMessageQueueBySTLqueue *)q.GetMessage();
		EXPECT_NE((long)p1, 0);
		EXPECT_EQ(p1->i, 2);
		delete p1;

		CLMessage1ForCLMessageQueueBySTLqueue * p2 = (CLMessage1ForCLMessageQueueBySTLqueue *)q.GetMessage();
		EXPECT_NE((long)p2, 0);
		EXPECT_EQ(p2->i, 3);
		delete p2;
	}

	EXPECT_EQ(gmsgdes, 3);
}

class CThreadFunctionForCLMessageQueue: public CLExecutiveFunctionProvider
{
public:
	CThreadFunctionForCLMessageQueue()
	{
	}

	virtual ~CThreadFunctionForCLMessageQueue()
	{
	}
	
	virtual CLStatus RunExecutiveFunction(void* pContext)
	{
		CLMessageQueueBySTLqueue *q = (CLMessageQueueBySTLqueue *)pContext;
		int i = 0;

		while(1)
		{
			CLMessage1ForCLMessageQueueBySTLqueue *pM = (CLMessage1ForCLMessageQueueBySTLqueue *)(q->GetMessage());
			if(pM != 0)
			{		
				delete pM;
				i++;
				if(i == 2000)
					break;
			}
		}

		EXPECT_EQ(i, 2000);
	}
};

class CThreadFunctionForCLMessageQueue2: public CLExecutiveFunctionProvider
{
public:
	CThreadFunctionForCLMessageQueue2()
	{
	}

	virtual ~CThreadFunctionForCLMessageQueue2()
	{
	}
	
	virtual CLStatus RunExecutiveFunction(void* pContext)
	{
		CLMessageQueueBySTLqueue *q = (CLMessageQueueBySTLqueue *)pContext;

		for(int i = 0; i < 1000; i++)
		{
			EXPECT_TRUE((q->PushMessage(new CLMessage1ForCLMessageQueueBySTLqueue(i))).IsSuccess());	
		}
	}
};


TEST(CLMessageQueueBySTLqueue, MutipleThread)
{
	CLMessageQueueBySTLqueue q;
	
	CLThread *t1 = new CLThread(new CThreadFunctionForCLMessageQueue, true);
	t1->Run(&q);

	CLThread *t2 = new CLThread(new CThreadFunctionForCLMessageQueue2, true);
	t2->Run(&q);

	CLThread *t3 = new CLThread(new CThreadFunctionForCLMessageQueue2, true);
	t3->Run(&q);

	t1->WaitForDeath();
	t2->WaitForDeath();
	t3->WaitForDeath();
}