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

	while(1)
	{
		if(startIndex + headLen >= wholeLen)
			break;
		dataVec.ReadData((char*)&ctxLen, startIndex, headLen);
		
		if(0 == ctxLen)
		{
			startIndex += headLen;
			continue;
		}
		if(startIndex + headLen + ctxLen > wholeLen)
		{
			break;
		}

		CLIOVector *pOneSerializedMsg = new CLIOVector();
		startIndex += headLen;
		dataVec.GetIOVecs(startIndex, ctxLen, *pOneSerializedMsg);

		vSerializedMsgs.push_back(pOneSerializedMsg);				
		startIndex += ctxLen;
	
	}
	
	return CLStatus(startIndex, 0);
}

