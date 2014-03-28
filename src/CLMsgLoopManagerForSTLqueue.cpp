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
#include "CLProtocolDecapsulatorBySplitPointer.h"
#include "CLInitialDataPostChannelNotifier.h"

CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(CLMessageObserver *pMsgObserver, const char* pstrThreadName) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(), pstrThreadName error";
		
	m_strThreadName = pstrThreadName;
	m_pEvent = new CLEvent(true);
	m_pSTLqueue = new CLSTLqueue();
	m_Uuid = m_pSTLqueue->GetUuid();
	m_pMsgReceiver = new CLMessageReceiver(&m_Uuid, new CLBufferManager(), new CLDataReceiverBySTLqueue(m_pSTLqueue), new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer());
}

CLMsgLoopManagerForSTLqueue::~CLMsgLoopManagerForSTLqueue()
{
}

CLStatus CLMsgLoopManagerForSTLqueue::Initialize()
{
	if(m_pMsgReceiver == 0)
		return CLStatus(-1, NORMAL_ERROR);

	CLMessagePoster *pMsgPoster = 0;

	try
	{
		CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
		if(pNameServer == 0)
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForSTLqueue::Initialize(), CLExecutiveNameServer::GetInstance error", 0);
			throw CLStatus(-1, NORMAL_ERROR);
		}

		pMsgPoster = new CLMessagePoster(new CLMsgToPointerSerializer, 0, new CLDataPostChannelBySTLqueueMaintainer(m_pSTLqueue), m_pEvent);

		CLStatus s2 = pMsgPoster->Initialize(new CLInitialDataPostChannelNotifier(), 0);
		if(!s2.IsSuccess() && (s2.m_clErrorCode == DATA_POSTER_INITIALIZE_ERROR))
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForSTLqueue::Initialize(), pMsgPoster->Initialize error", 0);
			throw CLStatus(-1, NORMAL_ERROR);
		}
		
		CLStatus s = pNameServer->Register(m_strThreadName.c_str(), m_Uuid, pMsgPoster, m_pMsgReceiver);
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForSTLqueue::Initialize(), pNameServer->Register error", 0);
			
			m_pMsgReceiver = 0;
			return CLStatus(-1, NORMAL_ERROR);
		}

		return CLStatus(0, 0);
	}
	catch(CLStatus& s1)
	{
		if(m_pMsgReceiver)
		{
			delete m_pMsgReceiver;
			m_pMsgReceiver = 0;
		}

		if(pMsgPoster)
			delete pMsgPoster;
		else
		{
			delete m_pEvent;
			delete m_pSTLqueue;
		}

		return s1;
	}
}

CLStatus CLMsgLoopManagerForSTLqueue::Uninitialize()
{
	m_pMsgReceiver = 0;

	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForMsgQueue::Uninitialize(), CLExecutiveNameServer::GetInstance error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	return pNameServer->ReleaseCommunicationPtr(m_strThreadName.c_str());
}
	
CLStatus CLMsgLoopManagerForSTLqueue::WaitForMessage()
{
	CLStatus s = m_pEvent->Wait();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForSTLqueue::WaitForMessage(), m_Event.Wait error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	long old_size = m_MessageContainer.size();

	CLStatus s1 = m_pMsgReceiver->GetMessage(m_MessageContainer);

	long new_size = m_MessageContainer.size();

	if(new_size > old_size)
	{
		if(!(m_pEvent->ReleaseSemaphore(new_size - old_size - 1).IsSuccess()))
		{
			//CLLogger::WriteLogMsg("In CLMsgLoopManagerForSTLqueue::WaitForMessage(), m_pEvent->ReleaseSemaphore error; but may be made by the sequence of sendmsg.set.(wait).sendmsg(compute new_size).set", 0);
		}
	}

	return s1;
}