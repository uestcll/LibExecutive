#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "LibExecutive.h"

using namespace std;

void NormalTCPClient()
{
	int nClientSocket = -1;
	try
	{
		nClientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if(-1 == nClientSocket)
		{
			cout << "socket error" << endl;
			return;
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
	}
}

void NormalTCPClientAcceptError()
{
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

		throw CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		if(nClientSocket != -1)
			close(nClientSocket);

		CLEvent event("test_for_socket_tcpserver_accept_error");
		event.Set();
	}
}

void TCPServer()
{
	int nListenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == nListenSocket)
	{
		std::cout << "socket error" << std::endl;
		return;
	}

	int optval = 1;
	if(setsockopt(nListenSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
	{
		std::cout << "setsockopt error" << std::endl;
		return;
	}

	sockaddr_in ServerAddress;
	memset(&ServerAddress, 0, sizeof(sockaddr_in));
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	ServerAddress.sin_port = htons(3600);

	if(::bind(nListenSocket, (sockaddr *)&ServerAddress, sizeof(sockaddr_in)) == -1)
	{
		std::cout << "bind error" << std::endl;
		::close(nListenSocket);
		return;
	}

	if(::listen(nListenSocket, 128) == -1)
	{
		std::cout << "listen error" << std::endl;
		::close(nListenSocket);
		return;
	}

	{
		CLEvent event("test_for_socket_tcpclient_notify");
		event.Set();
	}

	sockaddr_in ClientAddress;
	socklen_t LengthOfClientAddress = sizeof(sockaddr_in);
	int nConnectedSocket = ::accept(nListenSocket, (sockaddr *)&ClientAddress, &LengthOfClientAddress);
	if(-1 == nConnectedSocket)
	{
		std::cout << "accept error" << std::endl;
		::close(nListenSocket);
		return;
	}

	::write(nConnectedSocket, "Hello World\n", 13);

	::close(nConnectedSocket);
	::close(nListenSocket);

	return;
}

int main(int argc, char* argv[])
{
	if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
	{
		cout << "in test initialize error" << endl;
		return 0;
	}

	try
	{
		if(argv[1][0] == '1')
			NormalTCPClient();

		if(argv[1][0] == '2')
			NormalTCPClientAcceptError();

		if(argv[1][0] == '3')
			TCPServer();

		throw CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		{
			CLEvent event("test_for_socket_tcpserver");
			event.Set();
		}

		if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
			cout << "in test destroy error" << endl;
	}

    return 0;
}