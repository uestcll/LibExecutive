#ifndef CLProtocolDecapsulatorBySplitPointer_H
#define CLProtocolDecapsulatorBySplitPointer_H

#include "CLProtocolDecapsulator.h"

class CLProtocolDecapsulatorBySplitPointer : public CLProtocolDecapsulator
{
public:
	CLProtocolDecapsulatorBySplitPointer();
	virtual ~CLProtocolDecapsulatorBySplitPointer();

	virtual CLStatus Decapsulate(CLIOVectors& IOVectorsForData, unsigned int Length, std::vector<CLIOVectors *>& vSerializedMsgs, CLIOVectors& IOVectorForPartialData, void *pContext);

private:
	CLProtocolDecapsulatorBySplitPointer(const CLProtocolDecapsulatorBySplitPointer&);
	CLProtocolDecapsulatorBySplitPointer& operator=(const CLProtocolDecapsulatorBySplitPointer&);
};

#endif