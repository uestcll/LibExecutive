#include "CLMultiDeserializer.h"
#include "CLLogger.h"

using namespace std;

CLMultiDeserializer::CLMultiDeserializer()
{
}

CLMultiDeserializer::~CLMultiDeserializer()
{
	map<unsigned long, CLMessageDeserializer*>::iterator it;
	for(it = m_DeserializerTable.begin(); it != m_DeserializerTable.end(); it++)
		delete it->second;
}

CLStatus CLMultiDeserializer::RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer)
{
	if(pDeserializer == 0)
		return CLStatus(-1, 0);

	map<unsigned long, CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(lMsgID);
	if(it != m_DeserializerTable.end())
	{
		delete pDeserializer;
		CLLogger::WriteLogMsg("In CLMultiDeserializer::RegisterDeserializer(), m_DeserializerTable.find error", 0);
		return CLStatus(-1, 0);
	}

	m_DeserializerTable[lMsgID] = pDeserializer;

	return CLStatus(0, 0);
}

CLStatus CLMultiDeserializer::UnregisterDeserializer(unsigned long lMsgID)
{
	map<unsigned long, CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(lMsgID);
	if(it != m_DeserializerTable.end())
	{
		delete it->second;
		m_DeserializerTable.erase(it);
	}
}

CLStatus CLMultiDeserializer::Deserialize(CLIOVectors *pIOVectors, unsigned int Index, unsigned int Length, CLMessage **ppMsg)
{
	const int HeadLength = sizeof(int);
	const int MsgIDLength = sizeof(long);

	if(Length < HeadLength + MsgIDLength)
	{
		CLLogger::WriteLogMsg("In CLMultiDeserializer::Deserialize(), Length error", 0);
		return CLStatus(-1, 0);
	}

	//²¹³ä´¦ÀíÍøÂçË³Ðò
	char pID[sizeof(long)];
	for(int i = 0; i < MsgIDLength; i++)
		pID[i] = (*pIOVectors)[Index + HeadLength + i];

	unsigned long MsgID = *((unsigned long *)pID);

	map<unsigned long, CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(MsgID);
	if(it != m_DeserializerTable.end())
	{
		CLStatus s = it->second->Deserialize(pIOVectors, Index + HeadLength, Length - HeadLength, ppMsg);
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMultiDeserializer::Deserialize(), it->second->Deserialize error", 0);
			return s;
		}

		return CLStatus(0, 0);
	}
	else
	{
		CLLogger::WriteLogMsg("In CLMultiDeserializer::Deserialize(), m_DeserializerTable.find error", 0);
		return CLStatus(-1, 0);
	}
}