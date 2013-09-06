#ifndef CLDataReader_H
#define CLDataReader_H

#define READ_END -1
#define READ_ERROR -2

class CLProtoParser;
class CLMessageReceiver;

class CLDataReader
{
public:
	CLDataReader(int fd, CLProtoParser *pProtoParser, CLMessageReceiver pMsgReceiver);
	virtual ~CLDataReader();

	virtual int ReadData();

private:
	CLDataReader(const CLDataReader&);
	CLDataReader& operator=(cosnt CLDataReader&);

	CLStatus ReadHeader(int fd);
	CLStatus ReadContext(int fd);

protected:
	int m_fd;
	CLProtoParser *m_pProtoParser;
	CLMessageReceiver *m_pMsgReceiver;

	bool m_bNewData;
	bool m_bReadHead;

	char* m_pHeaderBuffer;
	char* m_pCtxBuffer;

	int  m_iHeadLength;
	int  m_iCtxLength;
	int  m_iReadOffset;

};

#endif