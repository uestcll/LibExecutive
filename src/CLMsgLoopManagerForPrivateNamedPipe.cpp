#include <string.h>
#include "CLMsgLoopManagerForPrivateNamedPipe.h"
#include "CLMessageReceiver.h"
#include "CLDataReceiverByNamedPipe.h"
#include "CLPointerToMsgDeserializer.h"
#include "CLExecutiveNameServer.h"
#include "CLLogger.h"
#include "CLEvent.h"
#include "ErrorCode.h"
#include "CLBufferManager.h"
#include "CLMessagePoster.h"
#include "CLMsgToPointerSerializer.h"
#include "CLDataPostChannelByNamedPipeMaintainer.h"
#include "CLProtocolDecapsulatorBySplitPointer.h"
#include "CLInitialDataPostChannelNotifier.h"

using namespace std;

#define EXECUTIVE_CHANNEL_PATH "/tmp/"

CLMsgLoopManagerForPrivateNamedPipe::CLMsgLoopManagerForPrivateNamedPipe(CLMessageObserver *pMsgObserver, const char* pstrThreadName) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForPrivateNamedPipe::CLMsgLoopManagerForPrivateNamedPipe(), pstrThreadName error";

	m_strThreadName = pstrThreadName;

	string strPath = EXECUTIVE_CHANNEL_PATH;
	strPath += pstrThreadName;

	m_pEvent = new CLEvent(true);
	CLDataReceiver *pDataReceiver = new CLDataReceiverByNamedPipe(strPath.c_str());
	m_Uuid = pDataReceiver->GetUuid();
	m_pMsgReceiver = new CLMessageReceiver(&m_Uuid, new CLBufferManager(), pDataReceiver, new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer);
}

CLMsgLoopManagerForPrivateNamedPipe::~CLMsgLoopManagerForPrivateNamedPipe()
{
}

CLStatus CLMsgLoopManagerForPrivateNamedPipe::Initialize()
{
	if(m_pMsgReceiver == 0)
		return CLStatus(-1, NORMAL_ERROR);

	CLMessagePoster *pMsgPoster = 0;

	try
	{
		CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
		if(pNameServer == 0)
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForPrivateNamedPipe::Initialize(), CLExecutiveNameServer::GetInstance error", 0);
			throw CLStatus(-1, NORMAL_ERROR);
		}

		string strPath = EXECUTIVE_CHANNEL_PATH;
		strPath += m_strThreadName;

		pMsgPoster = new CLMessagePoster(new CLMsgToPointerSerializer(), 0, new CLDataPostChannelByNamedPipeMaintainer(strPath.c_str()), m_pEvent);

		CLStatus s2 = pMsgPoster->Initialize(new CLInitialDataPostChannelNotifier(), 0);
		if(!s2.IsSuccess() && (s2.m_clErrorCode == DATA_POSTER_INITIALIZE_ERROR))
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForPrivateNamedPipe::Initialize(), pMsgPoster->Initialize error", 0);
			throw CLStatus(-1, NORMAL_ERROR);
		}

		CLStatus s = pNameServer->Register(m_strThreadName.c_str(), m_Uuid, pMsgPoster, m_pMsgReceiver);
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForPrivateNamedPipe::Initialize(), pNameServer->Register error", 0);
			
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
			delete m_pEvent;

		return s1;
	}
}

CLStatus CLMsgLoopManagerForPrivateNamedPipe::Uninitialize()
{
	m_pMsgReceiver = 0;

	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPrivateNamedPipe::Uninitialize(), CLExecutiveNameServer::GetInstance error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	return pNameServer->ReleaseCommunicationPtr(m_strThreadName.c_str());
}

CLStatus CLMsgLoopManagerForPrivateNamedPipe::WaitForMessage()
{
	CLStatus s = m_pEvent->Wait();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPrivateNamedPipe::WaitForMessage(), m_Event.Wait error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	long old_size = m_MessageContainer.size();

	CLStatus s1 = m_pMsgReceiver->GetMessage(m_MessageContainer);

	long new_size = m_MessageContainer.size();

	if(new_size > old_size)
	{
		if(!(m_pEvent->ReleaseSemaphore(new_size - old_size - 1).IsSuccess()))
		{
			//CLLogger::WriteLogMsg("In CLMsgLoopManagerForPrivateNamedPipe::WaitForMessage(), m_pEvent->ReleaseSemaphore error; but may be made by the sequence of sendmsg.set.(wait).sendmsg(compute new_size).set", 0);
		}
	}

	return s1;
}