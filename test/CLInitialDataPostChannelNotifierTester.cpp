#include <gtest/gtest.h>
#include "LibExecutive.h"

class CLInitialDataPostChannelNotifier_Test : public CLInitialDataPostChannelNotifier
{
public:

protected:
	virtual CLStatus NotifySuccess()
	{
		return CLStatus(3, 0);
	}

	virtual CLStatus NotifyFailure()
	{
		return CLStatus(2, 0);
	}
};

TEST(CLInitialDataPostChannelNotifier, Notify_Features_Test)
{
	CLLogger::WriteLogMsg("CLInitialDataPostChannelNotifier Test", 0);

	CLInitialDataPostChannelNotifier initial;
	EXPECT_TRUE(initial.Notify(DATA_POSTER_INITIALIZE_SUCCESS).IsSuccess());
	EXPECT_TRUE(initial.Notify(DATA_POSTER_INITIALIZE_ERROR).IsSuccess());
	EXPECT_FALSE(initial.Notify(100).IsSuccess());
}

TEST(CLInitialDataPostChannelNotifier, Notify2_Features_Test)
{
	CLInitialDataPostChannelNotifier_Test initial;
	CLStatus s1 = initial.Notify(DATA_POSTER_INITIALIZE_SUCCESS);
	EXPECT_EQ(s1.m_clReturnCode, 3);

	CLStatus s2 = initial.Notify(DATA_POSTER_INITIALIZE_ERROR);
	EXPECT_EQ(s2.m_clReturnCode, 2);

	EXPECT_FALSE(initial.Notify(100).IsSuccess());
}