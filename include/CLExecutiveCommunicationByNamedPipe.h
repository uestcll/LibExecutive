#ifndef CLExecutiveCommunicationByNamedPipe_H
#define CLExecutiveCommunicationByNamedPipe_H

#include <string>
#include "CLExecutiveCommunication.h"
#include "CLEvent.h"

class CLMessage;

class CLExecutiveCommunicationByNamedPipe : public CLExecutiveCommunication
{
public:
	CLExecutiveCommunicationByNamedPipe(const char *pstrExecutiveName, bool bDeleteMsg = true);
	virtual ~CLExecutiveCommunicationByNamedPipe();

	virtual CLStatus PostExecutiveMessage(CLMessage *pMessage);

protected:
	virtual char *GetMsgBuf(CLMessage *pMsg, unsigned int *pLength) = 0;

private:
	CLExecutiveCommunicationByNamedPipe(const CLExecutiveCommunicationByNamedPipe&);
	CLExecutiveCommunicationByNamedPipe& operator=(const CLExecutiveCommunicationByNamedPipe&);

protected:
	string m_strExecutiveName;

private:
	int m_Fd;
	CLEvent m_Event;
	long m_lPipeBufSize;
	bool m_bDeleteMsg;
};

#endif