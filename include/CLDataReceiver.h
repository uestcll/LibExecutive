#ifndef CLDATARECEIVER_H
#define CLDATARECEIVER_H

#include "CLStatus.h"

class CLIOVector;

class CLDataReceiver
{
public:
	CLDataReceiver();
	virtual ~CLDataReceiver();

	virtual CLStatus GetData(CLIOVector &IOVec) = 0;//m_clReturnCode is the data read length , <0 is error and not success

	virtual const int GetFd(){return -1;}
private:
	CLDataReceiver(const CLDataReceiver&);
	CLDataReceiver& operator=(const CLDataReceiver&);
};

#endif
