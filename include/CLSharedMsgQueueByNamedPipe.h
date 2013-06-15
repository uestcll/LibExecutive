#ifndef CLSharedMsgQueueByNamedPipe_H
#define CLSharedMsgQueueByNamedPipe_H

#include <map>
#include "CLMessageQueueByNamedPipe.h"

class CLMessageDeserializer;
class CLMessage;

class CLSharedMsgQueueByNamedPipe : public CLMessageQueueByNamedPipe
{
public:
	explicit CLSharedMsgQueueByNamedPipe(const char *pstrPipeName);
	virtual ~CLSharedMsgQueueByNamedPipe();

	CLStatus RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer);

protected:
	virtual CLMessage *ReadMsgFromPipe(int fd);

private:
	CLSharedMsgQueueByNamedPipe(const CLSharedMsgQueueByNamedPipe&);
	CLSharedMsgQueueByNamedPipe& operator=(const CLSharedMsgQueueByNamedPipe&);

private:
	map<unsigned long, CLMessageDeserializer*> m_DeserializerTable;
};

#endif