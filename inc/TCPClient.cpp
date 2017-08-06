#include "TCPClient.h"

#define MAX_RECEIVE_BUFFER_SIZE	256
#define MAX_DUMP_ROW_SEPERATE	8
#define MAX_DUMP_ROW_WIDTH		16

TCPClient::TCPClient(char* ip, unsigned short port)
	:mSocket(t::EAddressFamilyIPv4, t::ESocketStream, t::EProtocolTCP)
	, mServerPort(port)
{
	memset(mServerAddr, 0, MAX_IP_ADDR_LEN * sizeof(char));
	if (nullptr != ip) {
		memcpy(mServerAddr, ip, MAX_IP_ADDR_LEN * sizeof(char));
	}
}


TCPClient::~TCPClient()
{

}


bool TCPClient::onInitialize()
{
	WSADATA wsaData = { 0 };
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return false;
	}

	if (!mSocket.Create()) {
		return false;
	}

	if (!mSocket.Connect(mServerAddr, mServerPort)) {
		return false;
	}

	return true;
}

void TCPClient::onRun()
{
	char buffer[MAX_RECEIVE_BUFFER_SIZE] = { 0 };
	int len = 0;
	while (isRunning())
	{
		if (InvalidSocket != mSocket.getHandle()) {
			//Check alive status
			mLocker.lock();
			if (mSocket.readyToSend()) {
				memset(buffer, 0, MAX_RECEIVE_BUFFER_SIZE * sizeof(char));
				int err = mSocket.Send(buffer, 1, 0);
				if (ECONNRESET == err) {
					this->onDisconnected();
					mLocker.unlock();
					continue;
				}
			}
			mLocker.unlock();

			if (mSocket.readyToReceive()) {
				memset(buffer, 0, MAX_RECEIVE_BUFFER_SIZE * sizeof(char));
				len = mSocket.Receive(buffer, MAX_RECEIVE_BUFFER_SIZE * sizeof(char), 0);
				if (0 < len) {
					Dump(buffer, len);
				}
			}
		}
	}
}

int TCPClient::onFinalize()
{
	mSocket.Shutdown(t::EShutdowBoth);
	return WSACleanup();
}

void TCPClient::onDisconnected()
{
	printf("onDisconnected()\n");

	mSocket.Shutdown(t::EShutdowBoth);
	mSocket.Close();
}


void TCPClient::Dump(char* data, int len)
{
	if ((nullptr == data) || (0 >= len)) {
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

int TCPClient::Send(char* buffer, int len)
{
	int res = 0;
	mLocker.lock();
	if (mSocket.readyToSend()) {
		res = mSocket.Send(buffer, len, 0);
	}
	else {
		res = 0;
	}
	mLocker.unlock();

	return res;
}