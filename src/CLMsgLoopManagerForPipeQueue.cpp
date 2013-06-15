#include <string.h>
#include "CLMsgLoopManagerForPipeQueue.h"
#include "CLMessageQueueByNamedPipe.h"
#include "CLLogger.h"
#include "CLSharedMsgQueueByNamedPipe.h"
#include "CLPrivateMsgQueueByNamedPipe.h"
#include "CLExecutiveNameServer.h"
#include "CLPrivateExecutiveCommunicationByNamedPipe.h"

CLMsgLoopManagerForPipeQueue::CLMsgLoopManagerForPipeQueue(CLMessageObserver *pMsgObserver, const char* pstrThreadName, int PipeQueueType) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForPipeQueue::CLMsgLoopManagerForPipeQueue(), pstrThreadName error";

	m_strThreadName = pstrThreadName;

	if(PipeQueueType == PIPE_QUEUE_BETWEEN_PROCESS)
		m_pMsgQueue = new CLSharedMsgQueueByNamedPipe(pstrThreadName);
	else if(PipeQueueType == PIPE_QUEUE_IN_PROCESS)
		m_pMsgQueue = new CLPrivateMsgQueuebyNamedPipe(pstrThreadName);
	else
		throw "In CLMsgLoopManagerForPipeQueue::CLMsgLoopManagerForPipeQueue(), PipeQueueType error";
}

CLMsgLoopManagerForPipeQueue::~CLMsgLoopManagerForPipeQueue()
{
	delete m_pMsgQueue;
}

CLStatus CLMsgLoopManagerForPipeQueue::Initialize()
{
	CLPrivateMsgQueuebyNamedPipe *pQueue = dynamic_cast<CLPrivateMsgQueuebyNamedPipe *>(m_pMsgQueue);
	if(pQueue == 0)
		return CLStatus(0, 0);

	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPipeQueue::Initialize(), CLExecutiveNameServer::GetInstance error", 0);
		return CLStatus(-1, 0);
	}

	CLStatus s = pNameServer->Register(m_strThreadName.c_str(), new CLPrivateExecutiveCommunicationByNamedPipe(m_strThreadName.c_str()));
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPipeQueue::Initialize(), pNameServer->Register error", 0);
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForPipeQueue::Uninitialize()
{
	CLPrivateMsgQueuebyNamedPipe *pQueue = dynamic_cast<CLPrivateMsgQueuebyNamedPipe *>(m_pMsgQueue);
	if(pQueue == 0)
		return CLStatus(0, 0);

	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPipeQueue::Uninitialize(), CLExecutiveNameServer::GetInstance error", 0);
		return CLStatus(-1, 0);
	}

	return pNameServer->ReleaseCommunicationPtr(m_strThreadName.c_str());
}

CLMessage* CLMsgLoopManagerForPipeQueue::WaitForMessage()
{
	return m_pMsgQueue->GetMessage();
}

CLStatus CLMsgLoopManagerForPipeQueue::RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer)
{
	CLSharedMsgQueueByNamedPipe *pQueue = dynamic_cast<CLSharedMsgQueueByNamedPipe *>(m_pMsgQueue);
	if(pQueue != 0)
		return pQueue->RegisterDeserializer(lMsgID, pDeserializer);
	else
		return CLStatus(-1, 0);
}