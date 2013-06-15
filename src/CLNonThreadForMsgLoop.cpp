#include <string.h>
#include "CLNonThreadForMsgLoop.h"
#include "CLExecutiveFunctionForMsgLoop.h"
#include "CLMsgLoopManagerForSTLqueue.h"
#include "CLLogger.h"
#include "CLThreadInitialFinishedNotifier.h"
#include "CLMsgLoopManagerForPipeQueue.h"

CLNonThreadForMsgLoop::CLNonThreadForMsgLoop(CLMessageObserver *pMsgObserver, const char *pstrThreadName, int ExecutiveType)
{
	if(pMsgObserver == 0)
		throw "In CLNonThreadForMsgLoop::CLNonThreadForMsgLoop(), pMsgObserver error";

	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLNonThreadForMsgLoop::CLNonThreadForMsgLoop(), pstrThreadName error";

	m_pPipeMsgQueue = NULL;

	if(ExecutiveType == EXECUTIVE_IN_PROCESS_USE_STL_QUEUE)
	{
		m_pFunctionProvider = new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForSTLqueue(pMsgObserver, pstrThreadName));
	}
	else if(ExecutiveType == EXECUTIVE_IN_PROCESS_USE_PIPE_QUEUE)
	{
		m_pPipeMsgQueue = new CLMsgLoopManagerForPipeQueue(pMsgObserver, pstrThreadName, PIPE_QUEUE_IN_PROCESS);
		m_pFunctionProvider = new CLExecutiveFunctionForMsgLoop(m_pPipeMsgQueue);
	}
	else if(ExecutiveType == EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE)
	{
		m_pPipeMsgQueue = new CLMsgLoopManagerForPipeQueue(pMsgObserver, pstrThreadName, PIPE_QUEUE_BETWEEN_PROCESS);
		m_pFunctionProvider = new CLExecutiveFunctionForMsgLoop(m_pPipeMsgQueue);
	}
	else
		throw "In CLNonThreadForMsgLoop::CLNonThreadForMsgLoop(), ExecutiveType Error";
}

CLNonThreadForMsgLoop::~CLNonThreadForMsgLoop()
{
	delete m_pFunctionProvider;
}

CLStatus CLNonThreadForMsgLoop::Run(void *pContext)
{
	CLThreadInitialFinishedNotifier notifier(0);

	SLExecutiveInitialParameter para;
	para.pContext = pContext;
	para.pNotifier = &notifier;

	return m_pFunctionProvider->RunExecutiveFunction(&para);
}

CLStatus CLNonThreadForMsgLoop::RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer)
{
	if(m_pPipeMsgQueue != NULL)
		return m_pPipeMsgQueue->RegisterDeserializer(lMsgID, pDeserializer);

	return CLStatus(-1, 0);
}