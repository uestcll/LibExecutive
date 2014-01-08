#include <errno.h>
#include "CLDataPosterByNamedPipe.h"
#include "CLNamedPipe.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLDataPosterByNamedPipe::CLDataPosterByNamedPipe(CLNamedPipe *pNamedPipe)
{
	m_pNamedPipe = pNamedPipe;
}

CLDataPosterByNamedPipe::~CLDataPosterByNamedPipe()
{
}

CLStatus CLDataPosterByNamedPipe::PostData(CLIOVectors *pIOVectors)
{
	if((pIOVectors == 0) || (pIOVectors->Size() == 0))
		return CLStatus(-1, DATA_POSTER_POST_ERROR);

	CLStatus s = m_pNamedPipe->Write(*pIOVectors);
	if(!s.IsSuccess())
	{
		if(s.m_clErrorCode == EAGAIN)
			return CLStatus(-1, DATA_POSTER_POST_PENDING);

		CLLogger::WriteLogMsg("In CLDataPosterByNamedPipe::PostData, m_pNamedPipe->Write error", 0);
		return CLStatus(-1, DATA_POSTER_POST_ERROR);
	}

	if(s.m_clReturnCode == 0)
		return CLStatus(-1, DATA_POSTER_POST_PENDING);
	else
		return s;
}

CLStatus CLDataPosterByNamedPipe::PostDelayedData(CLIOVectors *pIOVectors)
{
	return PostData(pIOVectors);
}