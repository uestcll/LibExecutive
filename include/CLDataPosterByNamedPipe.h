#ifndef CLDATAPOSTERBYNAMEDPIPE_H
#define CLDATAPOSTERBYNAMEDPIPE_H

#include "CLDataPoster.h"
#include "CLNamedPipe.h"

class CLDataPosterByNamedPipe : public CLDataPoster
{
public:
	// CLDataPosterByNamedPipe(const char* strPipeName, int pipeType);
	CLDataPosterByNamedPipe(CLNamedPipe *pipe);
	virtual ~CLDataPosterByNamedPipe();

public:
	virtual CLStatus PostData(CLIOVector* pDataVec);

private:
	CLNamedPipe *m_pNamedPipe;
};


#endif