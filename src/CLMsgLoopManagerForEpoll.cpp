#include "CLMsgLoopManagerForEpoll.h"
#include "CLStatus.h"
#include "CLLogger.h"
#include "CLEpoll.h"
#include "CLMessageObserver.h"
#include "CLEpollEvent.h"
#include "CLMultiMsgDeserializer.h"
#include "CLMessageDeserializer.h"
#include "CLMsgLoopManagerForPipeQueue.h"
#include "CLMessageReceiverByTimerFd.h"
#include "CLTimerFd.h"
#include "CLPointerMsgDeserializer.h"
#include "CLProtoParserForPointerMsg.h"
#include "CLProtoParserForDefaultMsgFormat"


CLMsgLoopManagerForEpoll::CLMsgLoopManagerForEpoll(CLMessageObserver *pMessageObserver, CLEpoll *pEpoll, CLMessageDeserializer *pMsgDeserializer) : CLMessageLoopManager(pMessageObserver)
{
	m_pEpoll = pEpoll;
	m_pMsgReceiver = NULL;
	m_pMultiMsgDeserializer = pMsgDeserializer;
	if(!m_pMultiMsgDeserializer)
		m_pMultiMsgDeserializer = new CLMultiMsgDeserializer();
	m_pPointerMsgDeserializer = new CLPointerMsgDeserializer();
}	

CLMsgLoopManagerForEpoll::~CLMsgLoopManagerForEpoll()
{
	map<fd, CLMessageReceiver*>::iterator it = m_MsgReceiverMap.begin();

	for(; it != m_MsgReceiverMap.end(); ++it)
		delete it->second;

	map<fd, CLEpollEvent*>::iterator it = m_EpollEventMap.begin();
	for(; it != m_EpollEventMap.end(); ++it)
		delete it->second;

	if(m_pPointerMsgDeserializer)
		delete m_pPointerMsgDeserializer;
	if(m_pMultiMsgDeserializer)
		delete m_pMultiMsgDeserializer;
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
	// CLStatus s = m_pEpoll->Initialize(EPOLL_MAX_FD_SIZE);
	// if(!s.IsSuccess())
	// {
	// 	CLLogger::WriteLogMsg("In CLMessageLoopManager::EnterMessageLoop(), m_pEpoll->Initialize() error", 0);
	// 	return CLStatus(-1, 0);
	// }
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

CLStatus CLMsgLoopManagerForEpoll::RegisterMsgReceiver(CLMessageReceiver *pReceiver)
{
	int fd = pReceiver->GetFd();

	if(0 > fd)
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForEpoll::RegisterMsgReceiver(), fd < 0", 0);
		return CLStatus(-1, 0);
	}

	map<fd, CLMessageReceiver*>::iterator it = m_MsgReceiverMap.find(fd);
	if(it != m_MsgReceiverMap.end())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForEpoll::RegisterMsgReceiver(), it != m_MsgReceiverMap.end()", 0);
		return CLStatus(-1, 0);
	}
	m_MsgReceiverMap[fd] = pReceiver;

	map<fd, CLEpollEvent*>::iterator it = m_EpollEventMap.find(fd);
	if(it != m_EpollEventMap.end())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForEpoll::RegisterMsgReceiver(), it != m_EpollEventMap.end()", 0);
		return CLStatus(-1, 0);
	}

	CLEpollEvent *pEvent = new CLEpollEvent(m_pEpoll);
	pEvent->SetHandler(this);
	pEvent->SetFd(fd);

	CLStatus s = pEvent->RegisterREvent();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForEpoll::RegisterMsgReceiver(), pEvent->RegisterREvent() error", 0);
		return CLStatus(-1, 0);
	}	
	m_EpollEventMap[fd] = pEvent;

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForEpoll::UnRegisterMsgReceiver(int fd)
{
	map<fd, CLEpollEvent*>::iterator it = m_EpollEventMap.find(fd);
	if(it == m_EpollEventMap.end())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForEpoll::UnRegisterMsgReceiver(), it == m_EpollEventMap.end()", 0);
		return CLStatus(-1, 0);
	}
	CLStatus s = it->second->UnRegisterRWEvents();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForEpoll::UnRegisterMsgReceiver(), UnRegisterRWEvents() error", 0);
		return CLStatus(-1 ,0);
	}
	delete it->second;
	m_EpollEventMap.erase(it);

	map<fd, CLMessageReceiver*>::iterator it = m_MsgReceiverMap.find(fd);
	if(it == m_MsgReceiverMap.end())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForEpoll::UnRegisterMsgReceiver(), it == m_MsgReceiverMap.end()", 0);
		return CLStatus(-1, 0);
	}
	delete it->second;
	m_MsgReceiverMap.erase(it);

	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForEpoll::RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer)
{
	CLStatus s = m_pMultiMsgDeserializer->RegisterDeserializer(lMsgID, pDeserializer);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForEpoll::RegisterDeserializer(), m_pMultiMsgDeserializer->RegisterDeserializer() error", 0);
		return s;
	}
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
		//此处 要不要回收这个 msgReceiver？？
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

CLStatus CLMsgLoopManagerForEpoll::RegisterPipeReceiver(string strPipeName, int pipeType)
{
	if(pipeType == PIPE_QUEUE_BETWEEN_PROCESS)
	{
		return RegisterMsgReceiver(new CLMessageReceiver(new CLDataReceiverByNamedPipe(strPipeName.c_str(), true), new CLProtoParserForDefaultMsgFormat(), m_pMultiMsgDeserializer));
	
	}
	else if(pipeType == PIPE_QUEUE_IN_PROCESS)
	{
		
		return RegisterMsgReceiver(new CLMessageReceiver(new CLDataReceiverByNamedPipe(strPipeName.c_str()), new CLProtoParserForPointerMsg(), m_pPointerMsgDeserializer));
	}
	else
	{
		CLLogger::WriteLogMsg("In CLMsgLoopManagerForEpoll::RegisterPipeReceiver(), pipeType error", 0);
	}
	return CLStatus(-1, 0);
}

CLStatus CLMsgLoopManagerForEpoll::RegisterTimerReceiver(struct itimerspec& sTimerValue, string pstrRemoteName, const int& ID)
{
	return RegisterMsgReceiver(new CLMessageReceiverByTimerFd(new CLTimerFd(sTimerValue, pstrRemoteName, ID)));

}