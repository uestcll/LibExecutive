#ifndef CLMessageReceiverByNamedPipe_H
#define CLMessageReceiverByNamedPipe_H

#include <queue>
#include "CLStatus.h"
#include "CLMessageReceiver.h"

class CLMessage;
//class CLMessageReceiverBySTLqueue;

using namespace std;

class CLMessageReceiverByNamedPipe : public CLMessageReceiver
{
public:
	CLMessageReceiverByNamedPipe(const char *pstrPipeName);
	virtual ~CLMessageReceiverByNamedPipe();

	virtual CLMessage *GetMessage();

protected:
	virtual CLStatus ReadMsgFromPipe(int fd) = 0;

private:
	CLMessageReceiverByNamedPipe(const CLMessageReceiverByNamedPipe&);
	CLMessageReceiverByNamedPipe& operator=(const CLMessageReceiverByNamedPipe&);

protected:
	std::queue<CLMessage*> m_MessageQueue;
};

#endif