#ifndef CLPROTOPARSERFORSHAREDPIPE_H
#define CLPROTOPARSERFORSHAREDPIPE_H

#include "CLProtoParser.h"

class CLProtoParserForSharedPipe : public CLProtoParser
{
public:
	CLProtoParserForSharedPipe();
	virtual ~CLProtoParserForSharedPipe();

	virtual CLStatus DecapsulateMsg(CLIOVector& dataVec, vector<CLIOVector *> &vSerializedMsgs);
};

#endif