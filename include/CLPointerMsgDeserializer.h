#ifndef CLPOINTERMSGDESERIALIZER_H
#define CLPOINTERMSGDESERIALIZER_H

#include "CLMessageDeserializer.h"

class CLPointerMsgDeserializer : public CLMessageDeserializer
{
public:
	CLPointerMsgDeserializer();
	virtual ~CLPointerMsgDeserializer();

	virtual CLStatus Deserialize(CLIOVector dataVec, CLMessage **pMsg);

private:
	CLPointerMsgDeserializer(const CLPointerMsgDeserializer&);
	CLPointerMsgDeserializer& operator=(const CLPointerMsgDeserializer&);
};

#endif