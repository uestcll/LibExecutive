#include <string.h>
#include "CLMsgLoopManagerForSTLqueue.h"
#include "CLMessageReceiverBySTLqueue.h"
#include "CLExecutiveNameServer.h"
#include "CLThreadCommunicationBySTLqueue.h"
#include "CLLogger.h"

CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(CLMessageObserver *pMsgObserver, const char* pstrThreadName) : CLMessageLoopManager(pMsgObserver), m_Event(true)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(), pstrThreadName error";
		
	m_strThreadName = pstrThreadName;

	m_pMsgReceiver = new CLMessageReceiverBySTLqueue;
}

CLMsgLoopManagerForSTLqueue::~CLMsgLoopManagerForSTLqueue()
{
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

	CLStatus s = pNameServer->Register(m_strThreadName.c_str(), new CLThreadCommunicationBySTLqueue(m_pMsgReceiver, &m_Event));
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
	
CLMessage* CLMsgLoopManagerForSTLqueue::WaitForMessage()
{
	CLStatus s = m_Event.Wait();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForSTLqueue::WaitForMessage(), m_Event.Wait error", 0);
		return 0;
	}
	return m_pMsgReceiver->GetMessage();
}
