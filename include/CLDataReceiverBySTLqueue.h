#ifndef CLDataReceiverBySTLqueue_H
#define CLDataReceiverBySTLqueue_H

#include "CLDataReceiver.h"

class CLSTLqueue;

class CLDataReceiverBySTLqueue : public CLDataReceiver
{
public:
	explicit CLDataReceiverBySTLqueue(CLSTLqueue *pSTLqueue);
	virtual ~CLDataReceiverBySTLqueue();

	virtual CLStatus GetData(CLIOVectors& IOVectors, void **ppContext);

private:
	CLDataReceiverBySTLqueue(const CLDataReceiverBySTLqueue&);
	CLDataReceiverBySTLqueue& operator=(const CLDataReceiverBySTLqueue&);

private:
	CLSTLqueue *m_pSTLqueue;
};

#endif