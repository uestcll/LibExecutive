#ifndef CLDATAPOSTRESULTNOTIFIER_H
#define CLDATAPOSTRESULTNOTIFIER_H

#include "CLStatus.h"

#define POST_MSG_ERROR 31
#define POST_MSG_PARTION 32
#define POST_MSG_SUCCESS 33

class CLMessage;

class CLDataPostResultNotifier
{
public:
	CLDataPostResultNotifier(bool isDataDel = false);
	virtual ~CLDataPostResultNotifier();

	virtual CLStatus NotifyResult(int result);

	virtual CLStatus NotifySuccess();
	virtual CLStatus NotifyFailure();

	CLStatus SetMessage(CLMessage *pMsg); 

protected:
	bool m_bDataDelFlag;
	CLMessage *m_pMsg;

private:
	CLDataPostResultNotifier(const CLDataPostResultNotifier&);
	CLDataPostResultNotifier& operator=(const CLDataPostResultNotifier&);
};

#endif