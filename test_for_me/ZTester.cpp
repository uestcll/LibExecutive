#include <gtest/gtest.h>
#include "LibExecutive.h"

#define NUM 30

static long ObjectsForLog[NUM];
static long ObjectsForNameServer[NUM];

void* TestThreadForCLLibExecutiveInitializerSingleton(void *arg)
{	
	CLLibExecutiveInitializer::Initialize();

	long i = (long)arg;	
	long j = (long)CLLogger::GetInstance();	
	ObjectsForLog[i] = j;

	long k = (long)CLExecutiveNameServer::GetInstance();
	ObjectsForNameServer[i] = k;

	return 0;
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
		pthread_create(&(tid[i]), 0, TestThreadForCLLibExecutiveInitializerSingleton, (void *)i);
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