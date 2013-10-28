#ifndef CLDATAPOSTERBYSTLQUEUE_H
#define CLDATAPOSTERBYSTLQUEUE_H

#include "CLDataPoster.h"
#include "CLIOVector.h"
#include "CLSTLQueue.h"


class CLDataPosterBySTLQueue : public CLDataPoster
{
public:
	CLDataPosterBySTLQueue(CLSTLQueue *pQueue);
	virtual ~CLDataPosterBySTLQueue();

	virtual CLStatus PostData(CLIOVector& dataVec);
	 
private:
	CLSTLQueue *m_pQueue;

private:
	CLDataPosterBySTLQueue(const CLDataPosterBySTLQueue&);
	CLDataPosterBySTLQueue& operator=(const CLDataPosterBySTLQueue&);
};

#endif