#ifndef CLMessagePoster_H
#define CLMessagePoster_H

#include <map>

class CLMessageSerializer;
class CLProtocolEncapsulator;

class CLMessagePoster
{
public:
	explicit CLMessagePoster(CLProtocolEncapsulator *pProtocolEncapsulator);
	CLMessagePoster(CLMessageSerializer *pCommonMsgSerializer, CLProtocolEncapsulator *pProtocolEncapsulator);
	virtual ~CLMessagePoster();

private:
	CLMessagePoster(const CLMessagePoster&);
	CLMessagePoster& operator=(const CLMessagePoster&);

private:
	CLMessageSerializer *m_pCommonSerializer;
	std::map<unsigned long, CLMessageSerializer*> m_SerializerTable;
	CLProtocolEncapsulator *m_pProtocolEncapsulator;
};

#endif