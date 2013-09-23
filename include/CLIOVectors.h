#ifndef CLIOVectors_H
#define CLIOVectors_H

#include <list>
#include <sys/uio.h>
#include "CLStatus.h"

class CLIteratorForIOVectors;

struct SLIOVectorItem
{
	struct iovec IOVector;
	bool bDelete;
};

class CLIOVectors
{
	friend class CLIteratorForIOVectors;

public:
	CLIOVectors();
	explicit CLIOVectors(bool bDestroyIOVecs);
	virtual ~CLIOVectors();

	CLStatus PushBack(char *pBuffer, size_t nBufferLength, bool bDeleted = false);
	CLStatus PushFront(char *pBuffer, size_t nBufferLength, bool bDeleted = false);

	CLStatus PopBack(char **ppBuffer, size_t *pnBufferLength);
	CLStatus PopFront(char **ppBuffer, size_t *pnBufferLength);

	CLStatus GetIterator(unsigned int Index, CLIteratorForIOVectors& Iter);
	CLStatus WriteBlock(CLIteratorForIOVectors& Iter, char *pBuf, unsigned Length);
	CLStatus ReadBlock(CLIteratorForIOVectors& Iter, char *pBuf, unsigned Length);

	CLStatus WriteBlock(unsigned int Index, char *pBuf, unsigned int Length);
	CLStatus ReadBlock(unsigned int Index, char *pBuf, unsigned int Length);

	CLStatus PushBackRangeToAIOVector(CLIOVectors& IOVectors, unsigned int Index, unsigned int Length);

	CLStatus PushBackIOVector(CLIOVectors& IOVectors);

	size_t Size();
	int GetNumberOfIOVec();
	iovec *GetIOVecArray();

private:
	bool IsRangeInAIOVector(char *pAddr, unsigned int Length, std::list<SLIOVectorItem>::iterator& CurrentIter);
	void GetIndexPosition(unsigned int Index, char **ppAddrForIndex, std::list<SLIOVectorItem>::iterator *pIter);
	CLStatus TransferBlock(bool bWriteIntoIOVectors, char *pAddrInIOVector, std::list<SLIOVectorItem>::iterator& CurrentIter, char *pBuf, unsigned int Length, char **ppEndAddrInIOVector = 0);
	CLStatus TransferBlockByIndex(bool bWriteIntoIOVectors, unsigned int Index, char *pBuf, unsigned int Length);

private:
	CLIOVectors(const CLIOVectors&);
	CLIOVectors& operator=(const CLIOVectors&);

private:
	std::list<SLIOVectorItem> m_IOVectors;
	size_t m_nDataLength;
};

#endif