#include "inc/TCPServer.h"

#define MAX_RECEIVE_BUFFER_SIZE	256
#define MAX_DUMP_ROW_SEPERATE	8
#define MAX_DUMP_ROW_WIDTH		16

TCPServer::TCPServer()
	: mSocket(t::EAddressFamilyIPv4, t::ESocketStream, t::EProtocolTCP)
{
	
}


TCPServer::~TCPServer()
{
	
}

bool TCPServer::onInitialize()
{
	WSADATA wsaData = { 0 };
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return false;
	}

	char name[255] = { 0 };
	int len = gethostname(name, 255);
	struct hostent* host = gethostbyname(name);
	char* ip = nullptr;

	if (host->h_addrtype == AF_INET)
	{
		sockaddr_in addr = { 0 };
		addr.sin_addr.s_addr = *(u_long *)host->h_addr_list[0];
		ip = inet_ntoa(addr.sin_addr);
		printf("Local address: %s\n", ip);
	}

	if (!mSocket.Create()) {
		return false;
	}

	if (!mSocket.Bind(ip, 20001)) {
		return false;
	}

	if (!mSocket.Listen(SOMAXCONN)) {
		return false;
	}

	printf("Listening...\n");

	return true;
}

void TCPServer::onRun()
{
	char buffer[MAX_RECEIVE_BUFFER_SIZE] = { 0 };
	int len = 0;
	while (isRunning())
	{
		if (mSocket.readyToReceive()) {
			t::Socket* pSocket = mSocket.Accept();
			if(nullptr != pSocket) {
				TCPConnection* pConn = new TCPConnection(pSocket);
				if(pConn->Create()) {
					mConnections.insert(std::make_pair(pSocket, pConn));
				}
				else {
					delete pConn;
					delete pSocket;
				}
			}
		}
	}
}

int TCPServer::onFinalize()
{
	mSocket.Shutdown(t::EShutdowBoth);
	return WSACleanup();
}


void TCPServer::Dump(char* data, int len)
{
	if ( (nullptr == data) || (0 >= len) ){
		printf("NULL\n");
		return;
	}

	for (int i = 1; i <= len; i++) {
		if (i % MAX_DUMP_ROW_SEPERATE == 0) {
			printf(" ");
		}
		if (i % MAX_DUMP_ROW_WIDTH == 0) {
			printf("\n");
		}
		printf("%d", (int)data[i]);
	}
}

void TCPServer::closeAllConnection()
{
	ConnectionIter ite = mConnections.begin();
	t::Socket* pSocket = nullptr;
	TCPConnection* pConn = nullptr;

	for(;ite != mConnections.end(); ++ite) {
		pSocket = ite->first;
		pConn = ite->second;

		if(nullptr != pConn) {
			pConn->Stop();
		}
	}
}