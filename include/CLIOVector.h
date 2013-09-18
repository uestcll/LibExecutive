#ifndef CLIOVECTOR_H
#define CLIOVECTOR_H

#include <vector>
#include <sys/uio.h>
#include "CLLog.h"
#include "CLStatus.h"

//实现一个环形的缓冲区
class CLIOVector
{
public:
	CLIOVector();
	virtual ~CLIOVector();

	CLStatus PushBack(char* pBuffer, int bufLen);
	CLStatus PushFront(char* pBuffer, int bufLen);

	CLStatus PopBack(char** pBuffer, int* bufLen);
	CLStatus PopFront(char** pBuffer, int* bufLen);

	int IOVecNum();
	int Size();

private:
	char
private:
	std::vector<struct iovec> m_vecIOBuf;
};

#endif