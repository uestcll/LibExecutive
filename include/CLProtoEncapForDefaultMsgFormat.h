#ifndef CLPROTOENCAPFORDEFAULTMSGFORMAT_H
#define CLPROTOENCAPFORDEFAULTMSGFORMAT_H

#include "CLProtocolEncapsulator.h"

class CLProtoEncapForDefaultMsgFormat : public CLProtocolEncapsulator
{
public:
	CLProtoEncapForDefaultMsgFormat();
	virtual ~CLProtoEncapForDefaultMsgFormat();

public:
	virtual CLStatus Encapsulate(CLIOVector *pDataVec);

private:
	CLProtoEncapForDefaultMsgFormat(const CLProtoEncapForDefaultMsgFormat &);
	CLProtoEncapForDefaultMsgFormat& operator=(const CLProtoEncapForDefaultMsgFormat&);
};

#endif