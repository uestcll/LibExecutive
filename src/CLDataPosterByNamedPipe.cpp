#include "CLDataPosterByNamedPipe.h"
#include "CLIOVector.h"
#include "CLLogger.h"

using namespace std;

// CLDataPosterByNamedPipe::CLDataPosterByNamedPipe(const char* strPipeName, int pipeType)
// {
// 	if(strPipeName == 0)
// 		throw "In CLDataPosterByNamedPipe::CLDataPosterByNamedPipe(), strPipeName == 0";
// 	if(pipeType == PIPE_QUEUE_BETWEEN_PROCESS)
// 	{
// 		m_pNamedPipe = new CLNamedPipe(strPipeName, strPipeName, PIPE_FOR_WRITE);
// 	}
// 	else if(pipeType == PIPE_QUEUE_IN_PROCESS)
// 	{
// 		m_pNamedPipe = new CLNamedPipe(strPipeName, PIPE_FOR_WRITE);
// 	}
// }

CLDataPosterByNamedPipe::CLDataPosterByNamedPipe(CLNamedPipe *pipe) : m_pNamedPipe(pipe)
{

}

CLDataPosterByNamedPipe::~CLDataPosterByNamedPipe()
{
	// if(m_pNamedPipe != 0)
	// {
	// 	delete m_pNamedPipe;
	// 	m_pNamedPipe = 0;
	// }
}

CLStatus CLDataPosterByNamedPipe::PostData(CLIOVector* pDataVec) 
{
	if(!pDataVec || (pDataVec->Length() == 0))
		return CLStatus(-1, 0);

	CLStatus s = m_pNamedPipe->WriteVecs(*pDataVec);
	if(!s.IsSuccess())
		CLLogger::WriteLogMsg("In CLDataPosterByNamedPipe::PostData(), m_pNamedPipe->WriteVecs() error", 0);

	return s;
}