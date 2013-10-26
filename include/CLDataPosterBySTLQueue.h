#ifndef CLDATAPOSTERBYSTLQUEUE_H
#define CLDATAPOSTERBYSTLQUEUE_H

#include "CLDataPoster.h"
#include "CLIOVector.h"

class CLSTLQueue;

class CLDataPosterBySTLQueue : public CLDataPoster
{
public:
	CLDataPosterBySTLQueue(CLSTLQueue queue);
	virtual ~CLDataPosterBySTLQueue();

	virtual CLStatus PostData(CLIOVector& dataVec);
	 
private:
	CLSTLQueue m_Queue;
};