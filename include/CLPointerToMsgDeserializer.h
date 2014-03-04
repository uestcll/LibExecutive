#ifndef CLPointerToMsgDeserializer_H
#define CLPointerToMsgDeserializer_H

#include "CLMessageDeserializer.h"

class CLPointerToMsgDeserializer : public CLMessageDeserializer
{
public:
	CLPointerToMsgDeserializer();
	virtual ~CLPointerToMsgDeserializer();

	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *pContext);

private:
	CLPointerToMsgDeserializer(const CLPointerToMsgDeserializer&);
	CLPointerToMsgDeserializer& operator=(const CLPointerToMsgDeserializer&);
};

#endif