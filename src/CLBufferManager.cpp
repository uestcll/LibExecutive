#include "CLBufferManager.h"
#include "CLLogger.h"
#include "CLIOVectors.h"

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
	m_pOccupiedView->PushBackIOVector(IOVector);
}

CLStatus CLBufferManager::ReleaseOccupiedIOVector(CLIOVectors& IOVector)
{
	CLStatus s = m_pOccupiedView->FindIOVectors(IOVector, true);

	if(m_bNeedDestroy && (m_pOccupiedView->Size() == 0))
	{
		delete this;
	}

	return s;
}

CLStatus CLBufferManager::GetEmptyIOVector(CLIOVectors& IOVector)
{
	CLIOVectors tmp;
	tmp.PushBackIOVector(*m_pOccupiedView);
	tmp.PushBackIOVector(*m_pPartialDataView);

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

		for(int i = num - 1; i >= 0; i--)//bug
		{
			if(tmp.IsRangeOverlap(pIO[i]))
				continue;

			if((IOVector.Size() - pIO[i].iov_len) <= m_DefaultBufferSize)
				continue;

			CLIOVectors del_iov;
			del_iov.PushBack((char *)pIO[i].iov_base, pIO[i].iov_len, false);

			//performance????
			m_pOverallView->FindIOVectors(del_iov, true);
			IOVector.FindIOVectors(del_iov, true);
		}

		return CLStatus(0, 0);
	}

	return CLStatus(0, 0);
}

void CLBufferManager::GetPartialDataIOVector(CLIOVectors& IOVector)
{
	IOVector.PushBackIOVector(*m_pPartialDataView);
}

void CLBufferManager::SetPartialDataIOVector(CLIOVectors& IOVector)
{
	if(m_pPartialDataView)
		delete m_pPartialDataView;

	m_pPartialDataView = new CLIOVectors;
	m_pPartialDataView->PushBackIOVector(IOVector);
}

void CLBufferManager::AddIOVectorToOverallView(CLIOVectors& IOVector)
{
	m_pOverallView->PushBackIOVector(IOVector);
}

void CLBufferManager::SetDestroyFlag()
{
	m_bNeedDestroy = true;
	
	if(m_pPartialDataView)
		delete m_pPartialDataView;

	m_pPartialDataView = 0;

	if(m_pOccupiedView->Size() == 0)
		delete this;
}