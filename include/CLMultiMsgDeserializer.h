#ifndef CLMULTIMSGDESERIALIZER_H
#define CLMULTIMSGDESERIALIZER_H

#include <map>
#include "CLMessageDeserializer.h"

using namespace std;

class CLMultiMsgDeserializer : public CLMessageDeserializer
{
public:
	CLMultiMsgDeserializer();
	virtual ~CLMultiMsgDeserializer();

	virtual CLStatus Deserialize(CLIOVector dataVec, CLMessage **pMsg);

	CLStatus RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer);
	CLStatus UnRegisterDeserializer(unsigned long lMsgID);


private:
	CLMultiMsgDeserializer(const CLMultiMsgDeserializer&);
	CLMultiMsgDeserializer& operator=(const CLMultiMsgDeserializer&);

private:
	map<unsigned long, CLMessageDeserializer*> m_DeserializerTable;

};

#endif