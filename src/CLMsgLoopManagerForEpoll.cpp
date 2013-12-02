#include "CLMsgLoopManagerForEpoll.h"
#include "CLStatus.h"
#include "CLLogger.h"
#include "CLEpoll.h"
#include "CLMessageObserver.h"

CLMsgLoopManagerForEpoll::CLMsgLoopManagerForEpoll(CLMessageObserver *pMessageObserver, CLEpoll *pEpoll) : CLMessageLoopManager(pMessageObserver)
{
	m_pEpoll = pEpoll;
	m_pMsgReceiver = NULL;
}

CLMsgLoopManagerForEpoll::~CLMsgLoopManagerForEpoll()
{

}

CLStatus CLMsgLoopManagerForEpoll::Register(unsigned long lMsgID, CallBackForMessageLoop pMsgProcessFunction)
{
	if(pMsgProcessFunction == 0)
		return CLStatus(-1, 0);
	
	m_MsgMappingTable[lMsgID] = pMsgProcessFunction;

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForEpoll::Initialize()
{
	CLStatus s = m_pEpoll->Initialize(EPOLL_MAX_FD_SIZE, this);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessageLoopManager::EnterMessageLoop(), m_pEpoll->Initialize() error", 0);
		return CLStatus(-1, 0);
	}
}

CLStatus CLMsgLoopManagerForEpoll::EnterMessageLoop(void *pContext)
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

	bool bQuitLoop = false;
	
	m_pEpoll->Run();

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForEpoll::Uninitialize()
{


	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForEpoll::WaitForMessage()
{
	if(m_pMsgReceiver)
		return m_pMsgReceiver->GetMessage(m_MessageQueue);
	else 
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForEpoll::WaitForMessage(), m_pMsgReceiver is NULL", 0);
		return CLStatus(-1, 0);
	}
}

CLStatus CLMsgLoopManagerForEpoll::NotifyReadable(int fd)
{
	map<fd, CLMessageReceiver*>::iterator it =  m_MsgReceiverMap.find(fd);

	if(it == m_MsgReceiverMap.end())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForEpoll::NotifyReadable(), it == m_MsgReceiverMap.end()", 0);
		return CLStatus(-1, 0);
	}

	m_pMsgReceiver = it->second;

	CLStatus s = WaitForMessage();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForEpoll::NotifyReadable(), WaitForMessage() error", 0);
		return CLStatus(-1, 0);
	}

	while(!m_MessageQueue.empty())
	{
		CLMessage* pMsg = m_MessageQueue.front();
		m_MessageQueue.pop();

		if(pMsg != 0)
		{
			CLStatus s1 = DispatchMessage(pMsg);

			delete pMsg;

			// if(s4.m_clReturnCode == QUIT_MESSAGE_LOOP)
			// {
			// 	bQuitLoop = true;
			// 	break;
			// }
		}
	}

	return CLStatus(0, 0);
}