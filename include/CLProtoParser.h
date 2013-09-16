#ifndef CLPROTOPARSER_H
#define CLPROTOPARSER_H

class CLMessage;

typedef struct SLBufferForDeserializer
{
	long lMsgID;
	/* data */
}SLBufferForDeserializer;

class CLProtoParser
{
public:
	CLProtoParser();
	virtual ~CLProtoParser();

public:
	virtual int GetHeaderLength() = 0;
	virtual int GetContextLength() = 0;

	virtual CLMessage* EncapsulateMesage(const char* pHeader, int headerLen, const char* pContext, int ctxLen) = 0;

protected:
	int m_iHeaderLen;
	int m_iCtxLen;

	char* m_pHeader;
	char* m_pContext;

	CLMessage* m_pMsg;
};

#endif