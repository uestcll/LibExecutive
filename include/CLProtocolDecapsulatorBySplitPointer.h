#ifndef CLProtocolDecapsulatorBySplitPointer_H
#define CLProtocolDecapsulatorBySplitPointer_H

#include "CLProtocolDecapsulator.h"

class CLProtocolDecapsulatorBySplitPointer : public CLProtocolDecapsulator
{
public:
	CLProtocolDecapsulatorBySplitPointer();
	virtual ~CLProtocolDecapsulatorBySplitPointer();

	virtual CLStatus Decapsulate(CLIOVectors& IOVectorsForData, std::vector<CLIOVectors& >& vSerializedMsgs, CLIOVectors& IOVectorForPartialData, void *pContext);

private:
	CLProtocolDecapsulatorBySplitPointer(const CLProtocolDecapsulatorBySplitPointer&);
	CLProtocolDecapsulatorBySplitPointer& operator=(const CLProtocolDecapsulatorBySplitPointer&);
};

#endif