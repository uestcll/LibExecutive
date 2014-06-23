#ifndef CLDataReceiver_H
#define CLDataReceiver_H

#include "CLUuid.h"
#include "CLStatus.h"

class CLIOVectors;

class CLDataReceiver
{
public:
	CLDataReceiver();
	virtual ~CLDataReceiver();

	virtual CLStatus GetData(CLIOVectors& IOVectors, void *pContext) = 0;
	virtual CLUuid GetUuid() = 0;
	virtual int GetFd();

private:
	CLDataReceiver(const CLDataReceiver&);
	CLDataReceiver& operator=(const CLDataReceiver&);
};

#endif