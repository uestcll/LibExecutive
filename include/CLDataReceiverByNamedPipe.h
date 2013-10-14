#ifndef CLDATARECEIVERBYNAMEDPIPE_H
#define CLDATARECEIVERBYNAMEDPIPE_H

#include "CLDataReceiver.h"
#include "CLNamedPipe.h"

class CLBuffer;

class CLDataReceiverByNamedPipe : public CLDataReceiver
{
public:
	CLDataReceiverByNamedPipe(const char *pStrPipeName);
	virtual ~CLDataReceiverByNamedPipe();

	virtual CLStatus GetData(CLBuffer *pBuffer);

private:
	CLDataReceiverByNamedPipe(const CLDataReceiverByNamedPipe&);
	CLDataReceiverByNamedPipe& operator=(const CLDataReceiverByNamedPipe&);

private:
	CLNamedPipe m_NamedPipe;

};

#endif