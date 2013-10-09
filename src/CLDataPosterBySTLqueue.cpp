#include "CLDataPosterBySTLqueue.h"
#include "CLSTLqueue.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLDataPosterBySTLqueue::CLDataPosterBySTLqueue(CLSTLqueue *pSTLqueue)
{
	if(pSTLqueue == 0)
		throw "In CLDataPosterBySTLqueue::CLDataPosterBySTLqueue(), pSTLqueue == 0";

	m_pSTLqueue = pSTLqueue;
}

CLDataPosterBySTLqueue::~CLDataPosterBySTLqueue()
{
	delete m_pSTLqueue;
}

CLStatus CLDataPosterBySTLqueue::Initialize(void *pContext)
{
	return CLStatus(0, 0);
}

CLStatus CLDataPosterBySTLqueue::Uninitialize()
{
	return CLStatus(0, 0);
}

CLStatus CLDataPosterBySTLqueue::PostData(CLIOVectors *pIOVectors)
{
	if((pIOVectors == 0) || (pIOVectors->Size() < sizeof(unsigned long)))
	{
		CLLogger::WriteLogMsg("In CLDataPosterBySTLqueue::PostData(), pIOVector error", 0);
		return CLStatus(-1, DATA_POSTER_POST_ERROR);
	}

	unsigned long data = 0;
	CLStatus s = pIOVectors->ReadBlock(0, (char *)(&data), sizeof(unsigned long));
	if(!s.IsSuccess() || s.m_clReturnCode != sizeof(unsigned long))
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