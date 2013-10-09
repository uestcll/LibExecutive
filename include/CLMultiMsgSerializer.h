#ifndef CLMultiMsgSerializer_H
#define CLMultiMsgSerializer_H

#include <map>
#include "CLMessageSerializer.h"

using namespace std;

class CLMultiMsgSerializer : public CLMessageSerializer
{
public:
	CLMultiMsgSerializer();
	virtual ~CLMultiMsgSerializer();

	CLStatus RegisterSerializer(unsigned long lMsgID, CLMessageSerializer *pSerializer);
	CLStatus UnregisterSerializer(unsigned long lMsgID);

	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors);

private:
	CLMultiMsgSerializer(const CLMultiMsgSerializer&);
	CLMultiMsgSerializer& operator=(const CLMultiMsgSerializer&);

private:
	std::map<unsigned long, CLMessageSerializer*> m_SerializerTable;
};

#endif