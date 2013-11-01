#ifndef CLProtocolEncapsulator_H
#define CLProtocolEncapsulator_H

#include "CLStatus.h"

class CLIOVector;

class CLProtocolEncapsulator
{
public:
	CLProtocolEncapsulator();
	virtual ~CLProtocolEncapsulator();

public:
	virtual CLStatus Encapsulate(CLIOVector *pDataVec) = 0;

private:
	CLProtocolEncapsulator(const CLProtocolEncapsulator&);
	CLProtocolEncapsulator& operator=(const CLProtocolEncapsulator&);

};

#endif