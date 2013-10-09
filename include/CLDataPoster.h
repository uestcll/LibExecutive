#ifndef CLDataPoster_H
#define CLDataPoster_H

#include "CLStatus.h"

class CLIOVectors;

class CLDataPoster
{
public:
	CLDataPoster();
	virtual ~CLDataPoster();

	virtual CLStatus Initialize(void *pContext) = 0;
	virtual CLStatus Uninitialize() = 0;

	virtual CLStatus PostData(CLIOVectors *pIOVectors) = 0;

private:
	CLDataPoster(const CLDataPoster&);
	CLDataPoster& operator=(const CLDataPoster&);
};

#endif