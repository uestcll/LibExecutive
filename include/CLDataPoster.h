#ifndef CLDataPoster_H
#define CLDataPoster_H

#include "CLStatus.h"

#define INITIALIZE_PENDING 1
#define INITIALIZE_ERROR 0

class CLDataPoster
{
public:
	CLDataPoster();
	virtual ~CLDataPoster();

	virtual CLStatus Initialize() = 0;
	virtual CLStatus Uninitialize() = 0;

private:
	CLDataPoster(const CLDataPoster&);
	CLDataPoster& operator=(const CLDataPoster&);
};

#endif