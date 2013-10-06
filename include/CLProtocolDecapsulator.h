#ifndef CLProtocolDecapsulator_H
#define CLProtocolDecapsulator_H

#include <vector>
#include "CLStatus.h"

class CLIOVectors;

class CLProtocolDecapsulator
{
public:
	explicit CLProtocolDecapsulator(CLProtocolDecapsulator *pProtocolDecapsulator);
	virtual ~CLProtocolDecapsulator();

	virtual CLStatus Decapsulate(CLIOVectors& IOVectorsForData, unsigned int Length, std::vector<CLIOVectors *>& vSerializedMsgs, CLIOVectors& IOVectorForPartialData, void *pContext) = 0;

private:
	CLProtocolDecapsulator(const CLProtocolDecapsulator&);
	CLProtocolDecapsulator& operator=(const CLProtocolDecapsulator&);

protected:
	CLProtocolDecapsulator *m_pProtocolDecapsulator;
};

#endif