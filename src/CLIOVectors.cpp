#include "CLIOVectors.h"
#include "ErrorCode.h"

CLIOVectors::CLIOVectors()
{
	m_nDataLength = 0;
	m_bDestroyIOVecs = false;
}

CLIOVectors::CLIOVectors(bool bDestroyIOVecs)
{
	m_nDataLength = 0;
	m_bDestroyIOVecs = bDestroyIOVecs;
}

CLIOVectors::~CLIOVectors()
{
	if(m_bDestroyIOVecs)
	{
		for(int i = 0; i < m_IOVectors.size(); i++)
		{
			if(m_IOVectors[i].bDelete)
				delete [] ((char *)m_IOVectors[i].IOVector.iov_base);
		}
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

	m_nDataLength -= *pnBufferLength;

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

	m_nDataLength -= *pnBufferLength;

	return CLStatus(0, 0);
}

char& CLIOVectors::operator [](int index)
{
	return GetData(index);
}

const char& CLIOVectors::operator [](int index) const
{
	return GetData(index);
}

char& CLIOVectors::GetData(int index) const
{
	if(m_IOVectors.empty())
		throw "In CLIOVectors::operator [], m_IOVectors is empty error";

	if(index >= m_nDataLength)
		throw "In CLIOVectors::operator [], index >= m_nDataLength error";

	if(index < 0)
		throw "In CLIOVectors::operator [], index < 0 error";

	char *pAddrIndex = 0;
	IsRangeInAIOVector(index, 1, &pAddrIndex);

	return *pAddrIndex;
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

	for(int i = 0; i < n; i++)
	{
		pArray[i].iov_base = m_IOVectors[i].IOVector.iov_base;
		pArray[i].iov_len = m_IOVectors[i].IOVector.iov_len;
	}

	return pArray;
}

CLStatus CLIOVectors::WriteLong(unsigned int Index, long data)
{
	return WriteBasicTypeDataToIOVectors<long>(Index, data);
}

CLStatus CLIOVectors::WriteInt(unsigned int Index, int data)
{
	return WriteBasicTypeDataToIOVectors<int>(Index, data);
}

CLStatus CLIOVectors::WriteShort(unsigned int Index, short data)
{
	return WriteBasicTypeDataToIOVectors<short>(Index, data);
}

bool CLIOVectors::IsRangeInAIOVector(unsigned int Index, unsigned int Length, char **ppAddrForIndex, unsigned int *pIOV_Index) const
{
	int position = 0;
	int i;
	for(i = 0; i < m_IOVectors.size(); i++)
	{
		position += m_IOVectors[i].IOVector.iov_len;
		if(Index < position)
			break;
	}

	*ppAddrForIndex = (char *)m_IOVectors[i].IOVector.iov_base + (Index - (position - m_IOVectors[i].IOVector.iov_len));

	if(pIOV_Index != 0)
		*pIOV_Index = i;

	if(Index + Length > position)
		return false;
	else
		return true;
}

template<typename BasicType>
CLStatus CLIOVectors::WriteBasicTypeDataToIOVectors(unsigned int Index, BasicType data)
{
	if(Index + sizeof(BasicType) > m_nDataLength)
		return CLStatus(-1, NORMAL_ERROR);

	char *pAddrIndex = 0;
	if(IsRangeInAIOVector(Index, sizeof(BasicType), &pAddrIndex))
	{
		*((BasicType *)(pAddrIndex)) = data;
	}
	else
	{
		char *p = (char *)(&data);

		for(int i = 0; i < sizeof(BasicType); i++)
			(*this)[Index + i] = p[i];
	}

	return CLStatus(0, 0);
}

CLStatus CLIOVectors::ReadLong(unsigned int Index, long& data)
{
	return ReadBasicTypeDataFromIOVectors<long>(Index, data);
}

CLStatus CLIOVectors::ReadInt(unsigned int Index, int& data)
{
	return ReadBasicTypeDataFromIOVectors<int>(Index, data);
}

CLStatus CLIOVectors::ReadShort(unsigned int Index, short& data)
{
	return ReadBasicTypeDataFromIOVectors<short>(Index, data);
}

template<typename BasicType>
CLStatus CLIOVectors::ReadBasicTypeDataFromIOVectors(unsigned int Index, BasicType& data)
{
	if(Index + sizeof(BasicType) > m_nDataLength)
		return CLStatus(-1, NORMAL_ERROR);

	char *pAddrIndex = 0;
	if(IsRangeInAIOVector(Index, sizeof(BasicType), &pAddrIndex))
	{
		data = *((BasicType *)(pAddrIndex));
	}
	else
	{
		char *p = (char *)(&data);

		for(int i = 0; i < sizeof(BasicType); i++)
			p[i] = (*this)[Index + i];
	}

	return CLStatus(0, 0);
}

CLStatus CLIOVectors::PushBackRangeToAIOVector(CLIOVectors& IOVectors, unsigned int Index, unsigned int Length)
{
	if((Index + Length) > m_nDataLength)
		return CLStatus(-1, 0);

	char *pAddrIndex = 0;
	unsigned int iov_index = 0;
	if(IsRangeInAIOVector(Index, Length, &pAddrIndex, &iov_index))
		return IOVectors.PushBack(pAddrIndex, Length, false);

	unsigned long offset = pAddrIndex - (char *)(m_IOVectors[iov_index].IOVector.iov_base);
	unsigned long leftroom = m_IOVectors[iov_index].IOVector.iov_len - offset;
	CLStatus s1 = IOVectors.PushBack(pAddrIndex, leftroom, false);
	if(!s1.IsSuccess())
		return s1;

	Length = Length - leftroom;

	for(int i = 0; Length != 0; i++)
	{
		int room = m_IOVectors[iov_index + i].IOVector.iov_len;

		if(Length <= room)
			return IOVectors.PushBack((char *)(m_IOVectors[iov_index + i].IOVector.iov_base), Length, false);

		CLStatus s = IOVectors.PushBack((char *)(m_IOVectors[iov_index + i].IOVector.iov_base), room, false);
		if(!s.IsSuccess())
			return s;

		Length = Length - room;
	}

	return CLStatus(-1, NORMAL_ERROR);
}