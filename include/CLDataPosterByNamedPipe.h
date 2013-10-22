#ifndef CLDataPosterByNamedPipe_H
#define CLDataPosterByNamedPipe_H

#include "CLDataPoster.h"

class CLNamedPipe;

class CLDataPosterByNamedPipe : public CLDataPoster
{
public:
	CLDataPosterByNamedPipe(CLNamedPipe *pNamedPipe);
	virtual ~CLDataPosterByNamedPipe();

	virtual CLStatus PostData(CLIOVectors *pIOVectors);
	virtual CLStatus PostDelayedData(CLIOVectors *pIOVectors);

private:
	CLDataPosterByNamedPipe(const CLDataPosterByNamedPipe&);
	CLDataPosterByNamedPipe& operator=(const CLDataPosterByNamedPipe&);

private:
	CLNamedPipe *m_pNamedPipe;
};

#endif