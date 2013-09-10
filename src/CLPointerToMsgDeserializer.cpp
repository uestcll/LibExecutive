#include "CLPointerToMsgDeserializer.h"
#include "CLIOVectors.h"
#include "CLMessage.h"
#include "CLLogger.h"

CLPointerToMsgDeserializer::CLPointerToMsgDeserializer()
{
}

CLPointerToMsgDeserializer::~CLPointerToMsgDeserializer()
{
}

CLStatus CLPointerToMsgDeserializer::Deserialize(CLIOVectors *pIOVectors, unsigned int Index, unsigned int Length, CLMessage **ppMsg)
{
	if(Length != sizeof(CLMessage *))
	{
		CLLogger::WriteLogMsg("In CLPointerToMsgDeserializer::Deserialize(), Length error", 0);
		return CLStatus(-1, 0);
	}

	char *p = (char *)(*(ppMsg));
	for(int i = 0; i < sizeof(CLMessage *); i++)
		p[i] = (*pIOVectors)[i + Index];

	return CLStatus(0, 0);
}