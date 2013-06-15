#ifndef CLNonThreadForMsgLoop_H
#define CLNonThreadForMsgLoop_H

#include "CLStatus.h"

class CLMessageObserver;
class CLExecutiveFunctionProvider;
class CLMsgLoopManagerForPipeQueue;
class CLMessageDeserializer;

#define EXECUTIVE_IN_PROCESS_USE_STL_QUEUE 0
#define EXECUTIVE_IN_PROCESS_USE_PIPE_QUEUE 1
#define EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE 2

/*
该类用于让线程直接进入消息循环，而不是创建新线程
*/
class CLNonThreadForMsgLoop
{
public:
	/*
	pMsgObserver应从堆中分配，且不必调用delete，pstrThreadName所代表的线程名称必须是唯一的
	*/
	CLNonThreadForMsgLoop(CLMessageObserver *pMsgObserver, const char *pstrThreadName, int ExecutiveType = EXECUTIVE_IN_PROCESS_USE_STL_QUEUE);
	virtual ~CLNonThreadForMsgLoop();

	CLStatus Run(void *pContext);
	CLStatus RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer);

private:
	CLNonThreadForMsgLoop(const CLNonThreadForMsgLoop&);
	CLNonThreadForMsgLoop& operator=(const CLNonThreadForMsgLoop&);

private:
	CLExecutiveFunctionProvider *m_pFunctionProvider;
	CLMsgLoopManagerForPipeQueue *m_pPipeMsgQueue;
};

#endif