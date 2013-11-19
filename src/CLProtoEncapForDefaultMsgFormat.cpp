#include "CLProtoEncapForDefaultMsgFormat.h"
#include "CLLogger.h"
#include "CLIOVector.h"

CLProtoEncapForDefaultMsgFormat::CLProtoEncapForDefaultMsgFormat()
{

}

CLProtoEncapForDefaultMsgFormat::~CLProtoEncapForDefaultMsgFormat()
{

}

CLStatus CLProtoEncapForDefaultMsgFormat::Encapsulate(CLIOVector *pDataVec)
{
	int bufLen = pDataVec->Length();
	int headLen = sizeof(int);
	char *pHeadBuffer = new char[headLen];

	int *pHead = (int *)pHeadBuffer;
	*pHead = bufLen;

	return (pDataVec->PushFront(pHeadBuffer, headLen));
	// return CLStatus(0, 0);
}