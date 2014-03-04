#ifndef CLMultiMsgDeserializer_H
#define CLMultiMsgDeserializer_H

#include <map>
#include "CLMessageDeserializer.h"

class CLMultiMsgDeserializer : public CLMessageDeserializer
{
public:
	CLMultiMsgDeserializer();
	virtual ~CLMultiMsgDeserializer();

	CLStatus RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer);
	CLStatus UnregisterDeserializer(unsigned long lMsgID);

	virtual CLStatus Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *pContext);

private:
	CLMultiMsgDeserializer(const CLMultiMsgDeserializer&);
	CLMultiMsgDeserializer& operator=(const CLMultiMsgDeserializer&);

private:
	std::map<unsigned long, CLMessageDeserializer*> m_DeserializerTable;
};

#endif