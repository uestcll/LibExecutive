#include "CLIOVector.h"

using namespace std;

CLIOVector::CLIOVector()
{
	m_iDataLength;
}

CLIOVector::~CLIOVector()
{
	/*while(1)
	{
		if(m_ioVecQueue.empty())
		{
			break;
		}
		else
		{
			struct iovec tmp = m_ioVecQueue.back();
			delete [] tmp.iov_base;
			m_ioVecQueue.pop_back();
		}
	}*/		
}

CLStatus CLIOVector::PushBack(char* pBuffer, int bufLen)
{	
	if((pBuffer == 0) || (bufLen <= 0))
	{
		CLLogger::WriteLogMsg("In CLIOVector::PushBack(), pBuffer or bufLen is 0", 0);
		return CLStatus(-1, 0);
	}
	struct iovec item;
	item.iov_base = pBuffer;
	item.iov_len = bufLen;

	m_ioVecQueue.push_back(item);

	m_iDataLength += bufLen;

	return CLStatus(0, 0);
}

CLStatus CLIOVector::PushFront(char* pBuffer, int bufLen)
{
	if((pBuffer == 0) || (bufLen <= 0))
	{
		CLLogger::WriteLogMsg("In CLIOVector::PushBack(), pBuffer or bufLen is 0", 0);
		return CLStatus(-1, 0);
	}
	struct iovec item;
	item.iov_base = pBuffer;
	item.iov_len = bufLen;

	m_ioVecQueue.push_front(item);

	m_iDataLength += bufLen;

	return CLStatus(0, 0);
}

CLStatus CLIOVector::PopBack(char** pBuffer, int* bufLen)
{
	if(m_ioVecQueue.empty())
	{
		*pBuffer = 0;
		*bufLen = 0;
		return CLStatus(-1, 0);
	}
	struct iovec item;
	item = m_ioVecQueue.back();
	*(pBuffer) = (char *)item.iov_base;
	*(bufLen) = item.iov_len;

	m_iDataLength -= item.iov_len;

	m_ioVecQueue.pop_back();

	return CLStatus(0, 0);
}

CLStatus CLIOVector::PopFront(char** pBuffer, int* bufLen)
{
	if(m_ioVecQueue.empty())
	{
		*pBuffer = 0;
		*bufLen = 0;
		return CLStatus(-1, 0);
	}
	struct iovec item;
	item = m_ioVecQueue.front();
	*(pBuffer) = (char *)item.iov_base;
	*(bufLen) = item.iov_len;

	m_iDataLength -= item.iov_len;

	m_ioVecQueue.pop_front();

	return CLStatus(0, 0);
}

int CLIOVector::GetBufPtr(int index, char** pBufffer)
{
	int position = 0;
	deque<struct iovec>::iterator it = m_ioVecQueue.begin();
	for(; it <= m_ioVecQueue.end(); it++)
	{
		position += it->iov_len;
		if(position > index)
			break;
	}

	char* buf = it->iov_base;
	*pBuffer = buf + (index - position + it->iov_len);
	int continuousBufLen = position - index;
	return continuousBufLen;
}

CLStatus CLIOVector::GetIOVecs(int index, int len, CLIOVector& IOVector)
{
	int position = 0;
	deque<struct iovec>::iterator it = m_ioVecQueue.begin();
	for(; it <= m_ioVecQueue.end(); it++)
	{
		position += it->iov_len;
		if(position > index)
			break;
	}

	char* pBuffer = it->iov_base + (index - position + it->iov_len);
	int itemLen = position - index;
	int length = itemLen;

	while(length < len)
	{
		IOVector.PushBack(&pBuffer, itemLen);
		it++;
		pBuffer = it->iov_base;
		itemLen = it->iov_len;
		length += it->iov_len;
	}

	IOVector.PushBack(&pBuffer, len - (length - itemLen));

	return CLStatus(0, 0);
}

int CLIOVector::Length()
{
	return m_iDataLength;
}

int CLIOVector::IOVecNum()
{
	return m_ioVecQueue.size();
}

const char& CLIOVector::operator[](const int& index)
{
	char* buf = GetBufPtr(index);
	return buf[0];
}

CLStatus CLIOVector::FreeAll()
{
	if(m_ioVecQueue.empty())
	{
		return CLStatus(0, 0);
	}

	std::deque<struct iovec>::iterator it = m_ioVecQueue.begin();
	for(; it != m_ioVecQueue.end(); it++)
	{
		delete [] it->iov_base;
		it->iov_base = NULL;
	}

	return CLStatus(0, 0);

}
