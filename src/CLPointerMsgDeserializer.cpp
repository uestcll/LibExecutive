#include "CLPointerMsgDeserializer.h"
#include "CLIOVector.h"

CLPointerMsgDeserializer::CLPointerMsgDeserializer()
{

}

CLPointerMsgDeserializer::~CLPointerMsgDeserializer()
{

}

CLStatus CLPointerMsgDeserializer::Deserialize(CLIOVector &dataVec, CLMessage **pMsg)
{
	int len = sizeof(CLMessage*);
	dataVec.ReadData((char*)pMsg, 0, len);


	return CLStatus(0, 0);
}