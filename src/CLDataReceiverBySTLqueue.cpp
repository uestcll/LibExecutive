#include "CLDataReceiverBySTLqueue.h"
#include "CLLogger.h"
#include "CLCriticalSection.h"

using namespace std;

CLDataReceiverBySTLqueue::CLDataReceiverBySTLqueue()
{
}

CLDataReceiverBySTLqueue::~CLDataReceiverBySTLqueue()
{
}

CLStatus CLDataReceiverBySTLqueue::GetData(CLIOVectors *pIOVectors, void **ppContext)
{
	try
	{
		CLCriticalSection cs(&m_Mutex);

		*ppContext = 0;

		if(m_DataQueue.empty())
		{
			CLLogger::WriteLogMsg("In CLDataReceiverBySTLqueue::GetData(), m_DataQueue.empty error", 0);
			return CLStatus(-1, RECEIVED_ZERO_BYTE);
		}

		unsigned long *p = new unsigned long[1];
		*p = m_DataQueue.front();
		m_DataQueue.pop();

		CLStatus s = pIOVectors->PushBack(p, sizeof(unsigned long));
		if(!s.IsSuccess())
		{
			delete [] p;
			CLLogger::WriteLogMsg("In CLDataReceiverBySTLqueue::GetData(), pIOVectors->PushBack error", 0);
			return CLStatus(-1, RECEIVED_ERROR);
		}

		return CLStatus(0, 0);
	}
	catch(const char* str)
	{
		CLLogger::WriteLogMsg("In CLDataReceiverBySTLqueue::GetData(), exception arise", 0);
		return CLStatus(-1, RECEIVED_ERROR);
	}
}

CLStatus CLDataReceiverBySTLqueue::PushData(unsigned long ulData)
{
	try
	{
		CLCriticalSection cs(&m_Mutex);

		m_DataQueue.push(ulData);

		return CLStatus(0, 0);
	}
	catch(const char* str)
	{
		CLLogger::WriteLogMsg("In CLDataReceiverBySTLqueue::PushData(), exception arise", 0);
		return CLStatus(-1, 0);
	}
}