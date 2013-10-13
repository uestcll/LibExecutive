#include "CLProtoParserForSharedPipe.h"

CLProtoParserForSharedPipe::CLProtoParserForSharedPipe()
{

}

CLProtoParserForSharedPipe::~CLProtoParserForSharedPipe()
{

}

CLStatus CLProtoParserForSharedPipe::DecapsulateMsg(CLIOVector& dataVec, vector<CLIOVector *> vSerializedMsgs)
{
	int startIndex = 0;
	int ctxLen = 0;
	int headLen = sizeof(int);
	int wholeLen = dataVec.Length(0;)

	CLIOVector restDataVec = dataVec;

	while(1)
	{
		if(restDataVec.Length() < headLen)
			break;
		else
		{
			restDataVec.ReadData((char*)&ctxLen, startIndex, headLen);
			if(ctxLen == 0)
				continue;
			if(restDataVec.Length() < (headLen + ctxLen))
				break;
			else
			{
				CLIOVector *pOneSerializedMsg = new CLIOVector();
				startIndex += headLen;
				restDataVec.GetIOVec(startIndex, ctxLen, *pOneSerializedMsg);				
				startIndex += ctxLen;
				restDataVec.PopAll();
				dataVec.GetIOVec(startIndex, wholeLen - startIndex, restDataVec);
			}
		}
	}
	return CLStatus(startIndex, 0);
}

