#ifndef CLMultiDeserializer_H
#define CLMultiDeserializer_H

#include <map>
#include "CLMessageDeserializer.h"

class CLMultiDeserializer : public CLMessageDeserializer
{
public:
	CLMultiDeserializer();
	virtual ~CLMultiDeserializer();

	CLStatus RegisterDeserializer(unsigned long lMsgID, CLMessageDeserializer *pDeserializer);
	CLStatus UnregisterDeserializer(unsigned long lMsgID);

	virtual CLStatus Deserialize(CLIOVectors *pIOVectors, unsigned int Index, unsigned int Length, CLMessage **ppMsg);

private:
	CLMultiDeserializer(const CLMultiDeserializer&);
	CLMultiDeserializer& operator=(const CLMultiDeserializer&);

private:
	std::map<unsigned long, CLMessageDeserializer*> m_DeserializerTable;
};

#endif