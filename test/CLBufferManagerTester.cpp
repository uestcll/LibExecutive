#include <gtest/gtest.h>
#include "LibExecutive.h"

static bool CheckIOVectorStatus(iovec *pIOV, int n, CLIOVectors& iovs)
{
	if((pIOV == 0) && (n == 0))
	{
		if((iovs.Size() == 0) &&
			(iovs.GetNumberOfIOVec() == 0) &&
			(iovs.GetIOVecArray() == (iovec *)0))
			return true;
		else
			return false;
	}

	if((pIOV == 0) || (n == 0))
		return false;

	if(iovs.GetNumberOfIOVec() != n)
		return false;

	iovec *piov = iovs.GetIOVecArray();
	if(piov == 0)
		return false;

	try
	{
		int count = 0;

		for(int i = 0; i < n; i++)
		{
			if(pIOV[i].iov_base != piov[i].iov_base)
				throw false;

			if(pIOV[i].iov_len != piov[i].iov_len)
				throw false;

			count += pIOV[i].iov_len;
		}

		if(count != iovs.Size())
			throw false;

		throw true;
	}
	catch(bool b)
	{
		delete [] piov;
		return b;
	}
}

struct CLBufferManagerInfo
{
	CLIOVectors *m_pOverallView;
	CLIOVectors *m_pOccupiedView;
	CLIOVectors *m_pPartialDataView;

	unsigned int m_StepSize;
	unsigned int m_ThresholdForStep;
	unsigned int m_ThresholdForReduce;
	unsigned int m_DefaultBufferSize;

	bool m_bNeedDestroy;
};

TEST(CLBufferManager, Constructor_Features_Test)
{
	CLLogger::WriteLogMsg("CLBufferManager Test", 0);

	{
		CLBufferManager bm;
		CLBufferManagerInfo *pInfo = (CLBufferManagerInfo *)((char *)(&bm) + 8);

		EXPECT_EQ(pInfo->m_StepSize, 4096);
		EXPECT_EQ(pInfo->m_ThresholdForStep, 1024);
		EXPECT_EQ(pInfo->m_ThresholdForReduce, 4096 * 4);
		EXPECT_EQ(pInfo->m_DefaultBufferSize, 4096);
		EXPECT_FALSE(pInfo->m_bNeedDestroy);

		EXPECT_EQ(pInfo->m_pOverallView->Size(), 4096);
		EXPECT_EQ(pInfo->m_pOverallView->GetNumberOfIOVec(), 1);

		EXPECT_TRUE(CheckIOVectorStatus(0, 0, *(pInfo->m_pOccupiedView)));
		EXPECT_TRUE(CheckIOVectorStatus(0, 0, *(pInfo->m_pPartialDataView)));
	}
	
	{
		CLBufferManager bm(10 * 1024, 2 * 1024, 2 * 1024, 4 * 1024);
		CLBufferManagerInfo *pInfo = (CLBufferManagerInfo *)((char *)(&bm) + 8);

		EXPECT_EQ(pInfo->m_StepSize, 2048);
		EXPECT_EQ(pInfo->m_ThresholdForStep, 2048);
		EXPECT_EQ(pInfo->m_ThresholdForReduce, 10240);
		EXPECT_EQ(pInfo->m_DefaultBufferSize, 4096);
		EXPECT_FALSE(pInfo->m_bNeedDestroy);

		EXPECT_EQ(pInfo->m_pOverallView->Size(), 4096);
		EXPECT_EQ(pInfo->m_pOverallView->GetNumberOfIOVec(), 1);

		EXPECT_TRUE(CheckIOVectorStatus(0, 0, *(pInfo->m_pOccupiedView)));
		EXPECT_TRUE(CheckIOVectorStatus(0, 0, *(pInfo->m_pPartialDataView)));
	}
}

TEST(CLBufferManager, AddIOVectorToOverallView_Features_Test)
{
	CLBufferManager bm;

	char *buf = new char[10];
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack(buf, 10, false).IsSuccess());

	bm.AddIOVectorToOverallView(iov);

	CLBufferManagerInfo *pInfo = (CLBufferManagerInfo *)((char *)(&bm) + 8);

	iovec *iov1 = pInfo->m_pOverallView->GetIOVecArray();

	EXPECT_TRUE(pInfo->m_pOverallView->GetNumberOfIOVec() == 2);
	EXPECT_TRUE((char *)(iov1[1].iov_base) == buf);
	EXPECT_TRUE(iov1[1].iov_len == 10);

	delete [] iov1;
}

TEST(CLBufferManager, AddOccupiedIOVector_Features_Test)
{
	CLBufferManager bm;
	CLBufferManagerInfo *pInfo = (CLBufferManagerInfo *)((char *)(&bm) + 8);

	iovec *pIO = pInfo->m_pOverallView->GetIOVecArray();
	char *pbase = (char *)pIO->iov_base + 20;
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack(pbase, 10).IsSuccess());
	
	bm.AddOccupiedIOVector(iov);

	iovec io1;
	io1.iov_base = pbase;
	io1.iov_len = 10;

	EXPECT_TRUE(CheckIOVectorStatus(&io1, 1, *(pInfo->m_pOccupiedView)));

	delete [] pIO;
}

TEST(CLBufferManager, ReleaseOccupiedIOVector_Features_Test)
{
	{
		CLBufferManager bm;
		CLBufferManagerInfo *pInfo = (CLBufferManagerInfo *)((char *)(&bm) + 8);

		iovec *pIO = pInfo->m_pOverallView->GetIOVecArray();
		char *pbase = (char *)pIO->iov_base + 20;
		CLIOVectors iov;
		EXPECT_TRUE(iov.PushBack(pbase, 10).IsSuccess());

		bm.AddOccupiedIOVector(iov);

		EXPECT_TRUE(bm.ReleaseOccupiedIOVector(iov).IsSuccess());

		EXPECT_TRUE(CheckIOVectorStatus(0, 0, *(pInfo->m_pOccupiedView)));

		delete [] pIO;
	}

	{
		CLBufferManager *pbm = new CLBufferManager();
		CLBufferManagerInfo *pInfo = (CLBufferManagerInfo *)((char *)pbm + 8);

		iovec *pIO = pInfo->m_pOverallView->GetIOVecArray();
		char *pbase = (char *)pIO->iov_base + 20;
		CLIOVectors iov;
		EXPECT_TRUE(iov.PushBack(pbase, 10).IsSuccess());

		pbm->AddOccupiedIOVector(iov);
		pbm->SetDestroyFlag();		

		EXPECT_TRUE(pbm->ReleaseOccupiedIOVector(iov).IsSuccess());

		delete [] pIO;
	}
}

TEST(CLBufferManager, SetAndGetPartialDataIOVector_Features_Test)
{
	CLBufferManager bm;
	CLBufferManagerInfo *pInfo = (CLBufferManagerInfo *)((char *)(&bm) + 8);

	iovec *pIO = pInfo->m_pOverallView->GetIOVecArray();
	char *pbase = (char *)pIO->iov_base + 20;
	CLIOVectors iov;
	EXPECT_TRUE(iov.PushBack(pbase, 10).IsSuccess());

	bm.SetPartialDataIOVector(iov);

	iovec io1;
	io1.iov_base = pbase;
	io1.iov_len = 10;

	EXPECT_TRUE(CheckIOVectorStatus(&io1, 1, *(pInfo->m_pPartialDataView)));

	bm.SetPartialDataIOVector(iov);
	EXPECT_TRUE(CheckIOVectorStatus(&io1, 1, *(pInfo->m_pPartialDataView)));

	CLIOVectors tio;
	bm.GetPartialDataIOVector(tio);

	EXPECT_TRUE(CheckIOVectorStatus(&io1, 1, tio));

	delete [] pIO;
}

TEST(CLBufferManager, SetDestroyFlag_Features_Test)
{
	CLBufferManager *pbm = new CLBufferManager;
	pbm->SetDestroyFlag();
}

TEST(CLBufferManager, GetEmptyIOVector_Features_Test)
{
	CLBufferManager *pBM = new CLBufferManager;
	
	CLBufferManagerInfo *pInfo = (CLBufferManagerInfo *)((char *)pBM + 8);
	iovec *pIO = pInfo->m_pOverallView->GetIOVecArray();
	int num = pInfo->m_pOverallView->GetNumberOfIOVec();

	CLIOVectors iov;
	EXPECT_TRUE(pBM->GetEmptyIOVector(iov).IsSuccess());
	EXPECT_TRUE(CheckIOVectorStatus(pIO, num, iov));

	iov.Clear();

	char *pBase = (char *)(pIO[0].iov_base);
	CLIOVectors iov_partial;
	EXPECT_TRUE(iov_partial.PushBack(pBase+100, 10).IsSuccess());
	pBM->SetPartialDataIOVector(iov_partial);
	EXPECT_TRUE(pBM->GetEmptyIOVector(iov).IsSuccess());
	iovec IOVEC[2];
	IOVEC[0].iov_base = pBase;
	IOVEC[0].iov_len = 100;
	IOVEC[1].iov_base = pBase + 110;
	IOVEC[1].iov_len = 4096-110;
	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 2, iov));

	iov.Clear();
	CLIOVectors iov_occ;
	EXPECT_TRUE(iov_occ.PushBack(pBase+200, 20).IsSuccess());
	pBM->AddOccupiedIOVector(iov_occ);
	EXPECT_TRUE(pBM->GetEmptyIOVector(iov).IsSuccess());
	iovec IOVEC1[3];
	IOVEC1[0].iov_base = pBase;
	IOVEC1[0].iov_len = 100;
	IOVEC1[1].iov_base = pBase + 110;
	IOVEC1[1].iov_len = 90;
	IOVEC1[2].iov_base = pBase + 220;
	IOVEC1[2].iov_len = 4096-220;
	EXPECT_TRUE(CheckIOVectorStatus(IOVEC1, 3, iov));
	EXPECT_TRUE(pBM->ReleaseOccupiedIOVector(iov_occ).IsSuccess());

	iov.Clear();

	delete [] pIO;

	pBM->SetDestroyFlag();
}

TEST(CLBufferManager, GetEmptyIOVector_Features2_Test)
{
	CLBufferManager *pBM = new CLBufferManager;

	CLBufferManagerInfo *pInfo = (CLBufferManagerInfo *)((char *)pBM + 8);
	iovec *pIO = pInfo->m_pOverallView->GetIOVecArray();

	char *pBase = (char *)(pIO[0].iov_base);
	CLIOVectors iov_partial;
	EXPECT_TRUE(iov_partial.PushBack(pBase+100, 3500).IsSuccess());
	pBM->SetPartialDataIOVector(iov_partial);

	CLIOVectors iov;
	EXPECT_TRUE(pBM->GetEmptyIOVector(iov).IsSuccess());

	iovec *pIO1 = pInfo->m_pOverallView->GetIOVecArray();

	iovec IOVEC1[3];
	IOVEC1[0].iov_base = pBase;
	IOVEC1[0].iov_len = 100;
	IOVEC1[1].iov_base = pBase + 3600;
	IOVEC1[1].iov_len = 4096-3600;
	IOVEC1[2].iov_base = pIO1[1].iov_base;
	IOVEC1[2].iov_len = pIO1[1].iov_len;
	EXPECT_TRUE(CheckIOVectorStatus(IOVEC1, 3, iov));

	iov.Clear();

	delete [] pIO1;
	delete [] pIO;

	pBM->SetDestroyFlag();
}

TEST(CLBufferManager, GetEmptyIOVector_Features3_Test)
{
	iovec IOVEC[4];
	CLBufferManager *pBM = new CLBufferManager;

	CLBufferManagerInfo *pInfo = (CLBufferManagerInfo *)((char *)pBM + 8);
	iovec *pIO = pInfo->m_pOverallView->GetIOVecArray();

	char *pBase = (char *)(pIO[0].iov_base);
	CLIOVectors iov_occ;
	EXPECT_TRUE(iov_occ.PushBack(pBase+1, 4095).IsSuccess());
	pBM->AddOccupiedIOVector(iov_occ);
	
	CLIOVectors iov;
	EXPECT_TRUE(pBM->GetEmptyIOVector(iov).IsSuccess());

	iovec *pIO1 = pInfo->m_pOverallView->GetIOVecArray();
	char *pBase1 = (char *)(pIO1[1].iov_base);

	IOVEC[0].iov_base = pBase;
	IOVEC[0].iov_len = 1;
	IOVEC[1].iov_base = pBase1;
	IOVEC[1].iov_len = 4096;
	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 2, iov));

	IOVEC[0].iov_base = pBase;
	IOVEC[0].iov_len = 4096;
	IOVEC[1].iov_base = pBase1;
	IOVEC[1].iov_len = 4096;
	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 2, *(pInfo->m_pOverallView)));

	iov_occ.Clear();
	EXPECT_TRUE(iov_occ.PushBack(pBase1+1, 4095).IsSuccess());
	pBM->AddOccupiedIOVector(iov_occ);

	iov.Clear();
	EXPECT_TRUE(pBM->GetEmptyIOVector(iov).IsSuccess());

	iovec *pIO2 = pInfo->m_pOverallView->GetIOVecArray();
	char *pBase2 = (char *)(pIO2[2].iov_base);

	IOVEC[0].iov_base = pBase;
	IOVEC[0].iov_len = 1;
	IOVEC[1].iov_base = pBase1;
	IOVEC[1].iov_len = 1;
	IOVEC[2].iov_base = pBase2;
	IOVEC[2].iov_len = 4096;
	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 3, iov));

	IOVEC[0].iov_base = pBase;
	IOVEC[0].iov_len = 4096;
	IOVEC[1].iov_base = pBase1;
	IOVEC[1].iov_len = 4096;
	IOVEC[2].iov_base = pBase2;
	IOVEC[2].iov_len = 4096;
	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 3, *(pInfo->m_pOverallView)));

	iov_occ.Clear();
	EXPECT_TRUE(iov_occ.PushBack(pBase2+1, 4095).IsSuccess());
	pBM->AddOccupiedIOVector(iov_occ);

	iov.Clear();
	EXPECT_TRUE(pBM->GetEmptyIOVector(iov).IsSuccess());

	iovec *pIO3 = pInfo->m_pOverallView->GetIOVecArray();
	char *pBase3 = (char *)(pIO3[3].iov_base);

	IOVEC[0].iov_base = pBase;
	IOVEC[0].iov_len = 1;
	IOVEC[1].iov_base = pBase1;
	IOVEC[1].iov_len = 1;
	IOVEC[2].iov_base = pBase2;
	IOVEC[2].iov_len = 1;
	IOVEC[3].iov_base = pBase3;
	IOVEC[3].iov_len = 4096;
	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 4, iov));

	IOVEC[0].iov_base = pBase;
	IOVEC[0].iov_len = 4096;
	IOVEC[1].iov_base = pBase1;
	IOVEC[1].iov_len = 4096;
	IOVEC[2].iov_base = pBase2;
	IOVEC[2].iov_len = 4096;
	IOVEC[3].iov_base = pBase3;
	IOVEC[3].iov_len = 4096;
	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 4, *(pInfo->m_pOverallView)));

	iov_occ.Clear();
	EXPECT_TRUE(iov_occ.PushBack(pBase+1, 4095).IsSuccess());
	EXPECT_TRUE(iov_occ.PushBack(pBase1+1, 4095).IsSuccess());
	EXPECT_TRUE(iov_occ.PushBack(pBase2+1, 4095).IsSuccess());
	EXPECT_TRUE(pBM->ReleaseOccupiedIOVector(iov_occ).IsSuccess());

	iov.Clear();
	EXPECT_TRUE(pBM->GetEmptyIOVector(iov).IsSuccess());

	IOVEC[0].iov_base = pBase;
	IOVEC[0].iov_len = 4096;

	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 1, *(pInfo->m_pOverallView)));
	EXPECT_TRUE(CheckIOVectorStatus(IOVEC, 1, iov));

	delete [] pIO3;
	delete [] pIO2;
	delete [] pIO1;
	delete [] pIO;

	pBM->SetDestroyFlag();
}