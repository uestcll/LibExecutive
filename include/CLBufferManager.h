#ifndef CLBufferManager_H
#define CLBufferManager_H

#include "CLStatus.h"

class CLIOVectors;

class CLBufferManager
{
public:
	CLBufferManager(unsigned int ThresholdForStep = 1024, unsigned int StepSize = 4096, unsigned int DefaultBufferSize = 4096);
	virtual ~CLBufferManager();

	void AddOccupiedIOVector(CLIOVectors& IOVector);
	CLStatus ReleaseOccupiedIOVector(CLIOVectors& IOVector);

private:
	CLBufferManager(const CLBufferManager&);
	CLBufferManager& operator=(const CLBufferManager&);

private:
	CLIOVectors *m_pOverallView;
	CLIOVectors *m_pOccupiedView;
	CLIOVectors *m_pPartialDataView;

	unsigned int m_StepSize;
	unsigned int m_ThresholdForStep;
};

#endif