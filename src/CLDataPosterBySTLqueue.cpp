#include "CLDataPosterBySTLqueue.h"
#include "CLDataReceiverBySTLqueue.h"
#include "CLIOVectors.h"
#include "CLLogger.h"

CLDataPosterBySTLqueue::CLDataPosterBySTLqueue(CLDataReceiverBySTLqueue *pDataReceiverBySTLqueue)
{
	if(pDataReceiverBySTLqueue == 0)
		throw "In CLDataPosterBySTLqueue::CLDataPosterBySTLqueue(), pDataReceiverBySTLqueue == 0";

	m_pDataReceiverBySTLqueue = pDataReceiverBySTLqueue;
}

CLDataPosterBySTLqueue::~CLDataPosterBySTLqueue()
{
	if(m_pDataReceiverBySTLqueue)
		delete m_pDataReceiverBySTLqueue;
}

CLStatus CLDataPosterBySTLqueue::Initialize()
{
	return CLStatus(0, 0);
}

CLStatus CLDataPosterBySTLqueue::Uninitialize()
{
	return CLStatus(0, 0);
}

//bug
CLStatus CLDataPosterBySTLqueue::PostData(CLIOVectors *pIOVectors)
{
	char *pBuffer = 0;
	size_t len_buffer = 0;
	
	CLStatus s = pIOVectors->PopFront(&pBuffer, &len_buffer);
	if(s.IsSuccess())
	{
		if(len_buffer == sizeof(unsigned long))
		{
			CLStatus s1 = m_pDataReceiverBySTLqueue->PushData(*((long *)pBuffer));
			if(s1.IsSuccess())
			{
				delete pIOVectors;
				return CLStatus(0, 0);
			}
		}
	}

	delete pIOVectors;
	return CLStatus(-1, POSTER_POST_ERROR);
}