#include "CLPointerMsgSerializer.h"
#include "CLIOVector.h"

CLPointerMsgSerializer::CLPointerMsgSerializer()
{

}

CLPointerMsgSerializer::~CLPointerMsgSerializer()
{

}

CLStatus CLPointerMsgSerializer::Serialize(CLMessage *pMsg, CLIOVector *pDataVec)
{
	char *buf = (char*)&pMsg;

	return (pDataVec->PushBack(buf, sizeof(pMsg)));
}
