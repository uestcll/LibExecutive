#ifndef CLEXECUTIVE_H
#define CLEXECUTIVE_H

#include "CLStatus.h"

class CLExecutiveFunctionProvider;

/* 
CLExecutive类用于创建一个执行体
*/
class CLExecutive
{
public:
	/*
	构造函数中的CLExecutiveFunctionProvider类型参数，必须从堆中分配，且分配者
	不必再调用delete释放内存
	*/
	explicit CLExecutive(CLExecutiveFunctionProvider *pExecutiveFunctionProvider);
	virtual ~CLExecutive();

	/*
	Run方法用于创建一个执行体，当新执行体被真正创建后，Run方法才返回
	Run方法若返回出错，则本对象和CLExecutiveFunctionProvider对象都将被自动删除
	*/
	virtual CLStatus Run(void *pContext = 0) = 0;

	/*
	等待被创建的执行体死亡
	*/
	virtual CLStatus WaitForDeath() = 0;
	
protected:
	CLExecutiveFunctionProvider *m_pExecutiveFunctionProvider;

private:
	CLExecutive(const CLExecutive&);
	CLExecutive& operator=(const CLExecutive&);
};

#endif
