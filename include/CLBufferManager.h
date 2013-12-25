#ifndef CLBufferManager_H
#define CLBufferManager_H

#include "CLStatus.h"
#include "CLMutex.h"

class CLIOVectors;

class CLBufferManager
{
public:
	CLBufferManager();
	CLBufferManager(unsigned int ThresholdForReduce, unsigned int ThresholdForStep, unsigned int StepSize, unsigned int DefaultBufferSize);
	virtual ~CLBufferManager();

	void AddOccupiedIOVector(CLIOVectors& IOVector);
	CLStatus ReleaseOccupiedIOVector(CLIOVectors& IOVector);

	CLStatus GetEmptyIOVector(CLIOVectors& IOVector);

	void GetPartialDataIOVector(CLIOVectors& IOVector);
	void SetPartialDataIOVector(CLIOVectors& IOVector);

	void AddIOVectorToOverallView(CLIOVectors& IOVector);

	void SetDestroyFlag();

private:
	CLBufferManager(const CLBufferManager&);
	CLBufferManager& operator=(const CLBufferManager&);

	void Initialize(unsigned int ThresholdForReduce, unsigned int ThresholdForStep, unsigned int StepSize, unsigned int DefaultBufferSize);

private:
	CLIOVectors *m_pOverallView;
	CLIOVectors *m_pOccupiedView;
	CLIOVectors *m_pPartialDataView;

	unsigned int m_StepSize;
	unsigned int m_ThresholdForStep;
	unsigned int m_ThresholdForReduce;
	unsigned int m_DefaultBufferSize;

	bool m_bNeedDestroy;

	CLMutex m_Mutex;
};

#endif