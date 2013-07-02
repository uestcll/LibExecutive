#include <gtest/gtest.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "LibExecutive.h"
#include "CLLogger.h"

TEST(CLLogger, WriteLog_pstrmsg_0)
{
    CLLogger::Create();
	CLLogger *pLogger = CLLogger::GetInstance();
	EXPECT_TRUE(pLogger != 0);

	if(pLogger != 0)
	{
		CLStatus s = pLogger->WriteLog(0, 0);	
		EXPECT_EQ(s.m_clReturnCode, -1);
	}
}

TEST(CLLogger, WriteLog_pstrmsg_empty)
{
    CLLogger::Create();
	CLLogger *pLogger = CLLogger::GetInstance();
	EXPECT_TRUE(pLogger != 0);

	if(pLogger != 0)
	{
		CLStatus s = pLogger->WriteLog("", 0);	
		EXPECT_EQ(s.m_clReturnCode, -1);
	}
}

TEST(CLLogger, WriteLogMsg_pstrmsg_0)
{
    CLLogger::Create();
	CLStatus s = CLLogger::WriteLogMsg(0, 0);
	EXPECT_EQ(s.m_clReturnCode, -1);
}

TEST(CLLogger, WriteLogMsg_pstrmsg_empty)
{
	CLStatus s = CLLogger::WriteLogMsg("", 0);
	EXPECT_EQ(s.m_clReturnCode, -1);
}

/*TEST(CLLogger, Flush)
{
	CLLogger *pLog = CLLogger::GetInstance();
	EXPECT_TRUE(pLog != 0);

	CLStatus s = pLog->Flush();
	EXPECT_TRUE(s.IsSuccess());
}*/

TEST(CLLogger, Features)
{
    CLLogger::Create();
	const int n = 100000;
	for(int i = 0; i < n; i++)
		CLLogger::WriteLogMsg("nihao", 0);

	CLLogger *pLog = CLLogger::GetInstance();
	EXPECT_TRUE(pLog != 0);

	//CLStatus s = pLog->Flush();
	//EXPECT_TRUE(s.IsSuccess());

	FILE *fp = fopen("logger", "r");

	for(int i = 0; i < n; i++)
	{
		char buf[256] = {0};
		fgets(buf, 256, fp);

		EXPECT_EQ(strcmp(buf, "nihao	Error code: 0\r\n"), 0);
	}

	fclose(fp);
}

void* TestThreadForCLLog1(void *arg)
{
	long i = (long)arg;

	for(int j = 0; j < 1000; j++)
	{
		if((i % 2) == 0)
		{
			CLStatus s = CLLogger::GetInstance()->WriteLog("dddfaefgds", i);
			EXPECT_TRUE(s.IsSuccess());
		}
		/*
            else
			CLLogger::GetInstance()->Flush();
        */
	}
}

#define NUM 30

TEST(CLLogger, WriteLogForMultiThread)
{
    CLLogger::Create();
	pthread_t tid[NUM];
	for(int i = 0; i < NUM; i++)
	{
		pthread_create(&(tid[i]), 0, TestThreadForCLLog1, (void *)((long)i));
	}

	for(int i = 0; i < NUM; i++)
	{
		pthread_join(tid[i], 0);
	}
}

#define PNUM 10000

TEST(CLLogger, ProcessSafety)
{
    CLLogger::Create();
	//CLLogger::GetInstance()->Flush();
	truncate("logger", 0);

	pid_t p;
	p = fork();

	if(p == 0)
	{
		for(int i = 0; i < PNUM; i++)
		{
			CLLogger::WriteLogMsg("111", 0);
			//CLLogger::GetInstance()->Flush();
		}

		CLLogger::Destroy();
        //CLLogger::Destroy();
		exit(0);
	}
	else
	{
		for(int i = 0; i < PNUM; i++)
		{
			CLLogger::WriteLogMsg("222", 0);
			//CLLogger::GetInstance()->Flush();
		}
	}

	wait(0);

	FILE *fp = fopen("logger", "r");

	for(int i = 0; i < PNUM * 2; i++)
	{
		char buf[256];
		fgets(buf, 256, fp);

		int p = strcmp(buf, "111	Error code: 0\r\n");
		int q = strcmp(buf, "222	Error code: 0\r\n");
		bool b = false;
		if(( p == 0) || (q == 0))
			b = true;

		EXPECT_TRUE(b);
	}

	fclose(fp);
}

TEST(CLLogger, ProcessSafety2)
{
    CLLogger::Create();
	//CLLogger::GetInstance()->Flush();
	truncate("logger", 0);

	pid_t p;
	p = fork();

	if(p == 0)
	{
		for(int i = 0; i < PNUM; i++)
		{
			CLLogger::WriteLogMsg("1111", 0);
		}

		CLLogger::Destroy();
        //CLLogger::Destroy();
		exit(0);
	}
	else
	{
		for(int i = 0; i < PNUM; i++)
		{
			CLLogger::WriteLogMsg("2222", 0);
		}
	}

	wait(0);

	FILE *fp = fopen("logger", "r");

	for(int i = 0; i < PNUM * 2; i++)
	{
		char buf[256];
		fgets(buf, 256, fp);

		int p = strcmp(buf, "1111	Error code: 0\r\n");
		int q = strcmp(buf, "2222	Error code: 0\r\n");
		bool b = false;
		if(( p == 0) || (q == 0))
			b = true;

		EXPECT_TRUE(b);
	}

	fclose(fp);

	truncate("logger", 0);
}
