#include "CLDataPosterByNamedPipe.h"

CLDataPosterByNamedPipe::CLDataPosterByNamedPipe(const char *pstrNamedPipe) : m_NamedPipe(pstrNamedPipe)
{
}

CLDataPosterByNamedPipe::~CLDataPosterByNamedPipe()
{
}

CLStatus CLDataPosterByNamedPipe::Initialize()
{
	return CLStatus(0, 0);
}

CLStatus CLDataPosterByNamedPipe::Uninitialize()
{
	return CLStatus(0, 0);
}

//bug
CLStatus CLDataPosterByNamedPipe::PostData(CLIOVectors *pIOVectors)
{
	char *pBuffer = 0;
	size_t len_buffer = 0;

	CLStatus s = pIOVectors->PopFront(&pBuffer, &len_buffer);
	if(s.IsSuccess())
	{
		CLStatus s1 = m_NamedPipe->Write(pBuffer, len_buffer);
		if(s1.IsSuccess())
			return CLStatus(0, 0);
	}

	delete pIOVectors;

	return CLStatus(-1, POSTER_POST_ERROR);
}