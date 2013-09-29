#ifndef CLBUFFER_H
#define CLBUFFER_H

#include "CLStatus.h"
#define DEAFULT_BUFFER_LENGTH 4096

class CLIOVector;
//提供接口 得到从哪里开始 多长的字符串。如果是连续的，就返回指针。不是就拷贝一份成为连续的。
class CLBuffer
{
public:
	CLBuffer(int itemSize = DEAFULT_BUFFER_LENGTH);
	virtual ~CLBuffer();

	CLStatus newBuffer(int size);
	
	const int& DataStartIndex() const;
	CLStatus DataStartIndex(const int& newIndex);

	const int& UsedBufferLen() const;
	CLStatus AddUsedBufferLen(const int& addUsedLen);

	char* GetDataPtr();
	//CLIOVector* GetDataPtr(const int& index, const int& len);
	CLStatus ReadData(char* &pBuffer, const int& index, const int& len);

	CLStatus GetRestBufPtr(char** pBuf, int& restLen); //得到空闲buffer的头指针，并得到可以存储数据的空闲buffer长度

public:
	const int& m_ciDataStartIndex;
	const int& m_ciUsedBufferLen;

private:
	CLIOVector *m_pIOBufferVec;

	int m_iItemSize;
	int m_iSumBufferLen;
	int m_iDataStartIndex;
	int m_iUsedBufferLen;
};

#endif