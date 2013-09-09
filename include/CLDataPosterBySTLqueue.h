#ifndef CLDataPosterBySTLqueue_H
#define CLDataPosterBySTLqueue_H

#include "CLDataPoster.h"

class CLDataReceiverBySTLqueue;

//not implement..................
class CLDataPosterBySTLqueue : public CLDataPoster
{
public:
	CLDataPosterBySTLqueue(CLDataReceiverBySTLqueue *pDataReceiverBySTLqueue);
	virtual ~CLDataPosterBySTLqueue();

	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus PostData(CLIOVectors *pIOVectors);

private:
	CLDataPosterBySTLqueue(const CLDataPosterBySTLqueue&);
	CLDataPosterBySTLqueue& operator=(const CLDataPosterBySTLqueue&);

private:
	CLDataReceiverBySTLqueue *m_pDataReceiverBySTLqueue;
};

#endif