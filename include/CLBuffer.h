#ifndef CLBUFFER_H
#define CLBUFFER_H

#include "CLStatus.h"

class CLIOVector;
//提供接口 得到从哪里开始 多长的字符串。如果是连续的，就返回指针。不是就拷贝一份成为连续的。
class CLBuffer
{
public:
	CLBuffer();
	virtual ~CLBuffer();

	CLStatus newBuffer(int size);
	
	const int& DataStartIndex();
	CLStatus DataStartIndex(const int& newIndex);

	const int& UsedBufferLen();
	CLStatus AddUsedBufferLen(const int& addUsedLen);

	char* GetDataPtr();
	CLIOVector* GetDataPtr(const int& index, const int& len);

	CLStatus GetRestBufPtr(char** pBuf, int& restLen); //得到空闲buffer的头指针，并得到可以存储数据的空闲buffer长度
private:
	CLIOVector *m_pIOBufferVec;

	int m_iSumBufferLen;
	int m_iDataStartIndex;
	int m_iUsedBufferLen;
};

#endif