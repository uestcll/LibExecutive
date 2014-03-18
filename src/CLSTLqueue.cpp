#include "CLSTLqueue.h"
#include "CLLogger.h"
#include "CLCriticalSection.h"
#include "CLIOVectors.h"
#include "ErrorCode.h"
#include "CLIteratorForIOVectors.h"

CLSTLqueue::CLSTLqueue()
{
}

CLSTLqueue::~CLSTLqueue()
{
}

CLUuid CLSTLqueue::GetUuid()
{
	return m_UuidOfSTLqueue;
}

unsigned long CLSTLqueue::PopOneData()
{
	unsigned long l = m_DataQueue.front();
	m_DataQueue.pop();
	return l;
}

CLStatus CLSTLqueue::PopData(CLIOVectors& IOVectors)
{
	unsigned int length = IOVectors.Size();
	if(length < sizeof(unsigned long))
		return CLStatus(-1, NORMAL_ERROR);

	try
	{
		CLCriticalSection cs(&m_Mutex);

		if(m_DataQueue.empty())
			return CLStatus(-1, RECEIVED_ZERO_BYTE);

		unsigned int BytesRead = 0;

		CLIteratorForIOVectors iter;
		IOVectors.GetIterator(0, iter);

		while(true)
		{
			unsigned long data = PopOneData();
			CLStatus s = IOVectors.WriteBlock(iter, (char *)(&data), sizeof(unsigned long));
			if((!s.IsSuccess()) || (s.m_clReturnCode != sizeof(unsigned long)))
			{
				CLLogger::WriteLogMsg("In CLSTLqueue::PopData(), IOVectors.WriteBlock error", 0);
				return CLStatus(-1, NORMAL_ERROR);
			}

			BytesRead += sizeof(unsigned long);

			if(((length - BytesRead) < sizeof(unsigned long)) || m_DataQueue.empty())
				return CLStatus(BytesRead, 0);
		}
	}
	catch(const char* str)
	{
		CLLogger::WriteLogMsg("In CLSTLqueue::PopData(), exception arise", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}
}

CLStatus CLSTLqueue::PushData(unsigned long ulData)
{
	try
	{
		CLCriticalSection cs(&m_Mutex);

		m_DataQueue.push(ulData);

		return CLStatus(0, 0);
	}
	catch(const char* str)
	{
		CLLogger::WriteLogMsg("In CLSTLqueue::PushData(), exception arise", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}
}