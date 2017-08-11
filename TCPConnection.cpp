#include "inc/TCPConnection.h"

#define MAX_RECEIVE_BUFFER_SIZE	256
#define MAX_DUMP_ROW_SEPERATE	8
#define MAX_DUMP_ROW_WIDTH		16

TCPConnection::TCPConnection(t::Socket* pSocket)
	: mSocket(pSocket)
{

}


TCPConnection::~TCPConnection(void)
{

}


bool TCPConnection::onInitialize()
{
	if( (nullptr == mSocket)  || (InvalidSocket == mSocket->getHandle()) ){
		return false;
	}

	printf("\nNew connection...");

	return true;
}

void TCPConnection::onRun()
{
	char buffer[MAX_RECEIVE_BUFFER_SIZE] = { 0 };
	int len = 0;
	while (isRunning())
	{
		if (InvalidSocket != mSocket->getHandle()) {
			if (mSocket->readyToReceive()) {
				memset(buffer, 0, MAX_RECEIVE_BUFFER_SIZE * sizeof(char));
				len = mSocket->Receive(buffer, MAX_RECEIVE_BUFFER_SIZE * sizeof(char), 0);
				if (0 < len) {
					Dump(buffer, len);
				}
			}
		}
	}
}

int TCPConnection::onFinalize()
{
	return mSocket->Shutdown(t::EShutdowBoth);
}

void TCPConnection::Dump(char* data, int len)
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