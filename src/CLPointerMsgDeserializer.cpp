#include "CLPointerMsgDeserializer.h"
#include "CLIOVector.h"

CLPointerMsgDeserializer::CLPointerMsgDeserializer()
{

}

CLPointerMsgDeserializer::~CLPointerMsgDeserializer()
{

}

CLStatus CLPointerMsgDeserializer::Deserialize(CLIOVector dataVec, CLMessage **pMsg)
{
	char *pBuf;
	int len = sizeof(CLMessage*);
	dataVec.ReadData(pBuf, 0, len);

	*pMsg = (CLMessage *)(*pBuf);

	return CLStatus(0, 0);
}