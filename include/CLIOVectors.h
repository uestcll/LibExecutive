#ifndef CLIOVectors_H
#define CLIOVectors_H

#include <deque>
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

	CLStatus WriteLong(unsigned int Index, long ulong);
	CLStatus WriteInt(unsigned int Index, int uint);
	CLStatus WriteShort(unsigned int Index, short ushort);

	char& operator [](int index);
	const char& operator [](int index) const;

	size_t Size();
	int GetNumberOfIOVec();
	iovec *GetIOVecArray();

private:
	char& GetData(int index) const;
	bool IsRangeInAIOVector(unsigned int Index, unsigned int Length, char **ppAddrForIndex) const;

private:
	CLIOVectors(const CLIOVectors&);
	CLIOVectors& operator=(const CLIOVectors&);

private:
	std::deque<SLIOVectorItem> m_IOVectors;
	size_t m_nDataLength;

	bool m_bDestroyIOVecs;
};

#endif