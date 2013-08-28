#ifndef CLPrivateMsgReceiverbyNamedPipe_H
#define CLPrivateMsgReceiverbyNamedPipe_H

#include "CLMessageReceiverByNamedPipe.h"

class CLMessage;

class CLPrivateMsgReceiverbyNamedPipe : public CLMessageQueueByNamedPipe
{
public:
	explicit CLPrivateMsgReceiverbyNamedPipe(const char *pstrPipeName);
	virtual ~CLPrivateMsgReceiverbyNamedPipe();

protected:
	virtual CLStatus ReadMsgFromPipe(int fd);

private:
	CLPrivateMsgReceiverbyNamedPipe(const CLPrivateMsgReceiverbyNamedPipe&);
	CLPrivateMsgReceiverbyNamedPipe& operator=(const CLPrivateMsgReceiverbyNamedPipe&);
};

#endif