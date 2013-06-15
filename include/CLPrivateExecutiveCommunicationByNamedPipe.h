#ifndef CLPrivateExecutiveCommunicationByNamedPipe_H
#define CLPrivateExecutiveCommunicationByNamedPipe_H

#include "CLExecutiveCommunicationByNamedPipe.h"

class CLMessage;

class CLPrivateExecutiveCommunicationByNamedPipe : public CLExecutiveCommunicationByNamedPipe
{
public:
	CLPrivateExecutiveCommunicationByNamedPipe(const char *pstrExecutiveName);
	virtual ~CLPrivateExecutiveCommunicationByNamedPipe();

protected:
	virtual char *GetMsgBuf(CLMessage *pMsg, unsigned int *pLength);

private:
	CLPrivateExecutiveCommunicationByNamedPipe(const CLPrivateExecutiveCommunicationByNamedPipe&);
	CLPrivateExecutiveCommunicationByNamedPipe& operator=(const CLPrivateExecutiveCommunicationByNamedPipe&);
};

#endif