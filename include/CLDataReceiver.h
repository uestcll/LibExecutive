#ifndef CLDataReceiver_H
#define CLDataReceiver_H

#include "CLStatus.h"

class CLIOVectors;

#define RECEIVED_ZERO_BYTE 1
#define RECEIVED_ERROR 2
#define RECEIVED_PENDING 3

class CLDataReceiver
{
public:
	CLDataReceiver();
	virtual ~CLDataReceiver();

	virtual CLStatus GetData(CLIOVectors *pIOVectors, void **ppContext) = 0;

private:
	CLDataReceiver(const CLDataReceiver&);
	CLDataReceiver& operator=(const CLDataReceiver&);
};

#endif