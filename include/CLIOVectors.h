#ifndef CLIOVectors_H
#define CLIOVectors_H

#include <list>
#include <sys/uio.h>
#include "CLStatus.h"

struct SLIOVectorItem
{
	struct iovec IOVector;
	bool bDelete;
};

class CLIOVectors
{
public:
	CLIOVectors();
	explicit CLIOVectors(bool bDestroyIOVecs);
	virtual ~CLIOVectors();

	CLStatus PushBack(char *pBuffer, size_t nBufferLength, bool bDeleted = false);
	CLStatus PushFront(char *pBuffer, size_t nBufferLength, bool bDeleted = false);

	CLStatus PopBack(char **ppBuffer, size_t *pnBufferLength);
	CLStatus PopFront(char **ppBuffer, size_t *pnBufferLength);

	CLStatus WriteBlock(unsigned int Index, char *pBuf, unsigned int Length);
	CLStatus ReadBlock(unsigned int Index, char *pBuf, unsigned int Length);

	CLStatus PushBackRangeToAIOVector(CLIOVectors& IOVectors, unsigned int Index, unsigned int Length);

	size_t Size();
	int GetNumberOfIOVec();
	iovec *GetIOVecArray();

private:
	bool IsRangeInAIOVector(unsigned int Index, unsigned int Length, char **ppAddrForIndex, std::list<SLIOVectorItem>::iterator *pIter = 0);
	CLStatus TransferBlock(bool bWriteIntoIOVectors, unsigned int Index, char *pBuf, unsigned int Length);

private:
	CLIOVectors(const CLIOVectors&);
	CLIOVectors& operator=(const CLIOVectors&);

private:
	std::list<SLIOVectorItem> m_IOVectors;
	size_t m_nDataLength;
};

#endif