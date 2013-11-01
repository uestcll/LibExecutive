#ifndef CLPOINTERMSGSERIALIZER_H
#define CLPOINTERMSGSERIALIZER_H

#include "CLStatus.h"
#include "CLMessageSerializer.h"

class CLPointerMsgSerializer : public CLMessageSerializer
{
public:
	CLPointerMsgSerializer();
	virtual ~CLPointerMsgSerializer();

	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVector *pDataVec);

private:
	CLPointerMsgSerializer(const CLPointerMsgSerializer&);
	CLPointerMsgSerializer& operator=(const CLPointerMsgSerializer&);
};

#endif