#include "CLDataReceiverBySTLqueue.h"
#include "CLIOVectors.h"
#include "CLSTLqueue.h"
#include "CLLogger.h"

using namespace std;

#define READ_BUFFER_LEN_FOR_STL_QUEUE 4096

CLDataReceiverBySTLqueue::CLDataReceiverBySTLqueue(CLSTLqueue *pSTLqueue)
{
	if(pSTLqueue == 0)
		throw "In CLDataReceiverBySTLqueue::CLDataReceiverBySTLqueue(), pSTLqueue == 0";

	m_pSTLqueue = pSTLqueue;
}

CLDataReceiverBySTLqueue::~CLDataReceiverBySTLqueue()
{
}

CLStatus CLDataReceiverBySTLqueue::GetData(CLIOVectors *pIOVectors, void **ppContext)
{
	char *p = new char[READ_BUFFER_LEN_FOR_STL_QUEUE];

	try
	{
		*ppContext = 0;
		
		CLIOVectors iovectors;
		CLStatus s1 = iovectors.PushBack(p, READ_BUFFER_LEN_FOR_STL_QUEUE);
		if(!s1.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLDataReceiverBySTLqueue::GetData(), iovectors.PushBack error", 0);
			throw CLStatus(-1, RECEIVED_ERROR);
		}

		CLStatus s2 = m_pSTLqueue->PopData(iovectors, 0, READ_BUFFER_LEN_FOR_STL_QUEUE);
		if(s2.m_clReturnCode == 0)
			throw CLStatus(-1, RECEIVED_ZERO_BYTE);

		if(!s2.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLDataReceiverBySTLqueue::GetData(), m_pSTLqueue->PopData error", 0);
			throw CLStatus(-1, RECEIVED_ERROR);
		}

		CLStatus s3 = pIOVectors->PushBack(p, s2.m_clReturnCode);
		if(!s3.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLDataReceiverBySTLqueue::GetData(), pIOVectors->PushBack error", 0);
			throw CLStatus(-1, RECEIVED_ERROR);
		}
		
		return CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		delete [] p;
		return s;
	}
}