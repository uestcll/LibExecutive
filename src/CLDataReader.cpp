#include "CLDataReader.h"
#include "CLProtoParser.h"

CLDataReader::CLDataReader(int fd, CLProtoParser *pProtoParser, CLMessageReceiver pMsgReceiver)) : m_fd(fd), m_pProtoParser(pProtoParser), m_pMsgReceiver(pMsgReceiver)
{

}

CLDataReader::~CLDataReader()
{

}

CLStatus CLDataReader::ReadData()
{
	while(1)
	{
		if(m_bNewData == true)
		{
			m_iHeadLength = m_pProtoParser->GetHeaderLength();
			m_bReadHead = true;
			m_bNewData = false;
			m_iCtxLength = 0;
			m_pHeaderBuffer = new char[m_iHeadLength];
			m_pCtxBuffer = NULL;
			m_iReadOffset = 0;
		}
		if(m_bReadHead == true)
		{
			CLStatus s1 = ReadHeader(m_fd);
			if(!s1.IsSuccess())
			{
				if(s1.m_clReturnCode == READ_ERROR)
				{
					if((s1.m_clErrorCode != EWOULDBLOCK) && (s1.m_clErrorCode != EAGAIN) && (s1.m_clErrorCode != EINTR))
					{
						CLLogger::WriteLogMsg("In CLDataReader::ReadData, read header error", s1.m_clErrorCode);
					}
				}
				else
				{
					CLLogger::WriteLogMsg("In CLDataReader::ReadData, read header end", 0);
				}
				return s1;
			}
			
		}
		if((m_bNewData == false) && (m_bReadHead == false))
		{
			CLStatus s2 = ReadContext(m_fd);
			if(!s2.IsSuccess())
			{
				if(s2.m_clReturnCode == READ_ERROR)
				{
					if((s2.m_clErrorCode != EWOULDBLOCK) && (s2.m_clErrorCode != EAGAIN) && (s2.m_clErrorCode != EINTR))
					{
						CLLogger::WriteLogMsg("In CLDataReader::ReadData, read context error", s2.m_clErrorCode);
					}
				}
				else
				{
					CLLogger::WriteLogMsg("In CLDataReader::ReadData, read context end", 0);
				}
				return s2;
			}
			
		}

	}
	return CLStatus(0, 0);
}


CLStatus CLDataReader::ReadHeader(int fd)
{
	int ret = read(fd, m_pHeaderBuffer + m_iReadOffset, m_iHeadLength - m_iReadOffset;
	if(ret == 0)
	{
		return CLStatus(READ_END, errno);//read end 
	}
	else if(ret < 0)
	{
		//errno is ewouldblock is read finish
		return CLStatus(READ_ERROR, errno);//read error
	}
	m_iReadOffset += ret;
	if(m_iReadOffset == m_iHeadLength)
	{
		m_iReadOffset = 0;
		m_bReadHead = false;
		m_iCtxLength = m_pProtoParser->GetContextLength(m_pHeaderBuffer, m_iHeadLength);

		if(m_iCtxLength == 0)
		{
			m_bNewData = true;
			m_pMsgReceiver->EncapsulateMesage(m_pHeaderBuffer, m_iHeadLength, NULL, 0);
			/// Get Message

		}
		else if(m_iCtxLength > 0)
		{
			m_pCtxBuffer = new char[m_iCtxLength];
		}
		else
		{
			m_bNewData = true;
		}
	}

	return CLStatus(0, 0);
}

CLStatus CLDataReader::ReadContext(int fd)
{
	int ret = read(fd, m_pCtxBuffer + m_iReadOffset, (m_iCtxLength - m_iReadOffset));
	if(ret == 0)
	{
		return CLStatus(READ_END, errno);//read end 
	}
	else if(ret < 0)
	{
		//errno is ewouldblock is read finish
		return CLStatus(READ_ERROR, errno);//read error
	}
	m_iReadOffset += ret;
	if(m_iReadOffset == m_iCtxLength)
	{
		m_bNewData = true;
		m_iReadOffset = 0;

		m_pMsgReceiver->EncapsulateMesage(m_pHeaderBuffer, m_iHeadLength, m_pCtxBuffer, m_iCtxLength);
	}

	return CLStatus(0, 0);
}