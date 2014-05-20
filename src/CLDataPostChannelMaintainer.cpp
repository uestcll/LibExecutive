#include "CLDataPostChannelMaintainer.h"
#include "CLInitialDataPostChannelNotifier.h"
#include "CLLogger.h"

CLDataPostChannelMaintainer::CLDataPostChannelMaintainer()
{
}

CLDataPostChannelMaintainer::~CLDataPostChannelMaintainer()
{
}

void CLDataPostChannelMaintainer::NotifyConnectionResult(bool bSuccess)
{
	CLLogger::WriteLogMsg("In CLDataPostChannelMaintainer::NotifyConnectionResult(), this function isnot implemented", 0);
}