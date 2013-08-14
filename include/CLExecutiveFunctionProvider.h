#ifndef CLEXECUTIVEFUNCTIONPROVIDER_H
#define CLEXECUTIVEFUNCTIONPROVIDER_H

#include "CLStatus.h"

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