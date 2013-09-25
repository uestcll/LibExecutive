#include "CLIteratorForIOVectors.h"
#include "ErrorCode.h"

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
		return CLStatus(-1, NORMAL_ERROR);

	if(steps == 0)
		return CLStatus(0, 0);

	unsigned int offset = m_pData - (char *)m_Iter->IOVector.iov_base;
	unsigned int left_room = m_Iter->IOVector.iov_len - offset;
	if(steps < left_room - offset)
	{
		m_pData = m_pData + steps;
		return CLStatus(0, 0);
	}
}

CLStatus CLIteratorForIOVectors::Sub(unsigned int steps)
{

}