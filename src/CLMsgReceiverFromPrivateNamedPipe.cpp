#include <errno.h>
#include <string>
#include "CLLogger.h"
#include "CLMsgReceiverFromPrivateNamedPipe.h"
#include "CLNamedPipe.h"

using namespace std;

#define FILE_PATH_FOR_NAMED_PIPE "/tmp/"

CLMsgReceiverFromPrivateNamedPipe::CLMsgReceiverFromPrivateNamedPipe(const char *pstrPipeName) : CLMessageReceiver(NULL)
{
	string str = FILE_PATH_FOR_NAMED_PIPE;
	str += pstrPipeName;

	m_pNamedPipe = new CLNamedPipe(str.c_str());
}

CLMsgReceiverFromPrivateNamedPipe::~CLMsgReceiverFromPrivateNamedPipe()
{
	if(m_pNamedPipe)
		delete m_pNamedPipe;
}

CLStatus CLMsgReceiverFromPrivateNamedPipe::GetMessage(CLMessage **ppMsg)
{
	return m_pNamedPipe->Read(ppMsg, sizeof(CLMessage *));
}