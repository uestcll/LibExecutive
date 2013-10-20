#ifndef CLDataReceiverByNamedPipe_H
#define CLDataReceiverByNamedPipe_H

#include "CLDataReceiver.h"
#include "CLNamedPipe.h"

class CLDataReceiverByNamedPipe : public CLDataReceiver
{
public:
	CLDataReceiverByNamedPipe(const char *pstrNamedPipe);
	virtual ~CLDataReceiverByNamedPipe();

	virtual CLStatus GetData(CLIOVectors& IOVectors, void **ppContext);

private:
	CLDataReceiverByNamedPipe(const CLDataReceiverByNamedPipe&);
	CLDataReceiverByNamedPipe& operator=(const CLDataReceiverByNamedPipe&);

private:
	CLNamedPipe m_NamedPipe;
};

#endif