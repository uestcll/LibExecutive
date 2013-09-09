#ifndef CLIOVectors_H
#define CLIOVectors_H

#include <deque>
#include <sys/uio.h>
#include "CLStatus.h"

class CLIOVectors
{
public:
	CLIOVectors();
	explicit CLIOVectors(bool bDestroyIOVecs);
	virtual ~CLIOVectors();

	CLStatus PushBack(char *pBuffer, size_t nBufferLength);
	CLStatus PushFront(char *pBuffer, size_t nBufferLength);

	CLStatus PopBack(char **ppBuffer, size_t *pnBufferLength);
	CLStatus PopFront(char **ppBuffer, size_t *pnBufferLength);

	char& operator [](int index);
	const char& operator [](int index) const;

	size_t Size();
	int GetNumberOfIOVec();
	iovec *GetIOVecArray();

private:
	char& GetData(int index) const;

private:
	CLIOVectors(const CLIOVectors&);
	CLIOVectors& operator=(const CLIOVectors&);

private:
	std::deque<struct iovec> m_IOVectors;
	size_t m_nDataLength;

	bool m_bDestroyIOVecs;
};

#endif