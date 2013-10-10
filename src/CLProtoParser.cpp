#include "CLProtoParser.h"

CLProtoParser::CLProtoParser()
{

}

CLProtoParser::~CLProtoParser()
{

}

CLStatus CLProtoParser::Decapsulate(CLBuffer* pBuffer, vector<CLIOVector *> vSerializedMsgs)
{	
	int decapsulateStartIndex = m_pDataBuffer->DataStartIndex();

	CLStatus s = DecapsulateMsg(pBuffer, decapsulateStartIndex, vSerializedMsgs);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLProtoParser::Decapsulate(), DecapsulateMsg() error", 0);
		return s;
	}
	// int decapsulateLen = (int)(s.m_clReturnCode);
	m_pDataBuffer->DataStartIndex(decapsulateStartIndex + s.m_clReturnCode);

	return CLStatus(0, 0);
}

