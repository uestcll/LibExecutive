#ifndef CLDataPosterBySTLqueue_H
#define CLDataPosterBySTLqueue_H

#include "CLDataPoster.h"

class CLSTLqueue;

class CLDataPosterBySTLqueue : public CLDataPoster
{
public:
	CLDataPosterBySTLqueue(CLSTLqueue *pSTLqueue);
	virtual ~CLDataPosterBySTLqueue();

	virtual CLStatus Initialize(void *pContext);
	virtual CLStatus Uninitialize();

	virtual CLStatus PostData(CLIOVectors *pIOVectors);

private:
	CLDataPosterBySTLqueue(const CLDataPosterBySTLqueue&);
	CLDataPosterBySTLqueue& operator=(const CLDataPosterBySTLqueue&);

private:
	CLSTLqueue *m_pSTLqueue;
};

#endif