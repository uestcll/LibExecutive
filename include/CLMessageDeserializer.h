#ifndef CLMessageDeserializer_H
#define CLMessageDeserializer_H

#include "CLStatus.h"

class CLMessage;

class CLMessageDeserializer
{
public:
	CLMessageDeserializer();
	explicit CLMessageDeserializer(CLMessageDeserializer *pDeserializer);
	virtual ~CLMessageDeserializer();

	virtual CLStatus Deserialize(char *pBuffer, CLMessage **ppMsg) = 0;

private:
	CLMessageDeserializer(const CLMessageDeserializer&);
	CLMessageDeserializer& operator=(const CLMessageDeserializer&);

private:
	CLMessageDeserializer *m_pNextDeserializer;
};

#endif