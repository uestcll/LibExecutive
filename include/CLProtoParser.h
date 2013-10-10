#ifndef CLPROTOPARSER_H
#define CLPROTOPARSER_H

#include "CLStatus.h"

class CLMessage;
class CLBuffer;
class CLIOVector;

class CLProtoParser
{
public:
	CLProtoParser();
	virtual ~CLProtoParser();

public:
	CLStatus Decapsulate(CLBuffer* pBuffer, vector<CLIOVector *> vSerializedMsgs);

protected:
	virtual CLStatus DecapsulateMsg(CLBuffer *pBuffer, int& decapsulateStartIndex, vector<CLIOVector *> vSerializedMsgs) = 0;

};

#endif