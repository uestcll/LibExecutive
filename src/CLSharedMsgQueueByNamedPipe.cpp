#include <fcntl.h>
#include <errno.h>
#include "CLSharedMsgQueueByNamedPipe.h"
#include "CLMessageDeserializer.h"
#include "CLLogger.h"

CLSharedMsgQueueByNamedPipe::CLSharedMsgQueueByNamedPipe(const char *pstrPipeName) : CLMessageQueueByNamedPipe(pstrPipeName)
{
}

CLSharedMsgQueueByNamedPipe::~CLSharedMsgQueueByNamedPipe()
{
	map<unsigned long, CLMessageDeserializer*>::iterator it;
	for(it = m_DeserializerTable.begin(); it != m_DeserializerTable.end(); it++)
		delete it->second;
}

CLStatus CLSharedMsgQueueByNamedPipe::RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer)
{
	if(pDeserializer == 0)
		return CLStatus(-1, 0);

	map<unsigned long, CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(lMsgID);
	if(it != m_DeserializerTable.end())
	{
		delete pDeserializer;
		CLLogger::WriteLogMsg("In CLMessageQueueByNamedPipe::RegisterDeserializer(), m_DeserializerTable.find error", 0);
		return CLStatus(-1, 0);
	}

	m_DeserializerTable[lMsgID] = pDeserializer;

	return CLStatus(0, 0);
}

CLMessage *CLSharedMsgQueueByNamedPipe::ReadMsgFromPipe(int fd)
{
	int length = 0;
	if(read(fd, &length, sizeof(int)) != sizeof(int))
	{
		CLLogger::WriteLogMsg("In CLSharedMsgQueueByNamedPipe::ReadMsgFromPipe(), read error", errno);
		return 0;
	}

	char *pBuffer = new char[length];
	if(read(fd, pBuffer, length) != length)
	{
		CLLogger::WriteLogMsg("In CLSharedMsgQueueByNamedPipe::ReadMsgFromPipe(), read2 error", errno);
		delete [] pBuffer;
		return 0;
	}

	unsigned long MsgID = *((unsigned long *)pBuffer);
	map<unsigned long, CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(MsgID);
	if(it != m_DeserializerTable.end())
	{
		CLMessage *pMsg = it->second->Deserialize(pBuffer);
		delete [] pBuffer;
		return pMsg;
	}

	delete [] pBuffer;
	return NULL;
}