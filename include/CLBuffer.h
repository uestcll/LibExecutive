#ifndef CLBUFFER_H
#define CLBUFFER_H

#include "CLStatus.h"

class CLIOVector;

class CLBuffer
{
public:
	CLBuffer();
	virtual ~CLBuffer();

	CLStatus newBuffer(int size);
	
	const int& DataStartIndex();
	CLStatus DataStartIndex(const int& newIndex);

	const int& UsedBufferLen();
	CLStatus UsedBufferLen(const int& newUsedLen);

	char* GetDataPtr();
	char* GetDataPtr(int index);

	CLStatus GetBufPtr(char** pBuf, int restLen);
private:
	CLIOVector *m_pIOBufferVec;

	int m_iSumBufferLen;
	int m_iDataStartIndex;
	int m_iUsedBufferLen;
};

#endif