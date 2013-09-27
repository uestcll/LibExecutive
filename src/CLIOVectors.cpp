#include <memory.h>
#include <vector>
#include "CLIOVectors.h"
#include "ErrorCode.h"
#include "CLIteratorForIOVectors.h"
#include "CLLogger.h"

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
	{
		CLLogger::WriteLogMsg("In CLIOVectors::PushBack(), pBuffer == 0 or nBufferLength == 0", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

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
	{
		CLLogger::WriteLogMsg("In CLIOVectors::PushFront(), pBuffer == 0 or nBufferLength == 0", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

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

		CLLogger::WriteLogMsg("In CLIOVectors::PopBack(), m_IOVectors.empty", 0);
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

		CLLogger::WriteLogMsg("In CLIOVectors::PopFront(), m_IOVectors.empty", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	SLIOVectorItem item = m_IOVectors.front();
	*ppBuffer = (char *)item.IOVector.iov_base;
	*pnBufferLength = item.IOVector.iov_len;

	m_IOVectors.pop_front();

	m_nDataLength -= item.IOVector.iov_len;

	return CLStatus(0, 0);
}

CLStatus CLIOVectors::GetIterator(unsigned int Index, CLIteratorForIOVectors& Iter)
{
	if(Index >= m_nDataLength)
	{
		CLLogger::WriteLogMsg("In CLIOVectors::GetIterator(), Index >= m_nDataLength", 0);
		return CLStatus(-1, 0);
	}

	GetIndexPosition(Index, &(Iter.m_pData), &(Iter.m_Iter));
	Iter.m_pIOVectors = &m_IOVectors;

	return CLStatus(0, 0);
}

CLStatus CLIOVectors::WriteBlock(CLIteratorForIOVectors& Iter, char *pBuf, unsigned Length)
{
	return TransferBlockByIterator(true, Iter, pBuf, Length);
}

CLStatus CLIOVectors::ReadBlock(CLIteratorForIOVectors& Iter, char *pBuf, unsigned Length)
{
	return TransferBlockByIterator(false, Iter, pBuf, Length);
}

CLStatus CLIOVectors::WriteBlock(unsigned int Index, char *pBuf, unsigned int Length)
{
	return TransferBlockByIndex(true, Index, pBuf, Length);
}

CLStatus CLIOVectors::ReadBlock(unsigned int Index, char *pBuf, unsigned int Length)
{
	return TransferBlockByIndex(false, Index, pBuf, Length);
}

CLStatus CLIOVectors::PushBackRangeToAIOVector(CLIOVectors& IOVectors, unsigned int Index, unsigned int Length)
{
	if((Index + Length) > m_nDataLength)
	{
		CLLogger::WriteLogMsg("In CLIOVectors::PushBackRangeToAIOVector(), (Index + Length) > m_nDataLength", 0);
		return CLStatus(-1, 0);
	}

	char *pAddrIndex = 0;
	list<SLIOVectorItem>::iterator it;
	GetIndexPosition(Index, &pAddrIndex, &it);

	if(IsRangeInAIOVector(pAddrIndex, Length, it))
		return IOVectors.PushBack(pAddrIndex, Length, false);

	unsigned long offset = (unsigned long)pAddrIndex - (unsigned long)(it->IOVector.iov_base);
	unsigned long leftroom = it->IOVector.iov_len - offset;
	CLStatus s1 = IOVectors.PushBack(pAddrIndex, leftroom, false);
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLIOVectors::PushBackRangeToAIOVector(), IOVectors.PushBack 1 error", 0);
		return s1;
	}

	Length = Length - leftroom;

	while(true)
	{
		it++;
		int room = it->IOVector.iov_len;
		if(Length <= room)
			return IOVectors.PushBack((char *)it->IOVector.iov_base, Length, false);

		CLStatus s2 = IOVectors.PushBack((char *)it->IOVector.iov_base, room, false);
		if(!s2.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLIOVectors::PushBackRangeToAIOVector(), IOVectors.PushBack 2 error", 0);
			return s2;
		}

		Length = Length - room;
	}
}

CLStatus CLIOVectors::PushBackIOVector(CLIOVectors& IOVectors)
{
	list<SLIOVectorItem>::iterator it = IOVectors.m_IOVectors.begin();
	for(; it != IOVectors.m_IOVectors.end(); it++)
	{
		m_IOVectors.push_back(*it);
	}

	return CLStatus(0, 0);
}

CLStatus CLIOVectors::DifferenceBetweenIOVectors(CLIOVectors& Operand, CLIOVectors& Difference)
{
	vector<iovec> vTmp;

	list<SLIOVectorItem>::iterator iter = m_IOVectors.begin();
	for(; iter != m_IOVectors.end(); iter++)
	{
		DifferenceBetweenRangeAndIOVector(iter->IOVector, Operand, vTmp);
		
		for(int i = 0; i < vTmp.size(); i++)
			Difference.PushBack((char *)(vTmp[i].iov_base), vTmp[i].iov_len, false);

		vTmp.clear();
	}

	return CLStatus(0, 0);
}

bool CLIOVectors::IsRangeOverlap(iovec& Range)
{
	list<SLIOVectorItem>::iterator it = m_IOVectors.begin();
	for(; it != m_IOVectors.end(); it++)
	{
		unsigned long s1 = (unsigned long)it->IOVector.iov_base;
		unsigned long e1 = s1 + it->IOVector.iov_len - 1;
		unsigned long s2 = (unsigned long)Range.iov_base;
		unsigned long e2 = s2 + Range.iov_len - 1;

		if((e1 < s2) || (e2 < s1))
			continue;
		else
			return true;
	}

	return false;
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
	{
		CLLogger::WriteLogMsg("In CLIOVectors::GetIOVecArray(), m_IOVectors.empty() or m_nDataLength == 0", 0);
		return 0;
	}

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

bool CLIOVectors::IsRangeInAIOVector(char *pAddr, unsigned int Length, std::list<SLIOVectorItem>::iterator& CurrentIter)
{
	unsigned long offset = (unsigned long)pAddr - (unsigned long)(CurrentIter->IOVector.iov_base);
	unsigned long leftroom = CurrentIter->IOVector.iov_len - offset;
	
	if(Length <= leftroom)
		return true;
	else
		return false;
}

void CLIOVectors::GetIndexPosition(unsigned int Index, char **ppAddrForIndex, std::list<SLIOVectorItem>::iterator *pIter)
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

	*pIter = iter;
}

CLStatus CLIOVectors::TransferBlock(bool bWriteIntoIOVectors, char *pAddrInIOVector, std::list<SLIOVectorItem>::iterator& CurrentIter, char *pBuf, unsigned int Length, char **ppEndAddrInIOVector)
{
	if(IsRangeInAIOVector(pAddrInIOVector, Length, CurrentIter))
	{
		if(bWriteIntoIOVectors)
			memcpy(pAddrInIOVector, pBuf, Length);
		else
			memcpy(pBuf, pAddrInIOVector, Length);

		if(ppEndAddrInIOVector != 0)
		{
			if((pAddrInIOVector + Length - (char *)(CurrentIter->IOVector.iov_base)) == CurrentIter->IOVector.iov_len)
			{
				CurrentIter++;
				if(CurrentIter != m_IOVectors.end())
					*ppEndAddrInIOVector = (char *)(CurrentIter->IOVector.iov_base);
				else
					*ppEndAddrInIOVector = 0;
			}
			else
				*ppEndAddrInIOVector = pAddrInIOVector + Length;
		}

		return CLStatus(Length, 0);
	}

	unsigned int OriginalLength = Length;

	unsigned long offset = (unsigned long)pAddrInIOVector - (unsigned long)(CurrentIter->IOVector.iov_base);
	unsigned long leftroom = CurrentIter->IOVector.iov_len - offset;

	if(bWriteIntoIOVectors)
		memcpy(pAddrInIOVector, pBuf, leftroom);
	else
		memcpy(pBuf, pAddrInIOVector, leftroom);

	Length = Length - leftroom;
	pBuf = pBuf + leftroom;

	while(true)
	{
		CurrentIter++;
		if(CurrentIter == m_IOVectors.end())
		{
			if(ppEndAddrInIOVector != 0)
				*ppEndAddrInIOVector = 0;

			return CLStatus(OriginalLength - Length, 0);
		}

		int room = CurrentIter->IOVector.iov_len;
		if(Length <= room)
		{
			if(bWriteIntoIOVectors)
				memcpy((char *)CurrentIter->IOVector.iov_base, pBuf, Length);
			else
				memcpy(pBuf, (char *)CurrentIter->IOVector.iov_base, Length);

			if(ppEndAddrInIOVector != 0)
			{
				if(Length == room)
				{
					CurrentIter++;
					if(CurrentIter != m_IOVectors.end())
						*ppEndAddrInIOVector = (char *)CurrentIter->IOVector.iov_base;
					else
						*ppEndAddrInIOVector = 0;
				}
				else
					*ppEndAddrInIOVector = (char *)CurrentIter->IOVector.iov_base + Length;
			}

			return CLStatus(OriginalLength, 0);
		}

		if(bWriteIntoIOVectors)
			memcpy((char *)CurrentIter->IOVector.iov_base, pBuf, room);
		else
			memcpy(pBuf, (char *)CurrentIter->IOVector.iov_base, room);

		Length = Length - room;
		pBuf = pBuf + room;
	}
}

CLStatus CLIOVectors::TransferBlockByIndex(bool bWriteIntoIOVectors, unsigned int Index, char *pBuf, unsigned int Length)
{
	if(Index >= m_nDataLength)
	{
		CLLogger::WriteLogMsg("In CLIOVectors::TransferBlockByIndex(), Index >= m_nDataLength error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	char *pAddrInIOVecotr;
	list<SLIOVectorItem>::iterator it;
	GetIndexPosition(Index, &pAddrInIOVecotr, &it);

	return TransferBlock(bWriteIntoIOVectors, pAddrInIOVecotr, it, pBuf, Length);
}

CLStatus CLIOVectors::TransferBlockByIterator(bool bWriteIntoIOVectors, CLIteratorForIOVectors& Iter, char *pBuf, unsigned Length)
{
	if((Iter.m_pData == 0) || 
		(Iter.m_Iter == m_IOVectors.end()) || 
		(pBuf == 0) || 
		(Length == 0))
	{
		CLLogger::WriteLogMsg("In CLIOVectors::TransferBlockByIterator(), Iter.m_pData == 0 and so on", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	char *pEndData = 0;
	CLStatus s = TransferBlock(bWriteIntoIOVectors, Iter.m_pData, Iter.m_Iter, pBuf, Length, &pEndData);
	if(s.IsSuccess())
	{
		Iter.m_pData = pEndData;
		return s;
	}
	else
	{
		CLLogger::WriteLogMsg("In CLIOVectors::TransferBlockByIterator(), TransferBlock error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}
}

void CLIOVectors::DifferenceBetweenRanges(iovec& Range1, iovec& Range2, std::vector<iovec>& vResults)
{
	unsigned long s1 = (unsigned long)Range1.iov_base;
	unsigned long e1 = s1 + Range1.iov_len - 1;

	unsigned long s2 = (unsigned long)Range2.iov_base;
	unsigned long e2 = s2 + Range2.iov_len - 1;

	if((e1 < s2) || (e2 < s1))
	{
		vResults.push_back(Range1);
		return;
	}

	if((s1 < s2) && (s2 <= e1) && (e1 <= e2))
	{
		iovec tmp;
		tmp.iov_base = (void *)s1;
		tmp.iov_len = s2 - s1;
		vResults.push_back(tmp);
		return;
	}

	if((e2 < e1) && (e2 >= s1) && (s2 <= s1))
	{
		iovec tmp;
		tmp.iov_base = (void *)(e2 + 1);
		tmp.iov_len = e1 - e2;
		vResults.push_back(tmp);
		return;
	}

	if((s1 >= s2) && (e1 <= e2))
		return;

	if((s1 < s2) && (e2 < e1))
	{
		iovec tmp;
		tmp.iov_base = (void *)s1;
		tmp.iov_len = s2 - s1;
		vResults.push_back(tmp);

		tmp.iov_base = (void *)(e2 + 1);
		tmp.iov_len = e1 - e2;
		vResults.push_back(tmp);
		return;
	}

	return;
}

void CLIOVectors::DifferenceBetweenRangeAndIOVector(iovec& Range, CLIOVectors& IOVector, std::vector<iovec>& vResult)
{
	vector<iovec> *pvtest = new vector<iovec>;
	pvtest->push_back(Range);

	vector<iovec> *pTmp = new vector<iovec>;

	list<SLIOVectorItem>::iterator iter = IOVector.m_IOVectors.begin();
	for(; iter != IOVector.m_IOVectors.end(); iter++)
	{
		for(int i = 0; i < pvtest->size(); i++)
			DifferenceBetweenRanges((*pvtest)[i], iter->IOVector, *pTmp);

		vector<iovec> *p = pvtest;
		pvtest = pTmp;
		pTmp = p;

		pTmp->clear();
	}

	vResult = *pvtest;

	delete pTmp;
	delete pvtest;
}