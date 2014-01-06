#include "CLDataPosterBySTLqueue.h"
#include "CLSTLqueue.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLDataPosterBySTLqueue::CLDataPosterBySTLqueue(CLSTLqueue *pSTLqueue)
{
	m_pSTLqueue = pSTLqueue;
}

CLDataPosterBySTLqueue::~CLDataPosterBySTLqueue()
{
}

CLStatus CLDataPosterBySTLqueue::PostData(CLIOVectors *pIOVectors)
{
	if((pIOVectors == 0) || (pIOVectors->Size() < sizeof(unsigned long)))
		return CLStatus(-1, DATA_POSTER_POST_ERROR);

	unsigned long data = 0;
	CLStatus s = pIOVectors->ReadBlock(0, (char *)(&data), sizeof(unsigned long));
	if(!s.IsSuccess() || (s.m_clReturnCode != sizeof(unsigned long)))
	{
		CLLogger::WriteLogMsg("In CLDataPosterBySTLqueue::PostData(), pIOVectors->ReadBlock error", 0);
		return CLStatus(-1, DATA_POSTER_POST_ERROR);
	}

	if(m_pSTLqueue->PushData(data).IsSuccess())
		return CLStatus(sizeof(unsigned long), 0);
	else
	{
		CLLogger::WriteLogMsg("In CLDataPosterBySTLqueue::PostData(), m_pSTLqueue->PushData error", 0);
		return CLStatus(-1, DATA_POSTER_POST_ERROR);
	}
}