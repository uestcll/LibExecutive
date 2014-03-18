#ifndef CLSTLqueue_H
#define CLSTLqueue_H

#include <queue>
#include "CLMutex.h"
#include "CLStatus.h"
#include "CLUuid.h"

class CLIOVectors;

class CLSTLqueue
{
public:
	CLSTLqueue();
	virtual ~CLSTLqueue();

	CLUuid GetUuid();

	CLStatus PopData(CLIOVectors& IOVectors);
	CLStatus PushData(unsigned long ulData);

private:
	unsigned long PopOneData();

private:
	CLSTLqueue(const CLSTLqueue&);
	CLSTLqueue& operator=(const CLSTLqueue&);

private:
	std::queue<unsigned long> m_DataQueue;
	CLMutex m_Mutex;
	CLUuid m_UuidOfSTLqueue;
};

#endif