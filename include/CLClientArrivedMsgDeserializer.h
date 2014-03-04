#ifndef CLClientArrivedMsgDeserializer_H
#define CLClientArrivedMsgDeserializer_H

#include "CLMessageDeserializer.h"

class CLClientArrivedMsgDeserializer : public CLMessageDeserializer
{
public:
	CLClientArrivedMsgDeserializer();
	virtual ~CLClientArrivedMsgDeserializer();

	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *pContext);

private:
	CLClientArrivedMsgDeserializer(const CLClientArrivedMsgDeserializer&);
	CLClientArrivedMsgDeserializer& operator=(const CLClientArrivedMsgDeserializer&);
};

#endif