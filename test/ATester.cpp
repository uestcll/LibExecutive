#include <gtest/gtest.h>
#include "CLLibExecutiveInitializer.h"
#include "CLLogger.h"
#include "CLExecutiveNameServer.h"

#define NUM 30

long ObjectsForLog[NUM];
long ObjectsForNameServer[NUM];

void* TestThreadForCLLog(void *arg)
{	
	CLLibExecutiveInitializer::Initialize();

	long i = (long)arg;	
	long j = (long)CLLogger::GetInstance();	
	ObjectsForLog[i] = j;

	long k = (long)CLExecutiveNameServer::GetInstance();
	ObjectsForNameServer[i] = k;
}

TEST(CLLibExecutiveInitializer, FirstCallDestroy)
{
	EXPECT_FALSE(CLLibExecutiveInitializer::Destroy().IsSuccess());
}

TEST(CLLibExecutiveInitializer, Singleton)
{
	pthread_t tid[NUM];	
	for(long i = 0; i < NUM; i++)	
	{		
		pthread_create(&(tid[i]), 0, TestThreadForCLLog, (void *)i);
	}	

	for(long i = 0; i < NUM; i++)
	{		
		pthread_join(tid[i], 0);	
	}	

	long j = (long)CLLogger::GetInstance();	
	long k = (long)CLExecutiveNameServer::GetInstance();
	for(long i = 0; i < NUM; i++)	
	{		
		EXPECT_EQ(j, ObjectsForLog[i]);	
		EXPECT_EQ(k, ObjectsForNameServer[i]);
	}
}