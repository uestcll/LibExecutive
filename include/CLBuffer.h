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
	int DataStartIndex();
	CLStatus DataStartIndex(int newIndex);

	
	char* GetBufPtr(int index);
private:
	CLIOVector *m_pIOBufferVec;

	int sumBufferLen;
	int dataStartIndex;
	int usedBufferLen;
};

#endif