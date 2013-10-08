#ifndef CLSTLQUEUE_H
#define CLSTLQUEUE_H

#include <iostream>
#include <queue>
#include "CLStatus.h"
#include "CLMutex.h"

using namespace std;

class CLMessage;

class CLSTLQueue
{
public:
	CLSTLQueue();
	virtual ~CLSTLQueue();

	CLMessage* PopMessage();
	CLStatus PushMessage(CLMessage* pMsg);
	 
private:
    CLMutex m_MutexForQueue;
	queue<CLMessage*> m_MsgQueue; 
};

#endif