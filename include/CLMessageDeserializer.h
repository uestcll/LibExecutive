#ifndef CLMessageDeserializer_H
#define CLMessageDeserializer_H

#include "CLStatus.h"

class CLMessage;
class CLIOVectors;

class CLMessageDeserializer
{
public:
	CLMessageDeserializer();
	virtual ~CLMessageDeserializer();

	virtual CLStatus Deserialize(CLIOVectors* pIOVectors, CLMessage **ppMsg) = 0;

private:
	CLMessageDeserializer(const CLMessageDeserializer&);
	CLMessageDeserializer& operator=(const CLMessageDeserializer&);
};

#endif