#ifndef CLEXECUTIVEFUNCTIONFORMSGLOOP_H
#define CLEXECUTIVEFUNCTIONFORMSGLOOP_H

#include "CLStatus.h"
#include "CLExecutiveFunctionProvider.h"

class CLMessageLoopManager;

/*
该类帮助线程进入消息循环
构造函数中，pMsgLoopManager从堆中分配，不必调用delete
*/
class CLExecutiveFunctionForMsgLoop : public CLExecutiveFunctionProvider
{
public:
	explicit CLExecutiveFunctionForMsgLoop(CLMessageLoopManager *pMsgLoopManager);
	virtual ~CLExecutiveFunctionForMsgLoop();

	virtual CLStatus RunExecutiveFunction(void* pContext);

private:
	CLExecutiveFunctionForMsgLoop(const CLExecutiveFunctionForMsgLoop&);
	CLExecutiveFunctionForMsgLoop& operator=(const CLExecutiveFunctionForMsgLoop&);

private:
	CLMessageLoopManager *m_pMsgLoopManager;
};

#endif
