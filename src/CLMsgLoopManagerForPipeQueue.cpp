#include <string.h>
#include "CLMsgLoopManagerForPipeQueue.h"
#include "CLLogger.h"
#include "CLExecutiveNameServer.h"
#include "CLMultiMsgDeserializer.h"
#include "CLMessageReceiver.h"
#include "CLDataReceiverByNamedPipe.h"
#include "CLProtoParserForPointerMsg.h"
#include "CLProtoParserForDefaultMsgFormat.h"
#include "CLPointerMsgDeserializer.h"
#include "CLMessagePoster.h"
#include "CLDataPosterChannelByNamedPipeMaintainer.h"
#include "CLPointerMsgSerializer.h"
#include "CLMessage.h"

CLMsgLoopManagerForPipeQueue::CLMsgLoopManagerForPipeQueue(CLMessageObserver *pMsgObserver, const char* pstrThreadName, int PipeQueueType, CLMultiMsgDeserializer *pMultiMsgDeserializer) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForPipeQueue::CLMsgLoopManagerForPipeQueue(), pstrThreadName error";

	m_strThreadName = pstrThreadName;
	m_pEvent = new CLEvent(true);
	
	if(PipeQueueType == PIPE_QUEUE_BETWEEN_PROCESS)
	{
		if(pMultiMsgDeserializer)
		{
			m_pMultiMsgDeserializer = pMultiMsgDeserializer;
		}
		else
		{
			m_pMultiMsgDeserializer = new CLMultiMsgDeserializer();
		}
		m_pMsgReceiver = new CLMessageReceiver(new CLDataReceiverByNamedPipe(m_strThreadName.c_str(), true), new CLProtoParserForDefaultMsgFormat(), m_pMultiMsgDeserializer);
	}
	else if(PipeQueueType == PIPE_QUEUE_IN_PROCESS)
	{
		m_pMultiMsgDeserializer = NULL;
		m_pMsgReceiver = new CLMessageReceiver(new CLDataReceiverByNamedPipe(m_strThreadName.c_str()), new CLProtoParserForPointerMsg(), new CLPointerMsgDeserializer());
	}
	else
		throw "In CLMsgLoopManagerForPipeQueue::CLMsgLoopManagerForPipeQueue(), PipeQueueType error";
}

CLMsgLoopManagerForPipeQueue::~CLMsgLoopManagerForPipeQueue()
{
	delete m_pMsgReceiver;
}

CLStatus CLMsgLoopManagerForPipeQueue::Initialize()
{
	if(m_pMultiMsgDeserializer)
		return CLStatus(0, 0);

	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		delete m_pMsgReceiver;
		m_pMsgReceiver = 0;
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPipeQueue::Initialize(), CLExecutiveNameServer::GetInstance error", 0);
		return CLStatus(-1, 0);
	}

	CLStatus s = pNameServer->Register(m_strThreadName.c_str(), new CLMessagePoster(new CLDataPosterChannelByNamedPipeMaintainer(m_strThreadName.c_str()), new CLPointerMsgSerializer(), NULL, m_pEvent));
	if(!s.IsSuccess())
	{
		delete m_pMsgReceiver;
		m_pMsgReceiver = 0;
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPipeQueue::Initialize(), pNameServer->Register error", 0);
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForPipeQueue::Uninitialize()
{
	if(m_pMultiMsgDeserializer)
		return CLStatus(0, 0);

/*清理private pipe里面剩余的msg pointer，进行del*/
/*	CLStatus s = m_pMsgReceiver->GetMessage(m_MessageQueue);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPipeQueue::Uninitialize(), m_pMsgReceiver->GetMessage() error", 0);
		return s;
	}

	while(!m_MessageQueue.empty())
	{
		CLMessage* pMsg = m_MessageQueue.front();
		m_MessageQueue.pop();

		if(pMsg)
			delete pMsg;
	}
*/
/*end*/

	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPipeQueue::Uninitialize(), CLExecutiveNameServer::GetInstance error", 0);
		return CLStatus(-1, 0);
	}

	return pNameServer->ReleaseCommunicationPtr(m_strThreadName.c_str());
}

CLStatus CLMsgLoopManagerForPipeQueue::WaitForMessage()
{
	CLStatus s = m_pEvent->Wait();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPipeQueue::WaitForMessage(), m_pEvent->Wait error", 0);
		return s;
	}
	return m_pMsgReceiver->GetMessage(m_MessageQueue);
}

CLStatus CLMsgLoopManagerForPipeQueue::RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer)
{
	if(!m_pMultiMsgDeserializer)
		return CLStatus(-1, 0);
	
	CLStatus s = m_pMultiMsgDeserializer->RegisterDeserializer(lMsgID, pDeserializer);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPipeQueue::RegisterDeserializer(), m_pMultiMsgDeserializer->RegisterDeserializer() error", 0);
		return s;
	}
	return CLStatus(0, 0);
}