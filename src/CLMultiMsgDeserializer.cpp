#include "CLMultiMsgDeserializer.h"
#include "CLLogger.h"
#include "CLIOVector.h"

CLMultiMsgDeserializer::CLMultiMsgDeserializer()
{

}

CLMultiMsgDeserializer::~CLMultiMsgDeserializer()
{

}

CLStatus CLMultiMsgDeserializer::Deserialize(CLIOVector dataVec, CLMessage **pMsg)
{
	const unsigned int msgIDLen = sizeof(unsigned long);

	if(dataVec.Length() < msgIDLen)
	{
		*pMsg = 0;
		CLLogger::WriteLogMsg("In CLMultiMsgDeserializer:: Deserialize(), length is not enough", 0);
		return CLStatus(-1, 0);
	}

	unsigned long MsgID;

	CLStatus s1 = dataVec.ReadData((char*)&MsgID, 0, msgIDLen);
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMultiMsgDeserializer:: Deserialize(), dataVec.ReadData() for msgID is error", 0);
		return s1;
	}

	map<unsigned long, CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(MsgID);

	if(it == m_DeserializerTable.end())
	{
		*pMsg = 0;

		CLLogger::WriteLogMsg("In CLMultiMsgDeserializer:: Deserialize(), do not find the msgID in map", 0);
		return CLStatus(-1, 0);
	}

	CLStatus s2 = it->second->Deserialize(dataVec, pMsg);
	if(!s2.IsSuccess())
	{
		*pMsg = 0;
		CLLogger::WriteLogMsg("In CLMultiMsgDeserializer:: Deserialize(), it->second->Deserialize() error", 0);
		return s2;
	}

	return CLStatus(0, 0);

}

CLStatus CLMultiMsgDeserializer::RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer)
{
	if(pDeserializer == 0)
	{
		CLLogger::WriteLogMsg("In CLMultiMsgDeserializer::RegisterDeserializer(), pDeserializer is 0", 0);
		return CLStatus(-1, 0);
	}

	map<unsigned long, CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(lMsgID);
	if(it != m_DeserializerTable.end())
	{
		CLLogger::WriteLogMsg("In CLMultiMsgDeserializer::RegisterDeserializer(), the msgID pDeserializer is exist", 0);
		return CLStatus(-1, 0);
	}

	m_DeserializerTable[lMsgID] = pDeserializer;

	return CLStatus(0, 0);
}

CLStatus CLMultiMsgDeserializer::UnRegisterDeserializer(unsigned long lMsgID)
{
	map<unsigned long, CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(lMsgID);

	if(it == m_DeserializerTable.end())
	{
		CLLogger::WriteLogMsg("In CLMultiMsgDeserializer::UnRegisterDeserializer(), the msgId Deserialize is not exist", 0);
		return CLStatus(-1, 0);
	}

	delete it->second;
	m_DeserializerTable.erase(it);

	return CLStatus(0, 0);
}
