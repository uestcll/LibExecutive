#include "CLSTLqueue.h"
#include "CLLogger.h"
#include "CLCriticalSection.h"
#include "CLIOVectors.h"

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

CLStatus CLSTLqueue::PopData(CLIOVectors& IOVectors, unsigned int index, unsigned int length)
{
	if((length % sizeof(unsigned long) != 0) || (length == 0))
		return CLStatus(-1, 0);

	try
	{
		CLCriticalSection cs(&m_Mutex);

		if(m_DataQueue.empty())
			return CLStatus(0, 0);

		for(int i = 0; i < length / sizeof(unsigned long); i++)
		{
			unsigned long data = PopOneData();

			int temp_index = index + sizeof(unsigned long) * i;

			CLStatus s = IOVectors.WriteLong(temp_index, data);
			if(!s.IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLSTLqueue::PopData(), IOVectors.WriteLong error", 0);
				return CLStatus(-1, 0);
			}

			if(m_DataQueue.empty())
				return CLStatus(temp_index + sizeof(unsigned long) - index, 0);
		}

		return CLStatus(length, 0);
	}
	catch(const char* str)
	{
		CLLogger::WriteLogMsg("In CLSTLqueue::PopData(), exception arise", 0);
		return CLStatus(-1, 0);
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
		return CLStatus(-1, 0);
	}
}