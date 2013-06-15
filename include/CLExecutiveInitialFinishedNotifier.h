#ifndef CLExecutiveInitialFinishedNotifier_H
#define CLExecutiveInitialFinishedNotifier_H

#include "CLStatus.h"

class CLExecutiveInitialFinishedNotifier
{
public:
	CLExecutiveInitialFinishedNotifier();
	virtual ~CLExecutiveInitialFinishedNotifier();

	virtual CLStatus NotifyInitialFinished(bool bInitialSuccess) = 0;
	virtual bool IsInitialSuccess() = 0;

private:
	CLExecutiveInitialFinishedNotifier(const CLExecutiveInitialFinishedNotifier&);
	CLExecutiveInitialFinishedNotifier& operator=(const CLExecutiveInitialFinishedNotifier&);
};

#endif