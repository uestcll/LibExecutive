#include <cstring>
#include "CLIOVector.h"


using namespace std;

CLIOVector::CLIOVector()
{
	m_iDataLength = 0;
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

CLIOVector::CLIOVector(const CLIOVector& rhs)
{
	this->m_iDataLength = rhs.Length();

	struct iovec* pVec = rhs.GetIOVecStructs();
	for(int i = 0; i < rhs.IOVecNum(); i++)
	{
		(this->m_ioVecQueue).push_back(pVec[i]);
	}

	delete [] pVec;
}

CLIOVector& CLIOVector::operator=(const CLIOVector& rhs)
{
	this->m_iDataLength = rhs.Length();

	struct iovec* pVec = rhs.GetIOVecStructs();
	for(int i = 0; i < rhs.IOVecNum(); i++)
	{
		(this->m_ioVecQueue).push_back(pVec[i]);
	}

	delete [] pVec;

	return *this;
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

int CLIOVector::GetBufPtr(int index, char** pBuffer)
{
	int position = 0;
	deque<struct iovec>::iterator it = m_ioVecQueue.begin();
	for(; it <= m_ioVecQueue.end(); it++)
	{
		position += it->iov_len;
		if(position > index)
			break;
	}

	char* buf = (char*)(it->iov_base);
	*pBuffer = buf + (index - position + it->iov_len);
	int continuousBufLen = position - index;
	return continuousBufLen;
}

CLStatus CLIOVector::GetIOVecs(int index, int len, CLIOVector& IOVector)
{
	if((0 == index) && (m_iDataLength == len))
	{
		IOVector = *this;
		return CLStatus(0, 0);
	}	
	int position = 0;
	deque<struct iovec>::iterator it = m_ioVecQueue.begin();
	for(; it <= m_ioVecQueue.end(); it++)
	{
		position += it->iov_len;
		if(position > index)
			break;
	}

	char* pBuffer = (char*)(it->iov_base) + (index - position + it->iov_len);
	int itemLen = position - index;
	int length = itemLen;

	while(length < len)
	{
		IOVector.PushBack(pBuffer, itemLen);
		it++;
		pBuffer = (char*)(it->iov_base);
		itemLen = it->iov_len;
		length += it->iov_len;
	}

	IOVector.PushBack(pBuffer, len - (length - itemLen));
	//for test ..not used
	// if(index < 0 || index > m_iDataLength || len <= 0)
	// 	return CLStatus(-1, 0);

	// int position = 0;
	// deque<struct iovec>::iterator startIt;
	// deque<struct iovec>::iterator endIt;

	// deque<struct iovec>::iterator it = m_ioVecQueue.begin();
	// for(; it <= m_ioVecQueue.end(); it++)
	// {
	// 	position += it->iov_len;
	// 	if(position > index)
	// 	{
	// 		startIt = it;
	// 		if(index + len <= position)
	// 		{
	// 			IOVector.PushBack((char*)(startIt->iov_base) + (index - position + startIt->iov_len), len);
	// 			return CLStatus(0, 0); 
	// 		}
	// 		break;
	// 	}
		
	// }
	// it = startIt + 1;
	// for(; it <= m_ioVecQueue.end(); it++)
	// {
	// 	position += it->iov_len;
	// 	if(position >= (index + len)
	// 	{
	// 		endIt = it;
	// 		break;
	// 	}
	// }

	// CLIOVector 

	return CLStatus(0, 0);
}

const int CLIOVector::Length() const
{
	return m_iDataLength;
}

const int CLIOVector::IOVecNum() const
{
	return m_ioVecQueue.size();
}

const char& CLIOVector::operator[](const int& index)
{
	char* buf;
	int len = GetBufPtr(index, &buf);
	return buf[0];
}

CLStatus CLIOVector::FreeAndPopAll()
{
	if(m_ioVecQueue.empty())
	{
		return CLStatus(0, 0);
	}

	while(1)
	{
		char *pBuf;
		int len;
		CLStatus s = PopBack(&pBuf, &len);
		if(pBuf == 0)
			break;
		delete []pBuf;
	}
	// std::deque<struct iovec>::iterator it = m_ioVecQueue.begin();
	// for(; it != m_ioVecQueue.end(); it++)
	// {
	// 	delete [] (char *)(it->iov_base);
	// 	it->iov_base = NULL;
	// }

	return CLStatus(0, 0);

}

CLStatus CLIOVector::PopAll()
{
	while(!m_ioVecQueue.empty())
	{
		m_ioVecQueue.pop_back();
	}

	return CLStatus(0, 0);
}

CLStatus CLIOVector::ReadData(char* pBuffer, const int& index, const int& len)
{
	char* pBuf;
	int continuiousLen;
	continuiousLen = GetBufPtr(index, &pBuf);

	if(continuiousLen >= len)
	{
		// pBuffer = pBuf;
		memcpy(pBuffer, pBuf, len);
		return CLStatus(0, 0);
	}
	else
	{
		int nread = continuiousLen;
		int nstart = index + continuiousLen;
	
		while(nread != len)
		{
			continuiousLen = GetBufPtr(nstart, &pBuf);
			if(continuiousLen < (len - nread))
			{
				memcpy(pBuffer + nread, pBuf, continuiousLen);
				nread += continuiousLen;
				nstart += continuiousLen;
			}
			else 
			{
				memcpy(pBuffer + nread, pBuf, (len - nread));
				nread = len;
				return CLStatus(0, 0);
			}
		}
	}
}

CLStatus CLIOVector::WriteData(char* pBuffer, const int& index, const int& len)
{
	char* pBuf;
	int bufLen = GetBufPtr(index, &pBuf);

	if(len <= bufLen)
	{
		 memcpy(pBuf, pBuffer, len);
		 return CLStatus(0, 0);
	}
	else
	{
		int nwrite = 0;
		while(nwrite != len)
		{
			if(bufLen < (len - nwrite))
			{
				memcpy(pBuf, pBuffer + nwrite, bufLen);	
				nwrite += bufLen;
				bufLen = GetBufPtr(index, &pBuf);//buflen change to a new space len, pBuf alse
			}
			else
			{
				memcpy(pBuf, pBuffer + nwrite, (len - nwrite));
				nwrite = len;
			}
		}
	}
	return CLStatus(0, 0);
}

CLStatus CLIOVector::PushIOVecs(int index, CLIOVector& IOVector) //最好写一个从哪里开始到最后都pop出来的iovec！！！要写一个由index得到该index所在iterator位置的接口！！！
{
	CLIOVector tmpBakIOVec;
	CLIOVector tmpFrtIOVec;

	CLStatus s1 = GetIOVecs(index, m_iDataLength - index, tmpBakIOVec);
	CLStatus s2 = GetIOVecs(0, index, tmpFrtIOVec);
	if(!s1.IsSuccess() || !s2.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLIOVector::PushIOVecs(), GetIOVecs() error", 0);
		return s2;
	}

	tmpFrtIOVec.PushBackIOVecs(IOVector);

	tmpFrtIOVec.PushBackIOVecs(tmpBakIOVec);

	this->PopAll();

	return (this->PushBackIOVecs(tmpFrtIOVec));
}

CLStatus CLIOVector::PushBackIOVecs(CLIOVector& IOVector)
{
	deque<struct iovec>::iterator it = IOVector.m_ioVecQueue.begin();

	while(it != IOVector.m_ioVecQueue.end())
	{
		this->m_ioVecQueue.push_back(*it);
		it++;
	}
	return CLStatus(0, 0);
}

struct iovec* CLIOVector::GetIOVecStructs() const
{
	int num = m_ioVecQueue.size();
	if(num == 0)
	{
		return NULL;
	}
	struct iovec* pSIOVecs = new struct iovec [num];

	deque<struct iovec>::const_iterator it = m_ioVecQueue.begin();

	for(int i = 0; i < num; i++)
	{	
		pSIOVecs[i].iov_base = it->iov_base;
		pSIOVecs[i].iov_len = it->iov_len;
		++it;		
	}

	return pSIOVecs;
}