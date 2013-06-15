#include "CLSharedExecutiveCommunicationByNamedPipe.h"
#include "CLLogger.h"
#include "CLMessageSerializer.h"
#include "CLMessage.h"

CLSharedExecutiveCommunicationByNamedPipe::CLSharedExecutiveCommunicationByNamedPipe(const char *pstrExecutiveName) : CLExecutiveCommunicationByNamedPipe(pstrExecutiveName, true)
{
}

CLSharedExecutiveCommunicationByNamedPipe::~CLSharedExecutiveCommunicationByNamedPipe()
{
	map<unsigned long, CLMessageSerializer*>::iterator it;
	for(it = m_SerializerTable.begin(); it != m_SerializerTable.end(); it++)
		delete it->second;
}

CLStatus CLSharedExecutiveCommunicationByNamedPipe::RegisterSerializer(unsigned long lMsgID, CLMessageSerializer *pSerializer)
{
	if(pSerializer == 0)
		return CLStatus(-1, 0);

	map<unsigned long, CLMessageSerializer*>::iterator it = m_SerializerTable.find(lMsgID);
	if(it != m_SerializerTable.end())
	{
		delete pSerializer;
		CLLogger::WriteLogMsg("In CLSharedExecutiveCommunicationByNamedPipe::RegisterSerializer(), m_SerializerTable.find error", 0);
		return CLStatus(-1, 0);
	}

	m_SerializerTable[lMsgID] = pSerializer;

	return CLStatus(0, 0);
}

char *CLSharedExecutiveCommunicationByNamedPipe::GetMsgBuf(CLMessage *pMsg, unsigned int *pLength)
{
	map<unsigned long, CLMessageSerializer*>::iterator it = m_SerializerTable.find(pMsg->m_clMsgID);
	if(it == m_SerializerTable.end())
		return 0;

	unsigned int length = 0;
	char *pBuf = it->second->Serialize(pMsg, &length, sizeof(int));
	if(pBuf == 0)
		return 0;

	int *pHead = (int *)pBuf;
	*pHead = length - sizeof(int);

	*pLength = length;
	return pBuf;
}