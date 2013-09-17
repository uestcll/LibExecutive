#ifndef CLSTLqueue_H
#define CLSTLqueue_H

#include <queue>
#include "CLMutex.h"
#include "CLStatus.h"

class CLIOVectors;

class CLSTLqueue
{
public:
	CLSTLqueue();
	virtual ~CLSTLqueue();

	CLStatus PopData(CLIOVectors& IOVectors, unsigned int index, unsigned int length);
	CLStatus PushData(unsigned long ulData);

private:
	unsigned long PopOneData();

private:
	CLSTLqueue(const CLSTLqueue&);
	CLSTLqueue& operator=(const CLSTLqueue&);

private:
	std::queue<unsigned long> m_DataQueue;
	CLMutex m_Mutex;
};

#endif