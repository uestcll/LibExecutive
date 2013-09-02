#ifndef CLProtocolEncapsulator_H
#define CLProtocolEncapsulator_H

#include "CLStatus.h"

class CLProtocolEncapsulator
{
public:
	CLProtocolEncapsulator();
	explicit CLProtocolEncapsulator(CLProtocolEncapsulator *pProtocolEncapsulator);
	virtual ~CLProtocolEncapsulator();

	virtual CLStatus Encapsulate(char *)

private:
	CLProtocolEncapsulator(const CLProtocolEncapsulator&);
	CLProtocolEncapsulator& operator=(const CLProtocolEncapsulator&);

private:
	CLProtocolEncapsulator *m_pNextProtocolEncapsulator;
};

#endif