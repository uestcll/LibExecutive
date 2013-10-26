#ifndef CLDATAPOSTERBYNAMEDPIPE_H
#define CLDATAPOSTERBYNAMEDPIPE_H

#include "CLDataPoster.h"
#include "CLNamedPipe.h"

#define PIPE_QUEUE_BETWEEN_PROCESS 30
#define PIPE_QUEUE_IN_PROCESS 31

class CLDataPosterByNamedPipe : public CLDataPoster
{
public:
	CLDataPosterByNamedPipe(const char* strPipeName, int pipeType);
	CLDataPosterByNamedPipe(CLNamedPipe *pipe);
	virtual ~CLDataPosterByNamedPipe();

private:
	virtual CLStatus PostData(CLIOVector& dataVec);

private:
	CLNamedPipe *m_pNamedPipe;

	int m_iDataStartIndex;

};


#endif