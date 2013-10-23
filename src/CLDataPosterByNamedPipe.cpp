#include "CLDataPosterByNamedPipe.h"
#include "CLIOVector.h"

using namespace std;

CLDataPosterByNamedPipe::CLDataPosterByNamedPipe(const char* strPipeName) : m_iDataStartIndex(0)
{
	m_pNamedPipe = new CLNamedPipe(strPipeName, PIPE_FOR_WRITE);
}

CLDataPosterByNamedPipe::CLDataPosterByNamedPipe(const char* strPipeName, const char* strMutexName) : m_iDataStartIndex(0)
{
	m_pNamedPipe = new CLNamedPipe(strPipeName, strMutexName, PIPE_FOR_WRITE);
}

CLDataPosterByNamedPipe::CLDataPosterByNamedPipe(CLNamedPipe *pipe) : m_pNamedPipe(pipe), m_iDataStartIndex(0)
{

}

CLDataPosterByNamedPipe::~CLDataPosterByNamedPipe()
{

}

CLStatus CLDataPosterByNamedPipe::PostData(CLIOVector dataVec) 
{
	if(0 == m_iDataStartIndex)
	{
		
	}
	int ctxLen = dataVec.Length();


	return CLStatus(0, 0);
}