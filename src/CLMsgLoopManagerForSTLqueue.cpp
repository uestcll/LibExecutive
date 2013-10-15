#include <string.h>
#include "CLMsgLoopManagerForSTLqueue.h"
#include "CLMessageReceiver.h"
#include "CLDataReceiverBySTLqueue.h"
#include "CLPointerToMsgDeserializer.h"
#include "CLExecutiveNameServer.h"
#include "CLLogger.h"
#include "CLEvent.h"
#include "ErrorCode.h"
#include "CLBufferManager.h"
#include "CLSTLqueue.h"
#include "CLMessagePoster.h"
#include "CLMsgToPointerSerializer.h"
#include "CLDataPostChannelBySTLqueueMaintainer.h"

CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(CLMessageObserver *pMsgObserver, const char* pstrThreadName) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(), pstrThreadName error";
		
	m_strThreadName = pstrThreadName;
	m_pEvent = new CLEvent(true);
	m_pSTLqueue = new CLSTLqueue();

	m_pMsgReceiver = new CLMessageReceiver(new CLBufferManager(), new CLDataReceiverBySTLqueue(m_pSTLqueue), new CLPointerToMsgDeserializer());
}

CLMsgLoopManagerForSTLqueue::~CLMsgLoopManagerForSTLqueue()
{
	delete m_pMsgReceiver;
}

CLStatus CLMsgLoopManagerForSTLqueue::Initialize()
{
	try
	{
		CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
		if(pNameServer == 0)
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForSTLqueue::Initialize(), CLExecutiveNameServer::GetInstance error", 0);
			throw CLStatus(-1, 0);
		}

		CLStatus s = pNameServer->Register(m_strThreadName.c_str(), new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelBySTLqueueMaintainer(m_pSTLqueue), m_pEvent));
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForSTLqueue::Initialize(), pNameServer->Register error", 0);
			throw s;
		}

		return CLStatus(0, 0);
	}
	catch(CLStatus& s1)
	{
		delete m_pEvent;
		return s1;
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
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForSTLqueue::WaitForMessage(), m_Event.Wait error", 0);
		return CLStatus(-1, 0);
	}

	return m_pMsgReceiver->GetMessage(m_MessageContainer);
}