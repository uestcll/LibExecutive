#ifndef CLMessageDeserializer_H
#define CLMessageDeserializer_H

#include <vector>
#include "CLMessageReceiver.h"
#include "CLStatus.h"

class CLMessage;
class CLBuffer;

class CLMessageDeserializer
{
public:
	CLMessageDeserializer();
	virtual ~CLMessageDeserializer();

	virtual CLStatus Deserialize(CLBuffer *pBuffer, std::vector<SLSerializedMsgScope> sScope, std::queue<CLMessage*>) = 0;

private:
	CLMessageDeserializer(const CLMessageDeserializer&);
	CLMessageDeserializer& operator=(const CLMessageDeserializer&);
};

#endif