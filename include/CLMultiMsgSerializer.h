#ifndef CLMULTIMSGSERIALIZER_H
#define CLMULTIMSGSERIALIZER_H

#include "CLMessageSerializer.h"
#include "CLStatus.h"
#include <map>

class CLMultiMsgSerializer : public CLMessageSerializer
{
public:
	CLMultiMsgSerializer();
	virtual ~CLMultiMsgSerializer();

	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVector *pDataVec);

	CLStatus RegisterSerializer(unsigned long lMsgID, CLMessageSerializer *pSerializer);
	CLStatus UnRegisterSerializer(unsigned long lMsgID);

private:
	CLMultiMsgSerializer(const CLMultiMsgSerializer&);
	CLMultiMsgSerializer& operator=(const CLMultiMsgSerializer&);

private:
	std::map<unsigned long, CLMessageSerializer*> m_SerializerTable;		
};

#endif