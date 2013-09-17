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

			//CLIOVectors should do some optimization about write short/int/long to IOVectors or read from
			char *pdata = (char *)(&data);
			for(int j = 0; j < sizeof(unsigned long); j++)
				IOVectors[index + i * sizeof(unsigned long) + j] = pdata[j];

			if(m_DataQueue.empty())
				return CLStatus((i + 1) * sizeof(unsigned long), 0);
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