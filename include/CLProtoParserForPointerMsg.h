#ifndef CLPROTOPARSERFORPOINTERMSG_H
#define CLPROTOPARSERFORPOINTERMSG_H

#include "CLProtoParser.h"

class CLProtoParserForPointerMsg : public CLProtoParser
{
public:
	CLProtoParserForPointerMsg();
	virtual ~CLProtoParserForPointerMsg();

public:
	virtual CLStatus DecapsulateMsg(CLIOVector& dataVec, vector<CLIOVector *> &vSerializedMsgs);

private:
	CLProtoParserForPointerMsg(const CLProtoParserForPointerMsg &);
	CLProtoParserForPointerMsg& operator=(const CLProtoParserForPointerMsg &);
};


#endif