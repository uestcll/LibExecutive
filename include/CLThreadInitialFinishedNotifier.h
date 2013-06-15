#ifndef CLThreadInitialFinishedNotifier_H
#define CLThreadInitialFinishedNotifier_H

#include "CLExecutiveInitialFinishedNotifier.h"

class CLEvent;

class CLThreadInitialFinishedNotifier : public CLExecutiveInitialFinishedNotifier
{
public:
	CLThreadInitialFinishedNotifier(CLEvent *pEvent);
	virtual ~CLThreadInitialFinishedNotifier();

	virtual CLStatus NotifyInitialFinished(bool bInitialSuccess);
	virtual bool IsInitialSuccess();

private:
	CLThreadInitialFinishedNotifier(const CLThreadInitialFinishedNotifier&);
	CLThreadInitialFinishedNotifier& operator=(const CLThreadInitialFinishedNotifier&);

private:
	bool m_bSuccess;
	CLEvent *m_pEvent;
};

#endif