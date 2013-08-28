#ifndef CLSharedMsgReceiverByNamedPipe_H
#define CLSharedMsgReceiverByNamedPipe_H

#include <map>
#include "CLMessageReceiverByNamedPipe.h"


class CLMessageDeserializer;
class CLMessage;

class CLSharedMsgReceiverByNamedPipe : public CLMessageReceiverByNamedPipe
{
public:
	explicit CLSharedMsgReceiverByNamedPipe(const char *pstrPipeName);
	virtual ~CLSharedMsgReceiverByNamedPipe();

	CLStatus RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer);

protected:
	virtual CLStatus ReadMsgFromPipe(int fd);

private:
	CLSharedMsgReceiverByNamedPipe(const CLSharedMsgReceiverByNamedPipe&);
	CLSharedMsgReceiverByNamedPipe& operator=(const CLSharedMsgReceiverByNamedPipe&);

private:
	map<unsigned long, CLMessageDeserializer*> m_DeserializerTable;

	bool m_bNewMsg;
	bool m_bReadHead;

	char* m_pMsgBuffer;

	int  m_iHeadLength;
	int  m_iMsgLength;
	int  m_iReadLength;
};

#endif