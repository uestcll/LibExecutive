#include <string.h>
#include "CLThreadForMsgLoop.h"
#include "CLThread.h"
#include "CLExecutiveFunctionForMsgLoop.h"
#include "CLMsgLoopManagerForSTLqueue.h"
#include "CLThreadInitialFinishedNotifier.h"
#include "CLMsgLoopManagerForPrivateNamedPipe.h"
#include "CLMsgLoopManagerForShareNamedPipe.h"
#include "CLProtocolDecapsulatorByDefaultMsgFormat.h"
#include "CLMultiMsgSerializer.h"
#include "CLMultiMsgDeserializer.h"
#include "CLEvent.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLThreadForMsgLoop::CLThreadForMsgLoop(CLMessageObserver *pMsgObserver, const char *pstrThreadName, bool bWaitForDeath, int ExecutiveType, CLProtocolEncapsulator *pEncapsulator, CLProtocolDecapsulator *pDecapsulator)
{
	if(pMsgObserver == 0)
		throw "In CLThreadForMsgLoop::CLThreadForMsgLoop(), pMsgObserver error";

	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLThreadForMsgLoop::CLThreadForMsgLoop(), pstrThreadName error";

	m_bWaitForDeath = bWaitForDeath;

	if(ExecutiveType == EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE)
	{
		m_pSerializer = new CLMultiMsgSerializer();
		m_pDeserializer = new CLMultiMsgDeserializer();

		if(pDecapsulator == 0)
			pDecapsulator = new CLProtocolDecapsulatorByDefaultMsgFormat();

		m_pThread = new CLThread(new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForShareNamedPipe(pMsgObserver, pstrThreadName, pEncapsulator, pDecapsulator, m_pSerializer, m_pDeserializer)), bWaitForDeath);

		return;
	}
	else
	{
		m_pDeserializer = 0;
		m_pSerializer = 0;

		if(ExecutiveType == EXECUTIVE_IN_PROCESS_USE_STL_QUEUE)
		{
			m_pThread = new CLThread(new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForSTLqueue(pMsgObserver, pstrThreadName)), bWaitForDeath);
			return;
		}
		else if(ExecutiveType == EXECUTIVE_IN_PROCESS_USE_PIPE_QUEUE)
		{
			m_pThread = new CLThread(new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForPrivateNamedPipe(pMsgObserver, pstrThreadName)), bWaitForDeath);
			return;
		}
	}
	
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

CLStatus CLThreadForMsgLoop::RegisterSerializer(unsigned long lMsgID, CLMessageSerializer *pSerializer)
{
	if(m_pSerializer)
	{
		return m_pSerializer->RegisterSerializer(lMsgID, pSerializer);
	}

	return CLStatus(-1, NORMAL_ERROR);
}

CLStatus CLThreadForMsgLoop::RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer)
{
	if(m_pDeserializer)
	{
		return m_pDeserializer->RegisterDeserializer(lMsgID, pDeserializer);
	}

	return CLStatus(-1, NORMAL_ERROR);
}