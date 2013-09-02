#include <string.h>
#include "CLMsgLoopManagerForSTLqueue.h"
#include "CLMessageReceiverFromSTLqueue.h"
#include "CLExecutiveNameServer.h"
#include "CLThreadCommunicationBySTLqueue.h"
#include "CLLogger.h"
#include "CLEvent.h"

CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(CLMessageObserver *pMsgObserver, const char* pstrThreadName) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(), pstrThreadName error";
		
	m_strThreadName = pstrThreadName;
	m_pMsgReceiver = new CLMessageReceiverFromSTLqueue;
	m_pEvent = new CLEvent(true);
}

CLMsgLoopManagerForSTLqueue::~CLMsgLoopManagerForSTLqueue()
{
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

		CLStatus s = pNameServer->Register(m_strThreadName.c_str(), new CLThreadCommunicationBySTLqueue(m_pMsgReceiver, m_pEvent));
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForSTLqueue::Initialize(), pNameServer->Register error", 0);
			throw s;
		}

		return CLStatus(0, 0);
	}
	catch(CLStatus& s1)
	{
		delete m_pMsgReceiver;
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
	
CLMessage* CLMsgLoopManagerForSTLqueue::WaitForMessage()
{
	CLStatus s = m_pEvent->Wait();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForSTLqueue::WaitForMessage(), m_Event.Wait error", 0);
		return 0;
	}

	CLMessage *pMsg = NULL;
	m_pMsgReceiver->GetMessage(&pMsg);
	
	return pMsg;
}