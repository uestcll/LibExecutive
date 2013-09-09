#ifndef CLProtocolEncapsulator_H
#define CLProtocolEncapsulator_H

#include <uuid/uuid.h>
#include "CLStatus.h"

class CLIOVectors;

class CLProtocolEncapsulator
{
public:
	CLProtocolEncapsulator();
	explicit CLProtocolEncapsulator(CLProtocolEncapsulator *pProtocolEncapsulator);
	virtual ~CLProtocolEncapsulator();

	virtual CLStatus Initialize(uuid_t uuidOfCommObj) = 0;
	virtual CLStatus Uninitialize() = 0;

	virtual CLStatus Encapsulate(CLIOVectors *pIOVectors) = 0;

private:
	CLProtocolEncapsulator(const CLProtocolEncapsulator&);
	CLProtocolEncapsulator& operator=(const CLProtocolEncapsulator&);

protected:
	CLProtocolEncapsulator *m_pNextProtocolEncapsulator;
};

#endif