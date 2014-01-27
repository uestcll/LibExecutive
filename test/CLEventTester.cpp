#include <gtest/gtest.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include "LibExecutive.h"

struct TestForCLEvent
{
	CLEvent event;
	CLEvent eventfornew;
	int i;
};

static const int count = 100000;

static void* TestThreadForCLEvent(void *arg)
{
	TestForCLEvent *pT = (TestForCLEvent *)arg;

	for(int i = 0; i < count; i++)
	{
		pT->i++;

		EXPECT_TRUE((pT->event.Set()).IsSuccess());

		EXPECT_TRUE((pT->eventfornew.Wait()).IsSuccess());

		pT->i++;

		EXPECT_TRUE((pT->event.Set()).IsSuccess());

		EXPECT_TRUE((pT->eventfornew.Wait()).IsSuccess());
	}

	return 0;
}

TEST(CLEvent, ResetAutomatically)
{
	CLLogger::WriteLogMsg("CLEvent Test", 0);

	TestForCLEvent *pT = new TestForCLEvent;
	pT->i = 0;

	pthread_t tid;
	pthread_create(&tid, 0, TestThreadForCLEvent, pT);

	for(int i = 0; i < count; i++)
	{
		CLStatus s = pT->event.Wait();
		EXPECT_EQ(s.m_clReturnCode, 0);

		EXPECT_EQ(pT->i, 2 * i + 1);

		EXPECT_TRUE((pT->eventfornew.Set()).IsSuccess());

		CLStatus s2 = pT->event.Wait();
		EXPECT_EQ(s2.m_clReturnCode, 0);

		EXPECT_EQ(pT->i, 2 * i + 2);

		EXPECT_TRUE((pT->eventfornew.Set()).IsSuccess());
	}

	pthread_join(tid, 0);

	delete pT;
}

static void* TestThreadForCLEvent2(void *arg)
{
	CLEvent *p = (CLEvent *)arg;

	for(int i = 0; i < count; i++)
	{
		EXPECT_TRUE((p->Set()).IsSuccess());
	}

	return 0;
}

TEST(CLEvent, Semaphore)
{
	CLEvent e(true);

	pthread_t tid;
	pthread_create(&tid, 0, TestThreadForCLEvent2, &e);

	for(int i = 0; i < count; i++)
	{
		EXPECT_TRUE((e.Wait()).IsSuccess());
	}

	pthread_join(tid, 0);
}

static int g_dataforReleaseSemaphore = 0;

static void* TestThreadForCLEventRelease(void *arg)
{
	CLEvent *p = (CLEvent *)arg;

	sleep(5);

	g_dataforReleaseSemaphore = 10;

	EXPECT_TRUE(p->Set().IsSuccess());

	return 0;
}

TEST(CLEvent, ReleaseSemaphore)
{
	CLEvent event(true);

	for(int i = 0; i < 10; i++)
	{
		EXPECT_TRUE(event.Set().IsSuccess());
	}

	EXPECT_TRUE(event.ReleaseSemaphore(10).IsSuccess());

	pthread_t tid;
	pthread_create(&tid, 0, TestThreadForCLEventRelease, &event);

	EXPECT_TRUE(event.Wait().IsSuccess());
	EXPECT_EQ(g_dataforReleaseSemaphore, 10);

	pthread_join(tid, 0);
}

TEST(CLEvent, ResetAutomatically_shared)
{
	CLEvent event("test_for_event_auto");
	
	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLEvent_Auto/main").IsSuccess());

	EXPECT_TRUE(event.Wait().IsSuccess());
}

TEST(CLEvent, Semaphore_shared)
{
	CLEvent event("test_for_event_semaphore", true);

	CLProcess *process = new CLProcess(new CLProcessFunctionForExec);
	EXPECT_TRUE(process->Run((void *)"../test_for_exec/test_for_CLEvent_Semaphore/main").IsSuccess());

	event.Wait();
	event.Wait();
	event.Wait();
}