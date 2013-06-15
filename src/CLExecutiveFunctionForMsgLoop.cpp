#include "CLExecutiveFunctionForMsgLoop.h"
#include "CLMessageLoopManager.h"

CLExecutiveFunctionForMsgLoop::CLExecutiveFunctionForMsgLoop(CLMessageLoopManager *pMsgLoopManager)
{
	if(pMsgLoopManager == 0)
		throw "In CLExecutiveFunctionForMsgLoop::CLExecutiveFunctionForMsgLoop(), pMsgLoopManager error";
	
	m_pMsgLoopManager = pMsgLoopManager;
}

CLExecutiveFunctionForMsgLoop::~CLExecutiveFunctionForMsgLoop()
{
	delete m_pMsgLoopManager;
}

CLStatus CLExecutiveFunctionForMsgLoop::RunExecutiveFunction(void* pContext)
{
	return m_pMsgLoopManager->EnterMessageLoop(pContext);
}
