#ifndef CLDataPostResultNotifier_H
#define CLDataPostResultNotifier_H

#include "CLStatus.h"

class CLMessage;

class CLDataPostResultNotifier
{
public:
	CLDataPostResultNotifier();
	virtual ~CLDataPostResultNotifier();

	virtual CLStatus Notify(int Result) = 0;

	void SetMsg(CLMessage *pMsg);

private:
	CLDataPostResultNotifier(const CLDataPostResultNotifier&);
	CLDataPostResultNotifier& operator=(const CLDataPostResultNotifier&);

protected:
	CLMessage *m_pMsg;
};

#endif