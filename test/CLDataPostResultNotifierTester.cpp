#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLDataPostResultNotifier, Notify_Features_Test)
{
	CLDataPostResultNotifier notify;
	EXPECT_TRUE(notify.Notify(MSG_POST_SUCCESS).IsSuccess());
	EXPECT_TRUE(notify.Notify(MSG_POST_ERROR).IsSuccess());
	EXPECT_FALSE(notify.Notify(100).IsSuccess());

	CLMessage *p = new CLMessage(1);
	notify.SetMsg(p);
	EXPECT_TRUE(notify.Notify(MSG_POST_SUCCESS).IsSuccess());

	delete p;
}

TEST(CLDataPostResultNotifier, Notify2_Features_Test)
{
	CLDataPostResultNotifier notify(true);
	EXPECT_TRUE(notify.Notify(MSG_POST_SUCCESS).IsSuccess());
	EXPECT_TRUE(notify.Notify(MSG_POST_ERROR).IsSuccess());
	EXPECT_FALSE(notify.Notify(100).IsSuccess());

	CLMessage *p = new CLMessage(1);
	notify.SetMsg(p);
	EXPECT_TRUE(notify.Notify(MSG_POST_SUCCESS).IsSuccess());
}

class CLDataPostResultNotifier_Test : public CLDataPostResultNotifier
{
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

TEST(CLDataPostResultNotifier, Notify3_Features_Test)
{
	CLDataPostResultNotifier_Test notify;
	
	CLStatus s1 = notify.Notify(MSG_POST_SUCCESS);
	EXPECT_EQ(s1.m_clReturnCode, 3);

	CLStatus s2 = notify.Notify(MSG_POST_ERROR);
	EXPECT_EQ(s2.m_clReturnCode, 2);

	CLMessage *p = new CLMessage(1);
	notify.SetMsg(p);
	EXPECT_TRUE(notify.Notify(MSG_POST_SUCCESS).IsSuccess());

	delete p;
}