#include "CLIOVectors.h"
#include "CLIteratorForIOVectors.h"
#include "ErrorCode.h"
#include "CLLogger.h"

CLIteratorForIOVectors::CLIteratorForIOVectors()
{
	m_pData = 0;
	m_pIOVectors = 0;
}

CLIteratorForIOVectors::~CLIteratorForIOVectors()
{
}

CLStatus CLIteratorForIOVectors::Add(unsigned int steps)
{
	if((m_pData == 0) || (m_pIOVectors == 0) || (m_Iter == m_pIOVectors->end()))
	{
		CLLogger::WriteLogMsg("In CLIteratorForIOVectors::Add(), m_pData == 0 and so on", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	if(steps == 0)
		return CLStatus(0, 0);

	unsigned int offset = m_pData - (char *)m_Iter->IOVector.iov_base;
	unsigned int left_room = m_Iter->IOVector.iov_len - offset;
	if(steps < left_room)
	{
		m_pData = m_pData + steps;
		return CLStatus(0, 0);
	}

	steps = steps - left_room;

	while(true)
	{
		m_Iter++;
		if(m_Iter == m_pIOVectors->end())
		{
			m_pData = 0;
			return CLStatus(-1, IOVECTOR_ITERATOR_END);
		}

		if(steps < m_Iter->IOVector.iov_len)
		{
			m_pData = (char *)m_Iter->IOVector.iov_base + steps;
			return CLStatus(0, 0);
		}

		steps = steps - m_Iter->IOVector.iov_len;
	}
}

CLStatus CLIteratorForIOVectors::Sub(unsigned int steps)
{
	if((m_pData == 0) || (m_pIOVectors == 0) || (m_Iter == m_pIOVectors->end()))
	{
		CLLogger::WriteLogMsg("In CLIteratorForIOVectors::Sub(), m_pData == 0 and so on", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	if(steps == 0)
		return CLStatus(0, 0);

	unsigned int offset = m_pData - (char *)m_Iter->IOVector.iov_base;
	if(steps <= offset)
	{
		m_pData = m_pData - steps;
		return CLStatus(0, 0);
	}

	steps = steps - offset - 1;

	while(true)
	{
		m_Iter--;
		if(m_Iter == m_pIOVectors->end())
		{
			m_pData = 0;
			return CLStatus(-1, IOVECTOR_ITERATOR_END);
		}

		if(steps < m_Iter->IOVector.iov_len)
		{
			m_pData = (char *)m_Iter->IOVector.iov_base + m_Iter->IOVector.iov_len - 1 - steps;
			return CLStatus(0, 0);
		}

		steps = steps - m_Iter->IOVector.iov_len;
	}
}