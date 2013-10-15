#include "CLInitialDataPostChannelNotifier.h"
#include "ErrorCode.h"

CLInitialDataPostChannelNotifier::CLInitialDataPostChannelNotifier()
{
}

CLInitialDataPostChannelNotifier::~CLInitialDataPostChannelNotifier()
{
}

CLStatus CLInitialDataPostChannelNotifier::Notify(int Result)
{
	if(Result == DATA_POSTER_INITIALIZE_SUCCESS)
	{
		return NotifySuccess();
	}
	else if(Result == DATA_POSTER_INITIALIZE_ERROR)
	{
		return NotifyFailure();
	}

	return CLStatus(-1, NORMAL_ERROR);
}

CLStatus CLInitialDataPostChannelNotifier::NotifySuccess()
{
	return CLStatus(0, 0);
}

CLStatus CLInitialDataPostChannelNotifier::NotifyFailure()
{
	return CLStatus(0, 0);
}