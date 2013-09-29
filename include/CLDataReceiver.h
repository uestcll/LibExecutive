#ifndef CLDATARECEIVER_H
#define CLDATARECEIVER_H

#include "CLStatus.h"

class CLBuffer;

class CLDataReceiver
{
public:
	CLDataReceiver();
	virtual ~CLDataReceiver();

	virtual CLStatus GetData(CLBuffer *pBuffer) = 0;//m_clReturnCode is the data read length , <0 is error and not success

private:
	CLDataReceiver(const CLDataReceiver&);
	CLDataReceiver& operator=(const CLDataReceiver&);
};

#endif
