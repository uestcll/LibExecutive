#ifndef CLMsgPostResultsNotifier_H
#define CLMsgPostResultsNotifier_H

#include "CLStatus.h"

class CLMsgPostResultsNotifier
{
public:
	CLMsgPostResultsNotifier();
	virtual ~CLMsgPostResultsNotifier();

	virtual CLStatus InitialNotify(int Result) = 0;
	virtual CLStatus MsgPostNotify(int Result) = 0;

private:
	CLMsgPostResultsNotifier(const CLMsgPostResultsNotifier&);
	CLMsgPostResultsNotifier& operator=(const CLMsgPostResultsNotifier&);
};

#endif