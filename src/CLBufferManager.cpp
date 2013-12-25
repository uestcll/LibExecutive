#include "CLBufferManager.h"
#include "CLLogger.h"
#include "CLIOVectors.h"
#include "CLCriticalSection.h"

#define THRESHOLD_FOR_STEP 1024
#define DEFAULT_BUFFER_SIZE  4 * THRESHOLD_FOR_STEP
#define STEP_SIZE DEFAULT_BUFFER_SIZE
#define THRESHOLD_FOR_REDUCE 4 * DEFAULT_BUFFER_SIZE

CLBufferManager::CLBufferManager()
{
	Initialize(THRESHOLD_FOR_REDUCE, THRESHOLD_FOR_STEP, STEP_SIZE, DEFAULT_BUFFER_SIZE);
}

CLBufferManager::CLBufferManager(unsigned int ThresholdForReduce, unsigned int ThresholdForStep, unsigned int StepSize, unsigned int DefaultBufferSize)
{
	if((DefaultBufferSize == 0) || 
		(ThresholdForStep >= DefaultBufferSize) ||
		(ThresholdForReduce <= DefaultBufferSize))
		throw "In CLBufferManager::CLBufferManager(), Parameters error";

	Initialize(ThresholdForReduce, ThresholdForStep, StepSize, DefaultBufferSize);
}

void CLBufferManager::Initialize(unsigned int ThresholdForReduce, unsigned int ThresholdForStep, unsigned int StepSize, unsigned int DefaultBufferSize)
{
	m_StepSize = StepSize;
	m_ThresholdForStep = ThresholdForStep;
	m_ThresholdForReduce = ThresholdForReduce;
	m_DefaultBufferSize = DefaultBufferSize;

	m_pOverallView = new CLIOVectors;
	m_pOccupiedView = new CLIOVectors;
	m_pPartialDataView = new CLIOVectors;

	char *p = new char [m_DefaultBufferSize];
	m_pOverallView->PushBack(p, m_DefaultBufferSize, true);

	m_bNeedDestroy = false;
}

CLBufferManager::~CLBufferManager()
{
	if(m_pOccupiedView)
		delete m_pOccupiedView;

	if(m_pPartialDataView)
		delete m_pPartialDataView;

	if(m_pOverallView)
		delete m_pOverallView;
}

void CLBufferManager::AddOccupiedIOVector(CLIOVectors& IOVector)
{
	CLCriticalSection cs(&m_Mutex);

	m_pOccupiedView->PushBackIOVector(IOVector, IOVECTOR_NON_DELETE);
}

CLStatus CLBufferManager::ReleaseOccupiedIOVector(CLIOVectors& IOVector)
{
	bool flag = false;
	long r, e;

	{
		CLCriticalSection cs(&m_Mutex);

		CLStatus s = m_pOccupiedView->FindIOVectors(IOVector, true);
		r = s.m_clReturnCode;
		e = s.m_clErrorCode;

		if(m_bNeedDestroy && (m_pOccupiedView->Size() == 0))
			flag = true;
	}
	
	if(flag)
		delete this;

	return CLStatus(r, e);
}

CLStatus CLBufferManager::GetEmptyIOVector(CLIOVectors& IOVector)
{
	CLIOVectors tmp;
	tmp.PushBackIOVector(*m_pOccupiedView, IOVECTOR_NON_DELETE);
	tmp.PushBackIOVector(*m_pPartialDataView, IOVECTOR_NON_DELETE);

	m_pOverallView->DifferenceBetweenIOVectors(tmp, IOVector);

	if((IOVector.Size() <= m_ThresholdForStep) && (m_StepSize != 0))
	{
		char *p = new char [m_StepSize];
		m_pOverallView->PushBack(p, m_StepSize, true);
		IOVector.PushBack(p, m_StepSize, false);
		return CLStatus(0, 0);
	}

	if(IOVector.Size() >= m_ThresholdForReduce)
	{
		iovec* pIO = m_pOverallView->GetIOVecArray();
		int num = m_pOverallView->GetNumberOfIOVec();

		for(int i = num - 1; i >= 0; i--)
		{
			if(tmp.IsRangeOverlap(pIO[i]))
				continue;

			CLIOVectors del_iov;
			del_iov.PushBack((char *)pIO[i].iov_base, pIO[i].iov_len, true);

			m_pOverallView->FindIOVectors(del_iov, true);
			IOVector.FindIOVectors(del_iov, true);

			if(IOVector.Size() <= m_DefaultBufferSize)
				break;
		}

		delete [] pIO;

		return CLStatus(0, 0);
	}

	return CLStatus(0, 0);
}

void CLBufferManager::GetPartialDataIOVector(CLIOVectors& IOVector)
{
	IOVector.PushBackIOVector(*m_pPartialDataView, IOVECTOR_NON_DELETE);
}

void CLBufferManager::SetPartialDataIOVector(CLIOVectors& IOVector)
{
	m_pPartialDataView->Clear();

	m_pPartialDataView->PushBackIOVector(IOVector, IOVECTOR_NON_DELETE);
}

void CLBufferManager::AddIOVectorToOverallView(CLIOVectors& IOVector)
{
	m_pOverallView->PushBackIOVector(IOVector, IOVECTOR_DELETE);
}

void CLBufferManager::SetDestroyFlag()
{
	bool flag = false;

	{
		CLCriticalSection cs(&m_Mutex);

		m_bNeedDestroy = true;

		if(m_pPartialDataView)
			delete m_pPartialDataView;

		m_pPartialDataView = 0;

		if(m_pOccupiedView->Size() == 0)
			flag = true;
	}

	if(flag)
		delete this;
}