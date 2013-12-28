#ifndef CLProtocolDecapsulatorByDefaultMsgFormat_H
#define CLProtocolDecapsulatorByDefaultMsgFormat_H

#include "CLProtocolDecapsulator.h"

class CLIteratorForIOVectors;

class CLProtocolDecapsulatorByDefaultMsgFormat : public CLProtocolDecapsulator
{
public:
	CLProtocolDecapsulatorByDefaultMsgFormat();
	virtual ~CLProtocolDecapsulatorByDefaultMsgFormat();

	virtual CLStatus Decapsulate(CLIOVectors& IOVectorsForData, unsigned int Length, std::vector<CLIOVectors *>& vSerializedMsgs, CLIOVectors& IOVectorsForPartialData, void *pContext);

private:
	int IsDataComplete(CLIOVectors& IOVectorsForData, unsigned int LeftLength, CLIteratorForIOVectors& iter);

private:
	CLProtocolDecapsulatorByDefaultMsgFormat(const CLProtocolDecapsulatorByDefaultMsgFormat&);
	CLProtocolDecapsulatorByDefaultMsgFormat& operator=(const CLProtocolDecapsulatorByDefaultMsgFormat&);
};

#endif