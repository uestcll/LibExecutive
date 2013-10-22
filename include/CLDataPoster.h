#ifndef CLDATAPOSTER_H
#define CLDATAPOSTER_H

#include "CLStatus.h"

class CLIOVector;

class CLDataPoster
{
public:
	CLDataPoster();
	virtual ~CLDataPoster();

	virtual CLStatus PostData(CLIOVector dataVec) = 0;

private:
	CLDataPoster(const CLDataPoster&);
	CLDataPoster& operator=(const CLDataPoster&);
};