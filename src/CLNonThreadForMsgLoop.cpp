#include <string.h>
#include "CLNonThreadForMsgLoop.h"
#include "CLExecutiveFunctionForMsgLoop.h"
#include "CLMsgLoopManagerForSTLqueue.h"
#include "CLMsgLoopManagerForPrivateNamedPipe.h"
#include "CLMsgLoopManagerForShareNamedPipe.h"
#include "CLProtocolDecapsulatorByDefaultMsgFormat.h"
#include "CLThreadInitialFinishedNotifier.h"
#include "CLMultiMsgSerializer.h"
#include "CLMultiMsgDeserializer.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLNonThreadForMsgLoop::CLNonThreadForMsgLoop(CLMessageObserver *pMsgObserver, const char *pstrThreadName, int ExecutiveType, CLProtocolEncapsulator *pEncapsulator, CLProtocolDecapsulator *pDecapsulator)
{
	if(pMsgObserver == 0)
		throw "In CLNonThreadForMsgLoop::CLNonThreadForMsgLoop(), pMsgObserver error";

	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLNonThreadForMsgLoop::CLNonThreadForMsgLoop(), pstrThreadName error";

	if(ExecutiveType == EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE)
	{
		m_pSerializer = new CLMultiMsgSerializer();
		m_pDeserializer = new CLMultiMsgDeserializer();

		if(pDecapsulator == 0)
			pDecapsulator = new CLProtocolDecapsulatorByDefaultMsgFormat();

		m_pFunctionProvider = new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForShareNamedPipe(pMsgObserver, pstrThreadName, pEncapsulator, pDecapsulator, m_pSerializer, m_pDeserializer));

		return;
	}
	else
	{
		m_pSerializer = 0;
		m_pDeserializer = 0;

		if(ExecutiveType == EXECUTIVE_IN_PROCESS_USE_STL_QUEUE)
		{
			m_pFunctionProvider = new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForSTLqueue(pMsgObserver, pstrThreadName));
			return;
		}
		else if(ExecutiveType == EXECUTIVE_IN_PROCESS_USE_PIPE_QUEUE)
		{
			m_pFunctionProvider = new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForPrivateNamedPipe(pMsgObserver, pstrThreadName));
			return;
		}
	}
	
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

CLStatus CLNonThreadForMsgLoop::RegisterSerializer(unsigned long lMsgID, CLMessageSerializer *pSerializer)
{
	if(m_pSerializer)
	{
		return m_pSerializer->RegisterSerializer(lMsgID, pSerializer);
	}

	return CLStatus(-1, NORMAL_ERROR);
}

CLStatus CLNonThreadForMsgLoop::RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer)
{
	if(m_pDeserializer)
	{
		return m_pDeserializer->RegisterDeserializer(lMsgID, pDeserializer);
	}

	return CLStatus(-1, NORMAL_ERROR);
}