#ifndef CLPROTOPARSERFORDEFAULTMSGFORMAT_H
#define CLPROTOPARSERFORDEFAULTMSGFORMAT_H

#include "CLProtoParser.h"

class CLProtoParserForDefaultMsgFormat : public CLProtoParser
{
public:
	CLProtoParserForDefaultMsgFormat();
	virtual ~CLProtoParserForDefaultMsgFormat();

	virtual CLStatus DecapsulateMsg(CLIOVector& dataVec, vector<CLIOVector *> &vSerializedMsgs);
};

#endif