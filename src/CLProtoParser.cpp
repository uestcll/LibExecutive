#include "CLProtoParser.h"
#include "CLBuffer.h"
#include "CLLogger.h"
#include "CLIOVector.h"

CLProtoParser::CLProtoParser()
{

}

CLProtoParser::~CLProtoParser()
{

}

CLStatus CLProtoParser::Decapsulate(CLBuffer* pBuffer, vector<CLIOVector *> vSerializedMsgs)
{	
	int decapsulateStartIndex = pBuffer->DataStartIndex();
	CLIOVector dataVec;
	CLStatus s = pBuffer->GetDataIOVecs(dataVec);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLProtoParser::Decapsulate(), GetDataIOVecs() error", 0);
		return s;
	}

	CLStatus s1 = DecapsulateMsg(dataVec, vSerializedMsgs);
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLProtoParser::Decapsulate(), DecapsulateMsg() error", 0);
		return s1;
	}
	// int decapsulateLen = (int)(s.m_clReturnCode);
	pBuffer->DataStartIndex(decapsulateStartIndex + s.m_clReturnCode);

	return CLStatus(0, 0);
}

