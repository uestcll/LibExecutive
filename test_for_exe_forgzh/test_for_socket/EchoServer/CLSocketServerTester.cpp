#include <gtest/gtest.h>
#include <iostream>
#include <queue>
#include "CLSocket.h"
#include "LibExecutive.h"

using namespace std;

TEST(CLSocket, TCPServer)
{
	CLMessageReceiverByTCPListener *pServer = new CLMessageReceiverByTCPListener("localhost", "2323");

	int listenFd = pServer->GetFd();

	cout<<"The Listen Fd is: "<<listenFd<<endl;
	EXPECT_TRUE(listenFd);

	queue<CLMessage*> MessageQueue;

    while(true)
	{
		CLStatus s1 = pServer->GetMessage(MessageQueue);
		if(!s1.IsSuccess())
		{
			if(s1.m_clErrorCode != ACCEPT_PENDING)
			{
				cout<<"GetMessage() error "<<endl;
				return;
			}
			
			continue;
		}

		while(!MessageQueue.empty())
		{
			CLMessage* pMsg = m_MessageQueue.front();
			MessageQueue.pop();

			if(pMsg != 0)
			{
				// CLStatus s4 = DispatchMessage(pMsg);
				EXPECT_EQ(pMsg->m_clMsgID, TCP_CLIENT_ARRIVED_MSG_ID);
				CLTCPClientArrivedMessage *pClientSocketMsg = dynamic_cast<CLTCPClientArrivedMessage *>(pMsg);
				EXPECT_TRUE(pClientSocketMsg);

				CLSocket *pClient = pClientSocketMsg->m_pClientSocket;
				CLIOVector iovec;

				while(1)
				{		
					char *pBuff = new char[100];

					iovec.PushBack(pBuff, 100);

					CLStatus s2 = pClient->Read(iovec, NULL);

					if(s2.IsSuccess())
					{		
						if(s2.m_clErrorCode == IO_PAUSE)
						{
							cout<<"The connect closed by client!"<<endl;
							iovec->FreeAndPopAll();

							break;
						}

						char *data;

						EXPECT_EQ(iovec.GetBufPtr(0, &data), 100);
						EXPECT_EQ(s2.m_lReturnCode, 15);

						cout<<"The data is: "<<data;

						iovec->FreeAndPopAll();
					}

					if(!s2.IsSuccess() && s2.m_clErrorCode != IO_PENDING)
					{
						iovec->FreeAndPopAll();

						break;
					}
				}

				delete pMsg;
			}
		}	
	}
}