#include <errno.h>
#include <string>
#include "CLLogger.h"
#include "CLMsgReceiverFromPrivateNamedPipe.h"
#include "CLNamedPipe.h"

using namespace std;

#define FILE_PATH_FOR_NAMED_PIPE "/tmp/"

CLMsgReceiverFromPrivateNamedPipe::CLMsgReceiverFromPrivateNamedPipe(CLMessageDeserializer *pMsgDeserializer, const char *pstrPipeName) : CLMessageReceiver(pMsgDeserializer)
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
	long bufsize = m_pNamedPipe->GetSizeForAtomWriting();
	if(bufsize <= 0)
		return CLStatus(-1, MESSAGE_RECEIVE_ERROR);

	char *pBuf = new char[bufsize];
	CLStatus s = m_pNamedPipe->Read(pBuf, bufsize);
}