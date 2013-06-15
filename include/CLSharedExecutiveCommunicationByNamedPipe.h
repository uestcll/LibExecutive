#ifndef CLSharedExecutiveCommunicationByNamedPipe_H
#define CLSharedExecutiveCommunicationByNamedPipe_H

#include <map>
#include "CLExecutiveCommunicationByNamedPipe.h"

using namespace std;

class CLMessageSerializer;

class CLSharedExecutiveCommunicationByNamedPipe : public CLExecutiveCommunicationByNamedPipe
{
public:
	CLSharedExecutiveCommunicationByNamedPipe(const char *pstrExecutiveName);
	virtual ~CLSharedExecutiveCommunicationByNamedPipe();

	CLStatus RegisterSerializer(unsigned long lMsgID, CLMessageSerializer *pSerializer);

protected:
	virtual char *GetMsgBuf(CLMessage *pMsg, unsigned int *pLength);

private:
	CLSharedExecutiveCommunicationByNamedPipe(const CLSharedExecutiveCommunicationByNamedPipe&);
	CLSharedExecutiveCommunicationByNamedPipe& operator=(const CLSharedExecutiveCommunicationByNamedPipe&);

private:
	map<unsigned long, CLMessageSerializer*> m_SerializerTable;
};

#endif