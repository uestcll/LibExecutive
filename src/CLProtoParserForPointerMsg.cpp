#include "CLProtoParserForPointerMsg.h"
#include "CLLogger.h"
#include "CLMessage.h"
#include "CLIOVector.h"

CLProtoParserForPointerMsg::CLProtoParserForPointerMsg()
{

}

CLProtoParserForPointerMsg::~CLProtoParserForPointerMsg()
{


}

CLStatus CLProtoParserForPointerMsg::DecapsulateMsg(CLIOVector& dataVec, vector<CLIOVector *> &vSerializedMsgs)
{
	int startIndex = 0;
	int ctxLen = sizeof(CLMessage *);
	int wholeLen = dataVec.Length();

	while(1)
	{
		if(startIndex + ctxLen > wholeLen)
			break;
		CLIOVector *pOneSerializedMsg = new CLIOVector();
		dataVec.GetIOVecs(startIndex, ctxLen, *pOneSerializedMsg);
		vSerializedMsgs.push_back(pOneSerializedMsg);
		startIndex += ctxLen;
	}

	// CLIOVector restDataVec;

	// dataVec.GetIOVecs(startIndex, wholeLen - startIndex, restDataVec);

	// while(1)
	// {
	// 	if(restDataVec.Length() < ctxLen)
	// 		break;
	// 	else
	// 	{
	// 		CLIOVector *pOneSerializedMsg = new CLIOVector();
	// 		restDataVec.GetIOVecs(startIndex, ctxLen, *pOneSerializedMsg);
			
	// 		vSerializedMsgs.push_back(pOneSerializedMsg);				
	// 		startIndex += ctxLen;
	// 		restDataVec.PopAll();
	// 		dataVec.GetIOVecs(startIndex, wholeLen - startIndex, restDataVec);
	// 	}
	// }
	return CLStatus(startIndex, 0);
}