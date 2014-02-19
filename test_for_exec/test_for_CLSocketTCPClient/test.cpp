#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <arpa/inet.h>
#include "LibExecutive.h"

using namespace std;

int main(int argc, char* argv[])
{
	if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
	{
		cout << "in test initialize error" << endl;
		return 0;
	}

	int nClientSocket = -1;

	try
	{
		nClientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if(-1 == nClientSocket)
		{
			cout << "socket error" << endl;
			throw CLStatus(0, 0);
		}

		sockaddr_in ServerAddress;
		memset(&ServerAddress, 0, sizeof(sockaddr_in));
		ServerAddress.sin_family = AF_INET;
		if(inet_pton(AF_INET, "127.0.0.1", &ServerAddress.sin_addr) != 1)
		{
			cout << "inet_pton error" << endl;
			throw CLStatus(0, 0);
		}

		ServerAddress.sin_port = htons(3600);

		if(connect(nClientSocket, (sockaddr*)&ServerAddress, sizeof(ServerAddress)) == -1)
		{
			cout << "connect error" << endl;
			throw CLStatus(0, 0);
		}

		char buf[100];

		int count = read(nClientSocket, buf, 100);
		if((count == -1) || (count == 0))
		{
			cout << "read error" << endl;
			throw CLStatus(0, 0);
		}

		if(write(nClientSocket, buf, count) == -1)
			cout << "write error" << endl;
		
		throw CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		if(nClientSocket != -1)
			close(nClientSocket);

		{
			CLEvent event("test_for_socket_tcpserver");
			event.Set();
		}

		if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
			cout << "in test destroy error" << endl;
	}

    return 0;
}