#include <cstring>
#include "CLBuffer.h"
#include "CLIOVector.h"
// usedbufferlen 还需要设置和获得
CLBuffer::CLBuffer(int itemSize) : m_iItemSize(itemSize), m_ciDataStartIndex(m_iDataStartIndex), m_ciUsedBufferLen(m_iUsedBufferLen)
{
	m_pIOBufferVec = new CLIOVector;

	m_iSumBufferLen = m_iDataStartIndex = m_iUsedBufferLen = 0;

	CLStatus s = NewBuffer(itemSize);
	if(!s.IsSuccess())
	{
			throw "In CLBuffer::CLBuffer(int itemSize) new buffer error!";
	}
	
}

CLBuffer::~CLBuffer()
{
	if(m_pIOBufferVec != NULL)
	{
		m_pIOBufferVec->FreeAll();
		delete m_pIOBufferVec;
		m_pIOBufferVec = NULL;
	}
}

CLStatus CLBuffer::NewBuffer(int size)
{
	char* p = new char[size];
	CLStatus s = m_pIOBufferVec->PushBack(p, size);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLBuffer::NewBuffer(), bufferiovec pushback error", 0);
		return CLStatus(-1, 0);
	}
	m_iSumBufferLen += size;

	return CLStatus(0, 0);
}

const int& CLBuffer::DataStartIndex() const 
{
	return  m_ciDataStartIndex;
}

CLStatus CLBuffer::DataStartIndex(const int& newIndex)
{
	m_iDataStartIndex = newIndex;

	return CLStatus(0, 0);
}

/*CLIOVector* CLBuffer::GetDataPtr(const int& index, const int& len)
{
	CLIOVector *pIOVec = new CLIOVector();
	
}*/

// char* CLBuffer::GetDataPtr()
// {
// 	m_pIOBufferVec->GetBufPtr(m_iDataStartIndex);
// }

CLStatus CLBuffer::ReadData(char* &pBuffer, const int& index, const int& len)
{
	return m_pIOBufferVec->ReadData(pBuffer, index, len);
}

CLStatus CLBuffer::WriteData(char* pBuffer, const int& len)
{
	int restBufLen = m_iSumBufferLen - m_iUsedBufferLen;
	int diff = len - restBufLen;
	if(diff > 0)
	{
		CLStatus s = NewBuffer(diff);
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLBuffer::WriteData(), NewBuffer(0) error", 0);
			return s;
		}		
	}

	CLStatus s1 = m_pIOBufferVec->WriteData(pBuffer, m_iUsedBufferLen, len);
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLBuffer::WriteData(), m_pIOBufferVec->WriteData() error", 0);
		return s1;
	}	
	m_iUsedBufferLen += len;
	return CLStatus(0, 0);
}

CLStatus CLBuffer::GetRestBufPtr(char** pBuf, int &restLen)
{
	if(m_iUsedBufferLen == m_iSumBufferLen)
	{
		CLStatus s = NewBuffer(m_iItemSize);
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLBuffer::GetRestBufPtr(), new buffer error", 0);
			return s;
		}
	}

	int continueLen = m_pIOBufferVec->GetBufPtr(m_iUsedBufferLen, pBuf);
	
	restLen = m_iSumBufferLen - m_iUsedBufferLen; //here the continueLen is equals to restLen, should identify in debugging

	return CLStatus(0, 0);
}
	
const int& CLBuffer::UsedBufferLen() const
{
	return m_ciUsedBufferLen;
}

CLStatus CLBuffer::AddUsedBufferLen(const int& addUsedLen)
{
	m_iUsedBufferLen += addUsedLen;
	return CLStatus(0, 0);
}

CLStatus CLBuffer::AddDataStartIndex(const int& addDataLen)
{
	m_iDataStartIndex += addDataLen;
	return CLStatus(0, 0);
}

CLStatus CLBuffer::GetIOVecs(int index, int len, CLIOVector& IOVector)
{
	CLStatus s = m_pIOBufferVec->GetIOVecs(index, len, IOVector);

	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLBuffer::GetIOVecs(), error", 0);
		return s;
	}

	return CLStatus(0, 0);
}

CLStatus CLBuffer::GetDataIOVecs(CLIOVector& IOVector)
{
	return m_pIOBufferVec->GetIOVecs(m_iDataStartIndex, m_iUsedBufferLen - m_iDataStartIndex, IOVector);
}