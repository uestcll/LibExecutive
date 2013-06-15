#ifndef CLMessageQueueByNamedPipe_H
#define CLMessageQueueByNamedPipe_H

#include "CLStatus.h"
#include "CLEvent.h"

class CLMessage;

using namespace std;

class CLMessageQueueByNamedPipe
{
public:
	CLMessageQueueByNamedPipe(const char *pstrPipeName);
	virtual ~CLMessageQueueByNamedPipe();

	CLMessage *GetMessage();

protected:
	virtual CLMessage *ReadMsgFromPipe(int fd) = 0;

private:
	CLMessageQueueByNamedPipe(const CLMessageQueueByNamedPipe&);
	CLMessageQueueByNamedPipe& operator=(const CLMessageQueueByNamedPipe&);

private:
	CLEvent m_Event;
	int m_Fd;
};

#endif