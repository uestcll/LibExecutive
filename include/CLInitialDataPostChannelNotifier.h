#ifndef CLInitialDataPostChannelNotifier_H
#define CLInitialDataPostChannelNotifier_H

#include "CLStatus.h"

class CLInitialDataPostChannelNotifier
{
public:
	CLInitialDataPostChannelNotifier();
	virtual ~CLInitialDataPostChannelNotifier();

	virtual CLStatus Notify(int Result);

protected:
	virtual CLStatus NotifySuccess();
	virtual CLStatus NotifyFailure();

private:
	CLInitialDataPostChannelNotifier(const CLInitialDataPostChannelNotifier&);
	CLInitialDataPostChannelNotifier& operator=(const CLInitialDataPostChannelNotifier&);
};

#endif