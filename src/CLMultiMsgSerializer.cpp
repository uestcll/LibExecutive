#include "CLMultiMsgSerializer.h"
#include "CLMessage.h"
#include "CLLogger.h"
#include "ErrorCode.h"

using namespace std;

CLMultiMsgSerializer::CLMultiMsgSerializer()
{
}

CLMultiMsgSerializer::~CLMultiMsgSerializer()
{
	map<unsigned long, CLMessageSerializer*>::iterator it;
	for(it = m_SerializerTable.begin(); it != m_SerializerTable.end(); it++)
		delete it->second;
}

CLStatus CLMultiMsgSerializer::RegisterSerializer(unsigned long lMsgID, CLMessageSerializer *pSerializer)
{
	if(pSerializer == 0)
		return CLStatus(-1, NORMAL_ERROR);

	map<unsigned long, CLMessageSerializer*>::iterator it = m_SerializerTable.find(lMsgID);
	if(it != m_SerializerTable.end())
	{
		delete pSerializer;
		return CLStatus(-1, NORMAL_ERROR);
	}

	m_SerializerTable[lMsgID] = pSerializer;

	return CLStatus(0, 0);
}

CLStatus CLMultiMsgSerializer::UnregisterSerializer(unsigned long lMsgID)
{
	map<unsigned long, CLMessageSerializer*>::iterator it = m_SerializerTable.find(lMsgID);
	if(it == m_SerializerTable.end())
		return CLStatus(-1, NORMAL_ERROR);

	delete it->second;
	m_SerializerTable.erase(it);

	return CLStatus(0, 0);
}

CLStatus CLMultiMsgSerializer::Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
{
	if((pMsg == 0) || (pIOVectors == 0))
		return CLStatus(-1, NORMAL_ERROR);

	map<unsigned long, CLMessageSerializer*>::iterator it = m_SerializerTable.find(pMsg->m_clMsgID);
	if(it == m_SerializerTable.end())
	{
		CLLogger::WriteLogMsg("In CLMultiMsgSerializer::Serialize(), m_SerializerTable.find error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	return it->second->Serialize(pMsg, pIOVectors);
}