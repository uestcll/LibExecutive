#include "CLDataPostChannelMaintainer.h"
#include "CLInitialDataPostChannelNotifier.h"

CLDataPostChannelMaintainer::CLDataPostChannelMaintainer(CLInitialDataPostChannelNotifier *pNotifier)
{
	m_pNotifier = pNotifier;
}

CLDataPostChannelMaintainer::~CLDataPostChannelMaintainer()
{
}