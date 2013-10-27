#include <string.h>
#include "CLMsgLoopManagerForShareNamedPipe.h"
#include "CLMessageReceiver.h"
#include "CLDataReceiverByNamedPipe.h"
#include "CLExecutiveNameServer.h"
#include "CLLogger.h"
#include "CLEvent.h"
#include "ErrorCode.h"
#include "CLBufferManager.h"
#include "CLMessagePoster.h"
#include "CLDataPostChannelByNamedPipeMaintainer.h"
#include "CLMessageDeserializer.h"
#include "CLMessageSerializer.h"
#include "CLProtocolDecapsulator.h"
#include "CLProtocolEncapsulator.h"

using namespace std;

#define EXECUTIVE_CHANNEL_PATH "/tmp/"

CLMsgLoopManagerForShareNamedPipe::CLMsgLoopManagerForShareNamedPipe(CLMessageObserver *pMsgObserver, const char* pstrThreadName, CLProtocolEncapsulator *pEncapsulator, CLProtocolDecapsulator *pDecapsulator, CLMessageSerializer *pMsgSerializer, CLMessageDeserializer *pMsgDeserializer) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0) || (pMsgDeserializer == 0) || (pMsgSerializer == 0))
		throw "In CLMsgLoopManagerForShareNamedPipe::CLMsgLoopManagerForShareNamedPipe(), pstrThreadName or pMsgDeserializer or pMsgSerializer error";

	m_strThreadName = pstrThreadName;

	string strPath = EXECUTIVE_CHANNEL_PATH;
	strPath += pstrThreadName;

	m_pEvent = new CLEvent(m_strThreadName.c_str(), true);
	m_pMsgDeserializer = pMsgDeserializer;
	m_pMsgSerializer = pMsgSerializer;
	m_pDecapsulator = pDecapsulator;
	m_pEncapsulator = pEncapsulator;

	m_pMsgReceiver = new CLMessageReceiver(new CLBufferManager(), new CLDataReceiverByNamedPipe(strPath.c_str()), m_pMsgDeserializer, m_pDecapsulator);
}

CLMsgLoopManagerForShareNamedPipe::~CLMsgLoopManagerForShareNamedPipe()
{
	delete m_pMsgReceiver;
}

CLStatus CLMsgLoopManagerForShareNamedPipe::Initialize()
{
	try
	{
		CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
		if(pNameServer == 0)
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForShareNamedPipe::Initialize(), CLExecutiveNameServer::GetInstance error", 0);
			throw CLStatus(-1, 0);
		}

		string strPath = EXECUTIVE_CHANNEL_PATH;
		strPath += m_strThreadName;

		CLStatus s = pNameServer->Register(m_strThreadName.c_str(), new CLMessagePoster(m_pMsgSerializer, m_pEncapsulator, new CLDataPostChannelByNamedPipeMaintainer(strPath.c_str()), m_pEvent));
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMsgLoopManagerForShareNamedPipe::Initialize(), pNameServer->Register error", 0);
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

CLStatus CLMsgLoopManagerForShareNamedPipe::Uninitialize()
{
	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForShareNamedPipe::Uninitialize(), CLExecutiveNameServer::GetInstance error", 0);
		return CLStatus(-1, 0);
	}

	return pNameServer->ReleaseCommunicationPtr(m_strThreadName.c_str());
}

CLStatus CLMsgLoopManagerForShareNamedPipe::WaitForMessage()
{
	CLStatus s = m_pEvent->Wait();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForShareNamedPipe::WaitForMessage(), m_Event.Wait error", 0);
		return CLStatus(-1, 0);
	}

	return m_pMsgReceiver->GetMessage(m_MessageContainer);
}