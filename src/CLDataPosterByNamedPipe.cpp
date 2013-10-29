#include "CLDataPosterByNamedPipe.h"
#include "CLIOVector.h"
#include "CLLogger.h"

using namespace std;


CLDataPosterByNamedPipe::CLDataPosterByNamedPipe(const char* strPipeName, int pipeType) : m_iDataStartIndex(0)
{
	if(strPipeName == 0)
		throw "In CLDataPosterByNamedPipe::CLDataPosterByNamedPipe(), strPipeName == 0";
	if(pipeType == PIPE_QUEUE_BETWEEN_PROCESS)
	{
		m_pNamedPipe = new CLNamedPipe(strPipeName, strPipeName, PIPE_FOR_WRITE);
	}
	else if(pipeType == PIPE_QUEUE_IN_PROCESS)
	{
		m_pNamedPipe = new CLNamedPipe(strPipeName, PIPE_FOR_WRITE);
	}

	m_pDataVecter = new CLIOVector();
}

CLDataPosterByNamedPipe::CLDataPosterByNamedPipe(CLNamedPipe *pipe) : m_pNamedPipe(pipe), m_iDataStartIndex(0)
{
	m_pDataVecter = new CLIOVector();
}

CLDataPosterByNamedPipe::~CLDataPosterByNamedPipe()
{
	if(m_pNamedPipe != 0)
	{
		delete m_pNamedPipe;
		m_pNamedPipe = 0;
	}
}

CLStatus CLDataPosterByNamedPipe::PostData(CLIOVector& dataVec) 
{
	m_pDataVecter->PushBackIOVecs(dataVec);

	CLIOVector tmpVec;

	if(0 == m_iDataStartIndex)
	{
		tmpVec = *m_pDataVecter;
	}
	else
	{
		m_pDataVecter->GetIOVecs(m_iDataStartIndex, dataVec.Length(), tmpVec);
	}

	CLStatus s = m_pNamedPipe->WriteVecs(tmpVec);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLDataPosterByNamedPipe::PostData(), m_pNamedPipe->WriteVecs() error", 0);
		return CLStatus(-1, POST_DATA_ERROR);
	}

	if(tmpVec.Length() == s.m_clReturnCode)
	{
		m_pDataVecter->FreeAndPopAll();
		
		m_iDataStartIndex = 0;
		return CLStatus(s.m_clReturnCode, POST_DATA_COMPLETE); //data post complete and notify the maintainer to unregister write event in epoll
	}
	else
	{
		m_iDataStartIndex += s.m_clReturnCode;
		return CLStatus(m_iDataStartIndex, POST_DATA_PARTION);			
	}

	return CLStatus(0, 0);
}