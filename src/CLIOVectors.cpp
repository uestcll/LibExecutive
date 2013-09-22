#include <memory.h>
#include "CLIOVectors.h"
#include "ErrorCode.h"

using namespace std;

CLIOVectors::CLIOVectors()
{
	m_nDataLength = 0;
}

CLIOVectors::~CLIOVectors()
{
	list<SLIOVectorItem>::iterator iter = m_IOVectors.begin();
	for(; iter != m_IOVectors.end(); iter++)
	{
		if(iter->bDelete)
			delete [] (char *)iter->IOVector.iov_base;
	}
}

CLStatus CLIOVectors::PushBack(char *pBuffer, size_t nBufferLength, bool bDeleted)
{
	if((pBuffer == 0) || (nBufferLength == 0))
		return CLStatus(-1, NORMAL_ERROR);

	SLIOVectorItem item;
	item.IOVector.iov_base = pBuffer;
	item.IOVector.iov_len = nBufferLength;
	item.bDelete = bDeleted;

	m_IOVectors.push_back(item);

	m_nDataLength += nBufferLength;

	return CLStatus(0, 0);
}

CLStatus CLIOVectors::PushFront(char *pBuffer, size_t nBufferLength, bool bDeleted)
{
	if((pBuffer == 0) || (nBufferLength == 0))
		return CLStatus(-1, NORMAL_ERROR);

	SLIOVectorItem item;
	item.IOVector.iov_base = pBuffer;
	item.IOVector.iov_len = nBufferLength;
	item.bDelete = bDeleted;

	m_IOVectors.push_front(item);

	m_nDataLength += nBufferLength;

	return CLStatus(0, 0);
}

CLStatus CLIOVectors::PopBack(char **ppBuffer, size_t *pnBufferLength)
{
	if(m_IOVectors.empty())
	{
		*ppBuffer = 0;
		*pnBufferLength = 0;
		return CLStatus(-1, NORMAL_ERROR);
	}

	SLIOVectorItem item = m_IOVectors.back();
	*ppBuffer = (char *)item.IOVector.iov_base;
	*pnBufferLength = item.IOVector.iov_len;
	
	m_IOVectors.pop_back();

	m_nDataLength -= item.IOVector.iov_len;

	return CLStatus(0, 0);
}

CLStatus CLIOVectors::PopFront(char **ppBuffer, size_t *pnBufferLength)
{
	if(m_IOVectors.empty())
	{
		*ppBuffer = 0;
		*pnBufferLength = 0;
		return CLStatus(-1, NORMAL_ERROR);
	}

	SLIOVectorItem item = m_IOVectors.front();
	*ppBuffer = (char *)item.IOVector.iov_base;
	*pnBufferLength = item.IOVector.iov_len;

	m_IOVectors.pop_front();

	m_nDataLength -= item.IOVector.iov_len;

	return CLStatus(0, 0);
}

CLStatus CLIOVectors::WriteBlock(unsigned int Index, char *pBuf, unsigned int Length)
{
	return TransferBlock(true, Index, pBuf, Length);
}

CLStatus CLIOVectors::ReadBlock(unsigned int Index, char *pBuf, unsigned int Length)
{
	return TransferBlock(false, Index, pBuf, Length);
}

CLStatus CLIOVectors::PushBackRangeToAIOVector(CLIOVectors& IOVectors, unsigned int Index, unsigned int Length)
{
	if((Index + Length) > m_nDataLength)
		return CLStatus(-1, 0);

	char *pAddrIndex = 0;
	list<SLIOVectorItem>::iterator it = m_IOVectors.end();
	if(IsRangeInAIOVector(Index, Length, &pAddrIndex, &it))
		return IOVectors.PushBack(pAddrIndex, Length, false);

	unsigned long offset = (unsigned long)pAddrIndex - (unsigned long)(it->IOVector.iov_base);
	unsigned long leftroom = it->IOVector.iov_len - offset;
	CLStatus s1 = IOVectors.PushBack(pAddrIndex, leftroom, false);
	if(!s1.IsSuccess())
		return s1;

	Length = Length - leftroom;

	for(int i = 0; Length != 0; i++)
	{
		it++;
		int room = it->IOVector.iov_len;
		if(Length <= room)
			return IOVectors.PushBack((char *)it->IOVector.iov_base, Length, false);

		CLStatus s2 = IOVectors.PushBack((char *)it->IOVector.iov_base, room, false);
		if(!s2.IsSuccess())
			return s2;

		Length = Length - room;
	}

	return CLStatus(-1, NORMAL_ERROR);
}

size_t CLIOVectors::Size()
{
	return m_nDataLength;
}

int CLIOVectors::GetNumberOfIOVec()
{
	return m_IOVectors.size();
}

iovec *CLIOVectors::GetIOVecArray()
{
	if(m_IOVectors.empty() || m_nDataLength == 0)
		return 0;

	int n = m_IOVectors.size();
	iovec *pArray = new iovec[n];

	list<SLIOVectorItem>::iterator iter = m_IOVectors.begin();
	for(int i = 0; iter != m_IOVectors.end(); iter++, i++)
	{
		pArray[i].iov_base = (char *)iter->IOVector.iov_base;
		pArray[i].iov_len = iter->IOVector.iov_len;
	}

	return pArray;
}

bool CLIOVectors::IsRangeInAIOVector(unsigned int Index, unsigned int Length, char **ppAddrForIndex, std::list<SLIOVectorItem>::iterator *pIter)
{
	int position = 0;
	list<SLIOVectorItem>::iterator iter = m_IOVectors.begin();
	for(; iter != m_IOVectors.end(); iter++)
	{
		position += iter->IOVector.iov_len;
		if(Index < position)
			break;
	}

	*ppAddrForIndex = (char *)iter->IOVector.iov_base + (Index - (position - iter->IOVector.iov_len));

	if(pIter != 0)
		*pIter = iter;

	if(Index + Length > position)
		return false;
	else
		return true;
}

CLStatus CLIOVectors::TransferBlock(bool bWriteIntoIOVectors, unsigned int Index, char *pBuf, unsigned int Length)
{
	if(Index + Length > m_nDataLength)
		return CLStatus(-1, NORMAL_ERROR);

	char *pAddrIndex = 0;
	list<SLIOVectorItem>::iterator it;
	if(IsRangeInAIOVector(Index, Length, &pAddrIndex, &it))
	{
		if(bWriteIntoIOVectors)
			memcpy(pAddrIndex, pBuf, Length);
		else
			memcpy(pBuf, pAddrIndex, Length);

		return CLStatus(0, 0);
	}

	unsigned long offset = (unsigned long)pAddrIndex - (unsigned long)(it->IOVector.iov_base);
	unsigned long leftroom = it->IOVector.iov_len - offset;

	if(bWriteIntoIOVectors)
		memcpy(pAddrIndex, pBuf, leftroom);
	else
		memcpy(pBuf, pAddrIndex, leftroom);

	Length = Length - leftroom;
	pBuf = pBuf + leftroom;

	for(int i = 0; Length != 0; i++)
	{
		it++;
		int room = it->IOVector.iov_len;
		if(Length <= room)
		{
			if(bWriteIntoIOVectors)
				memcpy((char *)it->IOVector.iov_base, pBuf, Length);
			else
				memcpy(pBuf, (char *)it->IOVector.iov_base, Length);

			return CLStatus(0, 0);
		}

		if(bWriteIntoIOVectors)
			memcpy((char *)it->IOVector.iov_base, pBuf, room);
		else
			memcpy(pBuf, (char *)it->IOVector.iov_base, room);

		Length = Length - room;
		pBuf = pBuf + room;
	}

	return CLStatus(-1, NORMAL_ERROR);
}