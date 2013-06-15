#ifndef CLEXECUTIVEFUNCTIONPROVIDER_H
#define CLEXECUTIVEFUNCTIONPROVIDER_H

#include "CLStatus.h"

/*
CLExecutiveFunctionProvider类封装了执行体具体的业务功能
*/
class CLExecutiveFunctionProvider
{
public:
	CLExecutiveFunctionProvider();
	virtual ~CLExecutiveFunctionProvider();

public:
	virtual CLStatus RunExecutiveFunction(void* pContext) = 0;

private:
	CLExecutiveFunctionProvider(const CLExecutiveFunctionProvider&);
	CLExecutiveFunctionProvider& operator=(const CLExecutiveFunctionProvider&);
};

#endif
