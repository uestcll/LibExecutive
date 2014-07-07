#include "CLDataReceiver.h"
#include "CLLogger.h"

CLDataReceiver::CLDataReceiver()
{
}

CLDataReceiver::~CLDataReceiver()
{
}

int CLDataReceiver::GetFd()
{
	CLLogger::WriteLogMsg("In CLDataReceiver::GetFd, Error", 0);
	return -1;
}