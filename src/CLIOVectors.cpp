#include "CLIOVectors.h"

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
		return CLStatus(-1, 0);

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
		return CLStatus(-1, 0);

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
		return CLStatus(-1, 0);
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
		return CLStatus(-1, 0);
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

	int position = 0;
	int i;
	for(i = 0; i < m_IOVectors.size(); i++)
	{
		position += m_IOVectors[i].IOVector.iov_len;
		if(index < position)
			break;
	}

	if(i == m_IOVectors.size())
		throw "In CLIOVectors::operator [], index is too large error";

	return *((char *)m_IOVectors[i].IOVector.iov_base + (index - (position - m_IOVectors[i].IOVector.iov_len)));
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

CLStatus CLIOVectors::WriteLong(unsigned int Index, long ulong)
{
	if(Index + sizeof(long) > Size())
		return CLStatus(-1, 0);

	char *p = (char *)(&ulong);

	char *pAddrIndex = 0;
	if(IsRangeInAIOVector(Index, sizeof(long), &pAddrIndex))
	{

	}
	else
	{
		for(int i = 0; i < sizeof(long); i++)
			(*this)[Index + i] = p[i];
	}


	

	unsigned long p1 = (unsigned long)(&((*this)[Index + sizeof(unsigned long)]));
	unsigned long p2 = (unsigned long)(&((*this)[Index]));

	

	if((p2 > p1) || ((p1 - p2) != sizeof(unsigned long)))
	{
		for(int i = 0; i < sizeof(long); i++)
			(*this)[Index + i] = p[i];
	}
	else
	{
		unsigned long *p3 = (unsigned long *)p2;
		*p3 = ulong;
	}

	return CLStatus(0, 0);
}

CLStatus CLIOVectors::WriteInt(unsigned int Index, int uint)
{

}

CLStatus CLIOVectors::WriteShort(unsigned int Index, short ushort)
{

}

bool CLIOVectors::IsRangeInAIOVector(unsigned int Index, unsigned int Length, char **ppAddrForIndex) const
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

	if(Index + Length > position)
		return false;
	else
		return true;
}