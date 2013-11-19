#include "CLMultiMsgSerializer.h"
#include "CLIOVector.h"
#include "CLLogger.h"
#include "CLMessage.h"

using namespace std;

CLMultiMsgSerializer::CLMultiMsgSerializer()
{

}

CLMultiMsgSerializer::~CLMultiMsgSerializer()
{

}

CLStatus CLMultiMsgSerializer::Serialize(CLMessage *pMsg, CLIOVector *pDataVec)
{
	unsigned long msgID = pMsg->m_clMsgID;
	map<unsigned long, CLMessageSerializer*>::iterator it = m_SerializerTable.find(msgID);

	if(it == m_SerializerTable.end())
	{
		CLLogger::WriteLogMsg("In CLMultiMsgSerializer::Serialize(), m_SerializerTable.find() error", 0);
		return CLStatus(-1, 0);
	}

	CLStatus s = it->second->Serialize(pMsg, pDataVec);
	if(!s.IsSuccess())
	{	
		CLLogger::WriteLogMsg("In CLMultiMsgSerializer::Serialize(), it->second->Serialize() error", 0);
		return s;
	}

	return CLStatus(0, 0);
}

CLStatus CLMultiMsgSerializer::RegisterSerializer(unsigned long lMsgID, CLMessageSerializer *pSerializer)
{
	if(!pSerializer)
	{
		CLLogger::WriteLogMsg("In CLMultiMsgSerializer::RegisterSerializer(), parameter pSerializer is error", 0);
		return CLStatus(-1, 0);
	}
	map<unsigned long, CLMessageSerializer*>::iterator it = m_SerializerTable.find(lMsgID);

	if(it != m_SerializerTable.end())
	{
		CLLogger::WriteLogMsg("In CLMultiMsgSerializer::RegisterSerializer(), it != m_SerializerTable.end()", 0);
		return CLStatus(-1, 0);
	}
	m_SerializerTable[lMsgID] = pSerializer;

	return CLStatus(0, 0);
}

CLStatus CLMultiMsgSerializer::UnRegisterSerializer(unsigned long lMsgID)
{
	map<unsigned long, CLMessageSerializer*>::iterator it = m_SerializerTable.find(lMsgID);

	if(it == m_SerializerTable.end())
	{
		CLLogger::WriteLogMsg("In CLMultiMsgSerializer::RegisterSerializer(), it == m_SerializerTable.end(), cannot find the Serializer", 0);
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}