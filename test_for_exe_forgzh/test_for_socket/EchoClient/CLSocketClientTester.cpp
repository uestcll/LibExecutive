#include <gtest/gtest.h>
#include <iostream>
#include <queue>
#include <string.h>
#include "CLSocket.h"
#include "LibExecutive.h"

using namespace std;

TEST(CLSocket, TCPClient)
{
	CLSocket *pClient = new CLSocket("localhost", "2323", false);

	int clientFd = pClient->GetSocketFd();

	EXPECT_TRUE((clientFd == -1));

	cout<<"The Client Fd is: "<<clientFd<<endl;

	while(1)
	{
		CLStatus s1 = pClient->Connect();
		if(s1.IsSuccess())
		{

			clientFd = pClient->GetSocketFd();

			EXPECT_TRUE((clientFd != -1));

			cout<<"The Client Fd is: "<<clientFd<<endl;

			cout<<"Connect Success()!"<<endl;

			CLIOVector iovec;

			char *buf = new char [15];
			memset(buf, 0, 15);
			strcpy(buf, "hellohellohell");

			iovec.PushBack(buf, 15);

			CLStatus s2 = pClient->Write(iovec);

			if(s2.IsSuccess())
			{		
				if(s2.m_clErrorCode == IO_PAUSE)
				{
					cout<<"The connect closed by server!"<<endl;
					iovec.FreeAndPopAll();

					break;
				}

				EXPECT_EQ(s2.m_clReturnCode, 15);

				cout<<"The data is: "<<buf<<endl;

				iovec.FreeAndPopAll();
			}

			if(!s2.IsSuccess() && s2.m_clErrorCode != IO_PENDING)
			{
				iovec.FreeAndPopAll();

				break;
			}


			break;
		}
		else if(s1.m_clErrorCode == CONNECT_PENDING)
		{
			cout<<"Connect Pending()!"<<endl;

			continue;
		}
		else
		{
			cout<<"Connect Error()!"<<endl;

			break;
		}
	}

}