#ifndef CLDATARECEIVERBYNAMEDPIPE_H
#define CLDATARECEIVERBYNAMEDPIPE_H

#include "CLDataReceiver.h"

class CLBuffer;
Class CLNamedPipe;

class CLDataReceiverByNamedPipe : public CLDataReceiverByNamedPipe
{
public:
	CLDataReceiverByNamedPipe(CLNamedPipe *pNamedPipe);
	virtual ~CLDataReceiverByNamedPipe();

	virtual CLStatus GetData(CLBuffer *pBuffer);

private:
	CLDataReceiverByNamedPipe(const CLDataReceiverByNamedPipe&);
	CLDataReceiverByNamedPipe& operator=(const CLDataReceiverByNamedPipe&);

private:
	CLNamedPipe *m_pNamedPipe;

};

#endif