#include "CLBuffer.h"
#include "CLIOVector.h"
// usedbufferlen 还需要设置和获得
CLBuffer::CLBuffer()
{
	m_pIOBufferVec = new CLIOVector;

	m_iSumBufferLen = m_iDataStartIndex = m_iUsedBufferLen = 0;
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

const int& CLBuffer::DataStartIndex()
{
	return  m_iDataStartIndex;
}

CLStatus CLBuffer::DataStartIndex(const int& newIndex)
{
	m_iDataStartIndex = newIndex;

	return CLStatus(0, 0);
}

CLIOVector* CLBuffer::GetDataPtr(const int& index, const int& len)
{
	CLIOVector *pIOVec = new CLIOVector();
	
}
char* CLBuffer::GetDataPtr()
{
	return m_pIOBufferVec->GetBufPtr(m_iDataStartIndex);
}

CLStatus CLBuffer::GetRestBufPtr(char** pBuf, int *restLen)
{
	if(m_iUsedBufferLen == m_iSumBufferLen)
	{
		CLStatus s = NewBuffer(DEAFULT_BUFFER_LENGTH);
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLBuffer::GetBufPtr(), new buffer error", 0);
			return s;
		}
	}

	*pBuf = m_pIOBufferVec->GetBufPtr(m_iUsedBufferLen);
	*restLen = m_iSumBufferLen - m_iUsedBufferLen;

	return CLStatus(0, 0);
}

	
const int& CLBuffer::UsedBufferLen()
{
	return m_iUsedBufferLen;
}

CLStatus CLBuffer::AddUsedBufferLen(const int& addUsedLen)
{
	m_iUsedBufferLen += addUsedLen;
	return CLStatus(0, 0);
}