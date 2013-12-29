#include "CLMultiMsgDeserializer.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"
#include "CLBufferManager.h"

using namespace std;

CLMultiMsgDeserializer::CLMultiMsgDeserializer()
{
}

CLMultiMsgDeserializer::~CLMultiMsgDeserializer()
{
	map<unsigned long, CLMessageDeserializer*>::iterator it;
	for(it = m_DeserializerTable.begin(); it != m_DeserializerTable.end(); it++)
		delete it->second;
}

CLStatus CLMultiMsgDeserializer::RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer)
{
	if(pDeserializer == 0)
		return CLStatus(-1, NORMAL_ERROR);

	map<unsigned long, CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(lMsgID);
	if(it != m_DeserializerTable.end())
	{
		delete pDeserializer;
		CLLogger::WriteLogMsg("In CLMultiMsgDeserializer::RegisterDeserializer(), m_DeserializerTable.find error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	m_DeserializerTable[lMsgID] = pDeserializer;

	return CLStatus(0, 0);
}

CLStatus CLMultiMsgDeserializer::UnregisterDeserializer(unsigned long lMsgID)
{
	map<unsigned long, CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(lMsgID);
	if(it != m_DeserializerTable.end())
	{
		delete it->second;
		m_DeserializerTable.erase(it);

		return CLStatus(0, 0);
	}
	else
		return CLStatus(-1, NORMAL_ERROR);
}

CLStatus CLMultiMsgDeserializer::Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager)
{
	const unsigned int HeadLength = sizeof(int);
	const unsigned int MsgIDLength = sizeof(long);

	if(IOVectors.Size() < HeadLength + MsgIDLength)
	{
		*ppMsg = 0;
		return CLStatus(-1, NORMAL_ERROR);
	}

	unsigned long MsgID;
	CLStatus s = IOVectors.ReadBlock(HeadLength, (char *)(&MsgID), sizeof(long));
	if(!s.IsSuccess() || (s.m_clReturnCode != sizeof(long)))
	{
		*ppMsg = 0;
		CLLogger::WriteLogMsg("In CLMultiMsgDeserializer::Deserialize(), IOVectors.ReadBlock error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	map<unsigned long, CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(MsgID);
	if(it != m_DeserializerTable.end())
	{
		CLStatus s = it->second->Deserialize(IOVectors, ppMsg, BufferManager);
		if(!s.IsSuccess())
		{
			*ppMsg = 0;
			CLLogger::WriteLogMsg("In CLMultiMsgDeserializer::Deserialize(), it->second->Deserialize error", 0);
			return CLStatus(-1, NORMAL_ERROR);
		}

		return CLStatus(0, 0);
	}
	else
	{
		*ppMsg = 0;
		CLLogger::WriteLogMsg("In CLMultiMsgDeserializer::Deserialize(), m_DeserializerTable.find error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}
}