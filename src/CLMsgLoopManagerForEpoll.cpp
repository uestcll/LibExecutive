#include "CLMsgLoopManagerForEpoll.h"
#include "CLStatus.h"
#include "CLLogger.h"
#include "CLEpoll.h"
#include "CLMessageObserver.h"

CLMsgLoopManagerForEpoll::CLMsgLoopManagerForEpoll(CLMessageObserver *pMessageObserver, CLEpoll *pEpoll) : CLMessageLoopManager(pMessageObserver)
{
	m_pEpoll = pEpoll;

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
	CLStatus s = m_pEpoll->Initialize(this);
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
	

	
	return CLStatus(0, 0);
}