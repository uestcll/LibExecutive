#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "CLSharedMsgReceiverByNamedPipe.h"
#include "CLMessageDeserializer.h"
#include "CLLogger.h"

CLSharedMsgReceiverByNamedPipe::CLSharedMsgReceiverByNamedPipe(const char *pstrPipeName) : CLMessageQueueByNamedPipe(pstrPipeName)
{
}

CLSharedMsgReceiverByNamedPipe::~CLSharedMsgReceiverByNamedPipe()
{
	map<unsigned long, CLMessageDeserializer*>::iterator it;
	for(it = m_DeserializerTable.begin(); it != m_DeserializerTable.end(); it++)
		delete it->second;
}

CLStatus CLSharedMsgReceiverByNamedPipe::RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer)
{
	if(pDeserializer == 0)
		return CLStatus(-1, 0);

	map<unsigned long, CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(lMsgID);
	if(it != m_DeserializerTable.end())
	{
		delete pDeserializer;
		CLLogger::WriteLogMsg("In CLSharedMsgReceiverByNamedPipe::RegisterDeserializer(), m_DeserializerTable.find error", 0);
		return CLStatus(-1, 0);
	}

	m_DeserializerTable[lMsgID] = pDeserializer;

	return CLStatus(0, 0);
}

CLStatus CLSharedMsgReceiverByNamedPipe::ReadMsgFromPipe(int fd)
{
	while(1)
	{
		if(m_bNewMsg == true)
		{
			m_iHeadLength = sizeof(int);
			m_bReadHead = true;
			m_bNewMsg = false;
			m_iMsgLength = 0;
			m_pMsgBuffer = NULL;
			m_iReadLength = 0;
		}
		if(m_bReadHead == true)
		{
			int ret = read(fd, &m_iMsgLength, m_iHeadLength);
			if(ret != m_iHeadLength)
			{
				m_bNewMsg = true;
				CLLogger::WriteLogMsg("In CLSharedMsgReceiverByNamedPipe::ReadMsgFromPipe(), get head error", 0);
				return CLStatus(0, errno);
			}
			m_pMsgBuffer = new char[m_iMsgLength];
			m_bReadHead = false;
		}
		if(m_bReadHead == false)
		{
			int ret = read(fd, m_pMsgBuffer + m_iReadLength, (m_iMsgLength - m_iReadLength));
			if(ret <= 0)
			{
				return CLStatus(0, errno);
			}
			m_iReadLength += ret;
			if(m_iReadLength != m_iMsgLength)
			{
				return CLStatus(0, errno);
			}
			/*if(m_iReadLength == m_iMsgLength)
			{*/
			m_bNewMsg = true;
			unsigned long MsgID = *((unsigned long *)m_pMsgBuffer);
			map<unsigned long, CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(MsgID);
			if(it != m_DeserializerTable.end())
			{
				CLMessage *pMsg = it->second->Deserialize(m_pMsgBuffer);
				delete [] m_pMsgBuffer;
				m_MessageQueue.push(pMsg);	
			}
		}

	}
	return CLStatus(0, 0);
}
