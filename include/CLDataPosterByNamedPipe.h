#ifndef CLDataPosterByNamedPipe_H
#define CLDataPosterByNamedPipe_H

#include "CLDataPoster.h"
#include "CLNamedPipe.h"

class CLDataPosterByNamedPipe : public CLDataPoster
{
public:
	CLDataPosterByNamedPipe(const char *pstrNamedPipe);
	virtual ~CLDataPosterByNamedPipe();

	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus PostData(CLIOVectors *pIOVectors);

private:
	CLDataPosterByNamedPipe(const CLDataPosterByNamedPipe&);
	CLDataPosterByNamedPipe& operator=(const CLDataPosterByNamedPipe&);

private:
	CLNamedPipe m_NamedPipe;
};

#endif