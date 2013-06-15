#include <string.h>
#include "CLThreadForMsgLoop.h"
#include "CLThread.h"
#include "CLExecutiveFunctionForMsgLoop.h"
#include "CLMsgLoopManagerForSTLqueue.h"
#include "CLLogger.h"
#include "CLThreadInitialFinishedNotifier.h"
#include "CLEvent.h"
#include "CLMsgLoopManagerForPipeQueue.h"

CLThreadForMsgLoop::CLThreadForMsgLoop(CLMessageObserver *pMsgObserver, const char *pstrThreadName, bool bWaitForDeath, int ExecutiveType)
{
	if(pMsgObserver == 0)
		throw "In CLThreadForMsgLoop::CLThreadForMsgLoop(), pMsgObserver error";

	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLThreadForMsgLoop::CLThreadForMsgLoop(), pstrThreadName error";

	m_bWaitForDeath = bWaitForDeath;

	m_pPipeQueue = NULL;

	if(ExecutiveType == EXECUTIVE_IN_PROCESS_USE_STL_QUEUE)
	{
		m_pThread = new CLThread(new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForSTLqueue(pMsgObserver, pstrThreadName)), bWaitForDeath);
	}
	else if(ExecutiveType == EXECUTIVE_IN_PROCESS_USE_PIPE_QUEUE)
	{
		m_pPipeQueue = new CLMsgLoopManagerForPipeQueue(pMsgObserver, pstrThreadName, PIPE_QUEUE_IN_PROCESS);
		m_pThread = new CLThread(new CLExecutiveFunctionForMsgLoop(m_pPipeQueue), bWaitForDeath);
	}
	else if(ExecutiveType == EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE)
	{
		m_pPipeQueue = new CLMsgLoopManagerForPipeQueue(pMsgObserver, pstrThreadName, PIPE_QUEUE_BETWEEN_PROCESS);
		m_pThread = new CLThread(new CLExecutiveFunctionForMsgLoop(m_pPipeQueue), bWaitForDeath);
	}
	else
		throw "In CLThreadForMsgLoop::CLThreadForMsgLoop(), ExecutiveType Error";
}

CLThreadForMsgLoop::~CLThreadForMsgLoop()
{
	if((m_bWaitForDeath) && (m_pThread != 0))
	{
		CLStatus s = m_pThread->WaitForDeath();
		if(!s.IsSuccess())
			CLLogger::WriteLogMsg("In CLThreadForMsgLoop::~CLThreadForMsgLoop(), m_pThread->WaitForDeath error", 0);
	}
}

CLStatus CLThreadForMsgLoop::Run(void *pContext)
{
	if(m_pThread == 0)
		return CLStatus(-1, 0);

	CLEvent event;
	CLThreadInitialFinishedNotifier notifier(&event);

	SLExecutiveInitialParameter para;
	para.pContext = pContext;
	para.pNotifier = &notifier;

	CLStatus s = m_pThread->Run(&para);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLThreadForMsgLoop::Run(), m_pThread->Run error", 0);
		m_bWaitForDeath = false;
		m_pThread = 0;
		return CLStatus(-1, 0);
	}

	CLStatus s1 = event.Wait();
	if(!s1.IsSuccess())
		CLLogger::WriteLogMsg("In CLThreadForMsgLoop::Run(), event.Wait error", 0);

	if(notifier.IsInitialSuccess())
		return CLStatus(0, 0);
	else
		return CLStatus(-1, 0);
}

CLStatus CLThreadForMsgLoop::RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer)
{
	if(m_pPipeQueue != NULL)
		return m_pPipeQueue->RegisterDeserializer(lMsgID, pDeserializer);

	return CLStatus(-1, 0);
}