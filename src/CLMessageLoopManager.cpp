#include "CLMessageLoopManager.h"
#include "CLMessageObserver.h"
#include "CLMessage.h"
#include "CLLogger.h"
#include "CLExecutiveInitialFinishedNotifier.h"

CLMessageLoopManager::CLMessageLoopManager(CLMessageObserver *pMessageObserver)
{
	if(pMessageObserver == 0)
		throw "In CLMessageLoopManager::CLMessageLoopManager(), pMessageObserver error";
	
	m_pMessageObserver = pMessageObserver;
}

CLMessageLoopManager::~CLMessageLoopManager()
{
	while(!m_MessageContainer.empty())
	{
		CLMessage *pMsg = m_MessageContainer.front();
		m_MessageContainer.pop();
		if(pMsg)
			delete pMsg;
	}

	delete m_pMessageObserver;
}

CLStatus CLMessageLoopManager::Register(unsigned long lMsgID, CallBackForMessageLoop pMsgProcessFunction)
{
	if(pMsgProcessFunction == 0)
		return CLStatus(-1, 0);
	
	m_MsgMappingTable[lMsgID] = pMsgProcessFunction;

	return CLStatus(0, 0);
}

CLStatus CLMessageLoopManager::EnterMessageLoop(void *pContext)
{	
	SLExecutiveInitialParameter *para = (SLExecutiveInitialParameter *)pContext;

	if((para == 0) || (para->pNotifier == 0))
		return CLStatus(-1, 0);
	
	CLStatus s = Initialize();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessageLoopManager::EnterMessageLoop(), Initialize error", 0);
		para->pNotifier->NotifyInitialFinished(false);
		return CLStatus(-1, 0);
	}

	CLStatus s1 = m_pMessageObserver->Initialize(this, para->pContext);
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessageLoopManager::EnterMessageLoop(), m_pMessageObserver->Initialize error", 0);

		CLStatus s2 = Uninitialize();
		if(!s2.IsSuccess())
			CLLogger::WriteLogMsg("In CLMessageLoopManager::EnterMessageLoop(), Uninitialize() error", 0);	

		para->pNotifier->NotifyInitialFinished(false);		
		return CLStatus(-1, 0);
	}

	para->pNotifier->NotifyInitialFinished(true);

	bool bQuit = false;
	
	while(true)
	{
		CLStatus s5 = WaitForMessage();
		if(!s5.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMessageLoopManager::EnterMessageLoop(), WaitForMessage error", 0);
			continue;
		}

		while(!m_MessageContainer.empty())
		{
			CLMessage *pMsg = m_MessageContainer.front();
			m_MessageContainer.pop();

			if(pMsg)
			{
				CLStatus s3 = DispatchMessage(pMsg);

				delete pMsg;

				if(s3.m_clReturnCode == QUIT_MESSAGE_LOOP)
				{
					bQuit = true;
					break;
				}
			}
		}

		if(bQuit)
			break;
	}

	CLStatus s4 = Uninitialize();
	if(!s4.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessageLoopManager::EnterMessageLoop(), Uninitialize() error", 0);
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}

CLStatus CLMessageLoopManager::DispatchMessage(CLMessage *pMessage)
{
	std::map<unsigned long, CallBackForMessageLoop>::iterator it;
	it = m_MsgMappingTable.find(pMessage->m_clMsgID);

	if(it == m_MsgMappingTable.end())
	{
		CLLogger::WriteLogMsg("In CLMessageLoopManager::MessageDispatch(), it == m_MsgMappingTable.end", 0);
		return CLStatus(-1, 0);
	}
	
	CallBackForMessageLoop pFunction = it->second;

	return (m_pMessageObserver->*pFunction)(pMessage);
}