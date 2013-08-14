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

static const int count = 1000000;

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

TEST(CLEvent, ResetAutomatically_shared)
{
	CLEvent event("test_for_event_auto");
	pid_t pid = fork();
	if(pid == 0)
	{
		{
			CLEvent event("test_for_event_auto");
			EXPECT_TRUE(event.Set().IsSuccess());
		}

		CLLibExecutiveInitializer::Destroy();

		_exit(0);
	}

	EXPECT_TRUE(event.Wait().IsSuccess());

	wait(NULL);
}

TEST(CLEvent, Semaphore_shared)
{
	CLEvent event("test_for_event_semaphore", true);

	pid_t pid = fork();
	if(pid == 0)
	{
		{
			sleep(2);
			CLEvent event("test_for_event_semaphore", true);
			EXPECT_TRUE(event.Set().IsSuccess());
			EXPECT_TRUE(event.Set().IsSuccess());
			EXPECT_TRUE(event.Set().IsSuccess());
		}

		CLLibExecutiveInitializer::Destroy();

		_exit(0);
	}

	EXPECT_TRUE(event.Wait().IsSuccess());
	EXPECT_TRUE(event.Wait().IsSuccess());
	EXPECT_TRUE(event.Wait().IsSuccess());

	wait(NULL);
}