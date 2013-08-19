#include <gtest/gtest.h>
#include "LibExecutive.h"

class CLThreadForCLThreadInitialFinishedNotifier_Notify : public CLExecutiveFunctionProvider
{
public:
	virtual CLStatus RunExecutiveFunction(void* pContext)
	{
		CLThreadInitialFinishedNotifier *p = (CLThreadInitialFinishedNotifier *)(pContext);

		EXPECT_TRUE(p->NotifyInitialFinished(true).IsSuccess());

		return CLStatus(0, 0);
	}
};

TEST(CLThreadInitialFinishedNotifier, Notify)
{
	CLThread *p = new CLThread(new CLThreadForCLThreadInitialFinishedNotifier_Notify, false);

	CLEvent event;
	CLThreadInitialFinishedNotifier notifier(&event);

	EXPECT_TRUE(p->Run(&notifier).IsSuccess());

	EXPECT_TRUE(event.Wait().IsSuccess());

	EXPECT_TRUE(notifier.IsInitialSuccess());
}