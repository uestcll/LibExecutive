#ifndef CLIOVECTOR_H
#define CLIOVECTOR_H

#include <deque>
#include <sys/uio.h>
#include "CLLogger.h"
#include "CLStatus.h"

//实现一个逻辑连续的缓冲区
//是否要引入引用计数
class CLIOVector
{
public:
	CLIOVector();
	virtual ~CLIOVector();

	CLStatus PushBack(char* pBuffer, int bufLen);
	CLStatus PushFront(char* pBuffer, int bufLen);

	CLStatus PopBack(char** pBuffer, int* bufLen);
	CLStatus PopFront(char** pBuffer, int* bufLen);

	CLStatus FreeAll();
	CLStatus PopAll();

	CLStatus GetIOVecs(int index, int len, CLIOVector& IOVector);
	
	CLStatus PushIOVecs(int index, CLIOVector& IOVector);
	
	int GetBufPtr(int index, char** pBufffer); //返回值为index开始的连续空间的长度 index是从0开始的。

	CLStatus WriteData(char* pBuffer, const int& index, const int& len);
	CLStatus ReadData(char* pBuffer, const int& index, const int& len);

	int IOVecNum();//iovec item num
	int Length();//all buffer Length

	const char& operator[](const int &index);

private:
	int m_iDataLength;
	std::deque<struct iovec> m_ioVecQueue;
};

#endif