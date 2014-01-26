#include <gtest/gtest.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "LibExecutive.h"
#include "CLLogger.h"

TEST(CLLogger, WriteLogDirectly_Parameters_Test)
{
	EXPECT_FALSE(CLLogger::WriteLogDirectly(0, 0).IsSuccess());
	EXPECT_FALSE(CLLogger::WriteLogDirectly("", 0).IsSuccess());
}

TEST(CLLogger, WriteLog_pstrmsg_0)
{
	CLLogger::WriteLogMsg("CLLogger Test", 0);

	CLLogger *pLogger = CLLogger::GetInstance();
	EXPECT_TRUE(pLogger != 0);

	CLStatus s = pLogger->WriteLog(0, 0);	
	EXPECT_EQ(s.m_clReturnCode, -1);
}

TEST(CLLogger, WriteLog_pstrmsg_empty)
{
	CLLogger *pLogger = CLLogger::GetInstance();
	EXPECT_TRUE(pLogger != 0);

	CLStatus s = pLogger->WriteLog("", 0);	
	EXPECT_EQ(s.m_clReturnCode, -1);
}

TEST(CLLogger, WriteLogMsg_pstrmsg_0)
{
	CLStatus s = CLLogger::WriteLogMsg(0, 0);
	EXPECT_EQ(s.m_clReturnCode, -1);
}

TEST(CLLogger, WriteLogMsg_pstrmsg_empty)
{
	CLStatus s = CLLogger::WriteLogMsg("", 0);
	EXPECT_EQ(s.m_clReturnCode, -1);
}

static int GetNumberOfLinesOfLogFile()
{
	FILE *fp = fopen("logger", "r");
	if(fp == 0)
		EXPECT_TRUE(false);

	int num = 0;

	for(;;)
	{
		char buf[1024] = {0};

		if(fgets(buf, 1024, fp) == NULL)
			break;

		num++;
	}

	fclose(fp);

	return num;
}

TEST(CLLogger, Features)
{
	int num = GetNumberOfLinesOfLogFile();

	const int n = 10;
	for(int i = 0; i < n; i++)
		CLLogger::WriteLogMsg("nihao", 0);

	FILE *fp = fopen("logger", "r");

	for(int i = 0; i < n + num; i++)
	{
		char buf[256] = {0};
		fgets(buf, 256, fp);

		if(i < num)
			continue;

		EXPECT_EQ(strcmp(buf, "nihao	Error code: 0\r\n"), 0);
	}

	fclose(fp);
}

TEST(CLLogger, WriteLogDirectly_Features_Test)
{
	int num = GetNumberOfLinesOfLogFile();

	const int n = 20;
	for(int i = 0; i < n; i++)
		CLLogger::WriteLogDirectly("okoko", 0);

	FILE *fp = fopen("logger", "r");

	for(int i = 0; i < n + num; i++)
	{
		char buf[256] = {0};
		fgets(buf, 256, fp);

		if(i < num)
			continue;

		EXPECT_EQ(strcmp(buf, "okoko	Error code: 0\r\n"), 0);
	}

	fclose(fp);
}