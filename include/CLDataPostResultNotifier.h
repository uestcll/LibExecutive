#ifndef CLDataPostResultNotifier_H
#define CLDataPostResultNotifier_H

#include "CLStatus.h"

class CLMessage;

class CLDataPostResultNotifier
{
public:
	CLDataPostResultNotifier();
	explicit CLDataPostResultNotifier(bool bDeleteMsg);
	virtual ~CLDataPostResultNotifier();

	virtual CLStatus Notify(int Result);

	void SetMsg(CLMessage *pMsg);

protected:
	virtual CLStatus NotifySuccess();
	virtual CLStatus NotifyFailure();

private:
	CLDataPostResultNotifier(const CLDataPostResultNotifier&);
	CLDataPostResultNotifier& operator=(const CLDataPostResultNotifier&);

protected:
	CLMessage *m_pMsg;
	bool m_bDeleteMsg;
};

#endif