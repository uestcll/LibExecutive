#ifndef CLDATAPOSTERBYNAMEDPIPE_H
#define CLDATAPOSTERBYNAMEDPIPE_H

#include "CLDataPoster.h"
#include "CLNamedPipe.h"

class CLDataPosterByNamedPipe : public CLDataPoster
{
public:
	CLDataPosterByNamedPipe(const char* strPipeName);
	virtual ~CLDataPosterByNamedPipe();

private:
	virtual CLStatus PostData(CLIOVector dataVec);

private:
	CLNamedPipe *m_pNamedPipe;

	int m_iDataStartIndex;

};


#endif