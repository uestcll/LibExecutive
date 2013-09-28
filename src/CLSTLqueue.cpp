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
	{
		CLLogger::WriteLogMsg("In CLSTLqueue::PopData(), IOVectors.Size error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	try
	{
		CLCriticalSection cs(&m_Mutex);

		if(m_DataQueue.empty())
			return CLStatus(-1, RECEIVED_ZERO_BYTE);

		unsigned int BytesRead = 0;

		CLIteratorForIOVectors iter;
		CLStatus s1 = IOVectors.GetIterator(0, &iter);
		if(!s1.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLSTLqueue::PopData(), IOVectors.GetIterator error", 0);
			return CLStatus(-1, NORMAL_ERROR);
		}

		while(true)
		{
			unsigned long data = PopOneData();
			CLStatus s2 = IOVectors.WriteBlock(iter, (char *)(&data), sizeof(unsigned long));
			if(!s2.IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLSTLqueue::PopData(), IOVectors.WriteBlock error", 0);
				if(BytesRead == 0)
					return CLStatus(-1, NORMAL_ERROR);
				else
					return CLStatus(BytesRead, 0);
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
	//..............
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