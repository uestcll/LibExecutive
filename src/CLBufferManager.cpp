#include "CLBufferManager.h"
#include "CLLogger.h"
#include "CLIOVectors.h"

CLBufferManager::CLBufferManager(unsigned int ThresholdForStep, unsigned int StepSize, unsigned int DefaultBufferSize)
{
	if(DefaultBufferSize == 0)
		throw "In CLBufferManager::CLBufferManager(), DefaultBufferSize error";

	m_pOverallView = new CLIOVectors;
	m_pOccupiedView = new CLIOVectors;
	m_pPartialDataView = new CLIOVectors;

	char *p = new char [DefaultBufferSize];
	if(!m_pOverallView->PushBack(p, DefaultBufferSize, true).IsSuccess())
	{
		delete p;
		delete m_pOccupiedView;
		delete m_pPartialDataView;
		delete m_pOverallView;

		throw "In CLBufferManager::CLBufferManager(), PushBack error";
	}

	m_StepSize = StepSize;
	m_ThresholdForStep = ThresholdForStep;
}

CLBufferManager::~CLBufferManager()
{
	delete m_pOccupiedView;
	delete m_pPartialDataView;
	delete m_pOverallView;
}

void CLBufferManager::AddOccupiedIOVector(CLIOVectors& IOVector)
{
	m_pOccupiedView->PushBackIOVector(IOVector);
}

CLStatus CLBufferManager::ReleaseOccupiedIOVector(CLIOVectors& IOVector)
{
	//........................
}