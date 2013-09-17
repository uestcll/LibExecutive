#ifndef CLProtocolDecapsulator_H
#define CLProtocolDecapsulator_H

#include <vector>
#include "CLStatus.h"

class CLIOVectors;

struct SLMessageScopeInIOVectors
{
	unsigned int Index;
	unsigned int Length;
};

class CLProtocolDecapsulator
{
public:
	explicit CLProtocolDecapsulator(CLProtocolDecapsulator *pProtocolDecapsulator);
	virtual ~CLProtocolDecapsulator();

	virtual CLStatus Decapsulate(CLIOVectors& IOVectors, std::vector<CLIOVectors>& vSerializedMsgs, SLMessageScopeInIOVectors *pPartialMsgScope, void *pContext) = 0;

private:
	CLProtocolDecapsulator(const CLProtocolDecapsulator&);
	CLProtocolDecapsulator& operator=(const CLProtocolDecapsulator&);

protected:
	CLProtocolDecapsulator *m_pProtocolDecapsulator;
};

#endif