#ifndef CLPrivateMsgQueuebyNamedPipe_H
#define CLPrivateMsgQueuebyNamedPipe_H

#include "CLMessageQueueByNamedPipe.h"

class CLMessage;

class CLPrivateMsgQueuebyNamedPipe : public CLMessageQueueByNamedPipe
{
public:
	explicit CLPrivateMsgQueuebyNamedPipe(const char *pstrPipeName);
	virtual ~CLPrivateMsgQueuebyNamedPipe();

protected:
	virtual CLMessage *ReadMsgFromPipe(int fd);

private:
	CLPrivateMsgQueuebyNamedPipe(const CLPrivateMsgQueuebyNamedPipe&);
	CLPrivateMsgQueuebyNamedPipe& operator=(const CLPrivateMsgQueuebyNamedPipe&);
};

#endif