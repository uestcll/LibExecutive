#include <string.h>
#include "CLMsgLoopManagerForSTLqueue.h"
#include "CLDataReceiverBySTLQueue.h"
#include "CLExecutiveNameServer.h"
#include "CLMessagePoster.h"
#include "CLLogger.h"
#include "CLPointerMsgDeserializer.h"
#include "CLSTLQueue.h"
#include "CLEvent.h"
#include "CLMessagePoster.h"
#include "CLDataPosterChannelBySTLQueueMaintainer.h"
#include "CLPointerMsgSerializer.h"
#include "CLProtoParserForPointerMsg.h"

CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(CLMessageObserver *pMsgObserver, const char* pstrThreadName) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(), pstrThreadName error";
		
	m_strThreadName = pstrThreadName;
	m_pStlQueue = new CLSTLQueue();
	m_pEvent = new CLEvent(true);
	m_pMsgDeserializer = new CLPointerMsgDeserializer();
	m_pMsgReceiver = new CLMessageReceiver(new CLDataReceiverBySTLQueue(m_pStlQueue), new CLProtoParserForPointerMsg(), m_pMsgDeserializer);
}

CLMsgLoopManagerForSTLqueue::~CLMsgLoopManagerForSTLqueue()
{
	delete m_pMsgDeserializer;
    delete m_pMsgReceiver;
}

CLStatus CLMsgLoopManagerForSTLqueue::Initialize()
{
	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		delete m_pMsgReceiver;
		m_pMsgReceiver = 0;
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForMsgQueue::Initialize(), CLExecutiveNameServer::GetInstance error", 0);
		return CLStatus(-1, 0);
	}

	CLStatus s = pNameServer->Register(m_strThreadName.c_str(), new CLMessagePoster(new CLDataPosterChannelBySTLQueueMaintainer(m_pStlQueue), new CLPointerMsgSerializer(), NULL, m_pEvent));
	if(!s.IsSuccess())
	{
		delete m_pMsgReceiver;
		m_pMsgReceiver = 0;
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForMsgQueue::Initialize(), pNameServer->Register error", 0);
		return CLStatus(-1, 0);
	}
	else
	{
		return CLStatus(0, 0);
	}
}

CLStatus CLMsgLoopManagerForSTLqueue::Uninitialize()
{
	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForMsgQueue::Uninitialize(), CLExecutiveNameServer::GetInstance error", 0);
		return CLStatus(-1, 0);
	}

	return pNameServer->ReleaseCommunicationPtr(m_strThreadName.c_str());
}
	
CLStatus CLMsgLoopManagerForSTLqueue::WaitForMessage()
{
	CLStatus s = m_pEvent->Wait();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForSTLqueue::WaitForMessage(), m_pEvent->Wait error", 0);
		return s;
	}
	return m_pMsgReceiver->GetMessage(m_MessageQueue);
}
