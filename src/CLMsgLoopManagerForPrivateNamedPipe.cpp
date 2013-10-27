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
	m_pMsgReceiver = new CLMessageReceiver(new CLBufferManager(), new CLDataReceiverByNamedPipe(strPath.c_str()), new CLPointerToMsgDeserializer(), new CLProtocolDecapsulatorBySplitPointer);
}

CLMsgLoopManagerForPrivateNamedPipe::~CLMsgLoopManagerForPrivateNamedPipe()
{
	delete m_pMsgReceiver;
}

CLStatus CLMsgLoopManagerForPrivateNamedPipe::Initialize()
{
	try
	{
		CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
		if(pNameServer == 0)
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForPrivateNamedPipe::Initialize(), CLExecutiveNameServer::GetInstance error", 0);
			throw CLStatus(-1, 0);
		}

		string strPath = EXECUTIVE_CHANNEL_PATH;
		strPath += m_strThreadName;

		CLStatus s = pNameServer->Register(m_strThreadName.c_str(), new CLMessagePoster(new CLMsgToPointerSerializer(), 0, new CLDataPostChannelByNamedPipeMaintainer(strPath.c_str()), m_pEvent));
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForPrivateNamedPipe::Initialize(), pNameServer->Register error", 0);
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

CLStatus CLMsgLoopManagerForPrivateNamedPipe::Uninitialize()
{
	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPrivateNamedPipe::Uninitialize(), CLExecutiveNameServer::GetInstance error", 0);
		return CLStatus(-1, 0);
	}

	return pNameServer->ReleaseCommunicationPtr(m_strThreadName.c_str());
}

CLStatus CLMsgLoopManagerForPrivateNamedPipe::WaitForMessage()
{
	CLStatus s = m_pEvent->Wait();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForPrivateNamedPipe::WaitForMessage(), m_Event.Wait error", 0);
		return CLStatus(-1, 0);
	}

	return m_pMsgReceiver->GetMessage(m_MessageContainer);
}