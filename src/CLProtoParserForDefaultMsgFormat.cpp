#include "CLProtoParserForDefaultMsgFormat.h"
#include "CLIOVector.h"

CLProtoParserForDefaultMsgFormat::CLProtoParserForDefaultMsgFormat()
{

}

CLProtoParserForDefaultMsgFormat::~CLProtoParserForDefaultMsgFormat()
{

}

CLStatus CLProtoParserForDefaultMsgFormat::DecapsulateMsg(CLIOVector& dataVec, vector<CLIOVector *> &vSerializedMsgs)
{
	int startIndex = 0;
	int ctxLen = 0;
	int headLen = sizeof(int);
	int wholeLen = dataVec.Length();

	CLIOVector restDataVec;

	dataVec.GetIOVecs(startIndex, wholeLen - startIndex, restDataVec);

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
				restDataVec.GetIOVecs(startIndex, ctxLen, *pOneSerializedMsg);

				vSerializedMsgs.push_back(pOneSerializedMsg);				
				startIndex += ctxLen;
				restDataVec.PopAll();
				dataVec.GetIOVecs(startIndex, wholeLen - startIndex, restDataVec);
			}
		}
	}
	return CLStatus(startIndex, 0);
}

