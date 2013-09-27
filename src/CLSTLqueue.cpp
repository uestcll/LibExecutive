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
			return CLStatus(-1, NORMAL_ERROR);

		//.................
		for(; iter != )
		{
			if((length - BytesRead) < sizeof(unsigned long))
				return CLStatus(BytesRead, 0);

			unsigned long data = PopOneData();
		}



		for(int i = 0; i < length / sizeof(unsigned long); i++)
		{
			unsigned long data = PopOneData();

			int temp_index = index + sizeof(unsigned long) * i;

			CLStatus s = IOVectors.WriteLong(temp_index, data);
			if(!s.IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLSTLqueue::PopData(), IOVectors.WriteLong error", 0);
				return CLStatus(-1, RECEIVED_ERROR);
			}

			if(m_DataQueue.empty())
				return CLStatus(temp_index + sizeof(unsigned long) - index, 0);
		}

		return CLStatus(length, 0);
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