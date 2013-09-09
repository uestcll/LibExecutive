#ifndef CLDataPoster_H
#define CLDataPoster_H

#include "CLStatus.h"

#define POSTER_INITIALIZE_PENDING 1
#define POSTER_INITIALIZE_ERROR 0

#define POSTER_POST_PENDING 1
#define POSTER_POST_ERROR 0

class CLIOVectors;

class CLDataPoster
{
public:
	CLDataPoster();
	virtual ~CLDataPoster();

	virtual CLStatus Initialize() = 0;
	virtual CLStatus Uninitialize() = 0;

	virtual CLStatus PostData(CLIOVectors *pIOVectors) = 0;

private:
	CLDataPoster(const CLDataPoster&);
	CLDataPoster& operator=(const CLDataPoster&);
};

#endif