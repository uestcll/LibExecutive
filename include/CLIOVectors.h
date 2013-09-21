#ifndef CLIOVectors_H
#define CLIOVectors_H

#include <list>
#include <sys/uio.h>
#include "CLStatus.h"

struct SLIOVectorItem
{
	struct iovec IOVector;
	unsigned int ValidBeginIndex;
	unsigned int ValidLength;
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

	CLStatus WriteLong(unsigned int Index, long data);
	CLStatus WriteInt(unsigned int Index, int data);
	CLStatus WriteShort(unsigned int Index, short data);

	CLStatus ReadLong(unsigned int Index, long& data);
	CLStatus ReadInt(unsigned int Index, int& data);
	CLStatus ReadShort(unsigned int Index, short& data);

	CLStatus PushBackRangeToAIOVector(CLIOVectors& IOVectors, unsigned int Index, unsigned int Length);

	char& operator [](int index);
	const char& operator [](int index) const;

	size_t Size();
	int GetNumberOfIOVec();
	iovec *GetIOVecArray(unsigned int &NumberOfValidIOVec);

private:
	char& GetData(int index) const;
	bool IsRangeInAIOVector(unsigned int Index, unsigned int Length, char **ppAddrForIndex, std::list<SLIOVectorItem>::iterator *pIter = 0) const;

	template<typename BasicType>
	CLStatus WriteBasicTypeDataToIOVectors(unsigned int Index, BasicType data);

	template<typename BasicType>
	CLStatus ReadBasicTypeDataFromIOVectors(unsigned int Index, BasicType& data);

private:
	CLIOVectors(const CLIOVectors&);
	CLIOVectors& operator=(const CLIOVectors&);

private:
	std::list<SLIOVectorItem> m_IOVectors;
	size_t m_nDataLength;
	size_t m_nValidDataLength;
};

#endif