#include "inc/Socket.h"

#if (defined(_WIN32) || defined(_WIN64))
#pragma comment(lib, "Ws2_32.lib")
#endif

namespace t{
////////////////////////////////////////////
Socket::Socket(EAddressFamily af, ESocketType type, EProtocol proto)
: m_hSocket(InvalidSocket)
, m_Options(m_hSocket)
{
	m_addr.sin_family = af;
	m_eType = type;
	m_eProtocol = proto;
	m_nLastErr = 0;
	
	memset(&m_szLastError, 0, MAX_SOCKET_ERR_DESC);
}

Socket::~Socket(void)
{
	if (InvalidSocket != m_hSocket) {
		this->Shutdown(EShutdowBoth);
		this->Close();
	}
}


Socket* Socket::Accept()
{
	Socket* pSocket = nullptr;
	
	sockaddr_in remoteAddr = { 0 };
	socklen_t len = sizeof(remoteAddr);
	SocketMutex locker(&m_Locker);
	SocketHandle hSock = accept(m_hSocket, (sockaddr*)&remoteAddr, &len);
	if (InvalidSocket != hSock) {
		if (!this->onAccepting(hSock, remoteAddr)) {
			CloseSocket(hSock);
			return pSocket;
		}

		pSocket = new Socket(EAddressFamilyUnspecified, ESocketUnknow, EProtocolUnknow);
		if (nullptr != pSocket) {
			if (!pSocket->Attach(hSock, remoteAddr)) {
				CloseSocket(hSock);
				delete pSocket;
				pSocket = nullptr;
			}
			else {
				this->onAccepted(pSocket);
			}
		}
	}
	
	return pSocket;
}

bool Socket::Attach(SocketHandle hSocket, const sockaddr_in& addr)
{
	if (InvalidSocket != m_hSocket) {
		return false;
	}

	if (!onAttaching(hSocket, addr)) {
		return false;
	}

	m_hSocket = hSocket;

	m_Options.setSocketHandle(m_hSocket);

	this->onAttached();

	return true;
}

SocketHandle Socket::Detach()
{
	if (InvalidSocket == m_hSocket) {
		return InvalidSocket;
	}

	SocketHandle hSocket = m_hSocket;
	m_hSocket = InvalidSocket;
	this->onDetaching(hSocket);
	m_Options.setSocketHandle(InvalidSocket);
	m_eType = ESocketUnknow;
	m_eProtocol = EProtocolUnknow;
	memset(&m_addr, 0, sizeof(sockaddr));

	this->onDetached();

	return hSocket;
}

bool Socket::Bind(char* ip, unsigned short port)
{
	bool res = false;
	if (!this->onBinding(ip, port)) {
		return res;
	}

	sockaddr_in addr = { 0 };

	addr.sin_family = m_addr.sin_family;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);

	SocketMutex locker(&m_Locker);
	m_nLastErr = bind(m_hSocket, (sockaddr*)&addr, sizeof(addr));
	if (SocketError == m_nLastErr) {
		memset(m_szLastError, 0, MAX_SOCKET_ERR_DESC);
#if(defined(_WIN32) || defined(_WIN64) )
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(), 0, m_szLastError, MAX_SOCKET_ERR_DESC, nullptr);
#endif
		res = false;
	}
	else {
		this->onBound();
		res = true;
	}

	return res;
}

bool Socket::Connect(char* ip, unsigned short port)
{
	if (!this->onConnecting(ip, port)) {
		return false;
	}

	sockaddr_in remoteAddr = { 0 };

	remoteAddr.sin_family = static_cast<ADDRESS_FAMILY>(m_addr.sin_family);
	remoteAddr.sin_addr.s_addr = inet_addr(ip);
	remoteAddr.sin_port = htons(port);

	SocketMutex locker(&m_Locker);
	m_nLastErr = connect(m_hSocket, (sockaddr*)&remoteAddr, sizeof(remoteAddr));
	if (SocketError == m_nLastErr) {
		memset(m_szLastError, 0, MAX_SOCKET_ERR_DESC);
#if(defined(_WIN32) || defined(_WIN64) )
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(), 0, m_szLastError, MAX_SOCKET_ERR_DESC, nullptr);
#endif
		return false;
	}

	this->onConnected();

	return true;
}


bool Socket::Create()
{
	if(!this->onCreating(m_eType, m_eProtocol)) {
		return false;
	}

	SocketMutex locker(&m_Locker);
	m_hSocket = socket(static_cast<int>(m_addr.sin_family), static_cast<int>(m_eType), static_cast<int>(m_eProtocol));
	if (InvalidSocket == m_hSocket) {
		memset(m_szLastError, 0, MAX_SOCKET_ERR_DESC);
#if(defined(_WIN32) || defined(_WIN64) )
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(), 0, m_szLastError, MAX_SOCKET_ERR_DESC, nullptr);
#endif
		return false;
	}

	this->onCreated();

	return true;
}

bool Socket::Listen(int backlog)
{
	bool res = false;
	if (!this->onListening(backlog)) {
		return res;
	}

	SocketMutex locker(&m_Locker);
	m_nLastErr = listen(m_hSocket, backlog);
	if (SocketError == m_nLastErr) {
		memset(m_szLastError, 0, MAX_SOCKET_ERR_DESC);
#if(defined(_WIN32) || defined(_WIN64) )
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(), 0, m_szLastError, MAX_SOCKET_ERR_DESC, nullptr);
#endif
		res = false;
	}
	else {
		this->onListened();
		res = true;
	}

	return res;
}


int Socket::Receive(char* buffer, int len, int flags)
{
	if (!this->onReceiving(buffer, len, flags)) {
		return -1;;
	}

	SocketMutex locker(&m_Locker);
	int nRecvLen = recv(m_hSocket, buffer, len, flags);
	if (nRecvLen <= 0) {
		m_nLastErr = WSAGetLastError();
		memset(m_szLastError, 0, MAX_SOCKET_ERR_DESC);
#if(defined(_WIN32) || defined(_WIN64) )
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(), 0, m_szLastError, MAX_SOCKET_ERR_DESC, nullptr);
#endif
	}
	else {
		this->onReceived(buffer, len, flags);
	}

	return nRecvLen;
}

int Socket::receiveFrom(char* buffer, int len, int flags, char* ip, unsigned short port)
{
	if (!this->onReceivingFrom(buffer, len, flags, ip, port)) {
		return -1;
	}

	sockaddr_in remoteAddr = { 0 };
	socklen_t addrLen = sizeof(remoteAddr);
	remoteAddr.sin_family = m_addr.sin_family;
	remoteAddr.sin_addr.s_addr = inet_addr(ip);
	remoteAddr.sin_port = htons(port);

	SocketMutex locker(&m_Locker);
	int nRecvLen = recvfrom(m_hSocket, buffer, len, flags, (sockaddr*)&remoteAddr, &addrLen);

	if (nRecvLen <= 0) {
		m_nLastErr = WSAGetLastError();
		memset(m_szLastError, 0, MAX_SOCKET_ERR_DESC);
#if(defined(_WIN32) || defined(_WIN64) )
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(), 0, m_szLastError, MAX_SOCKET_ERR_DESC, nullptr);
#endif
	}
	else {
		this->onReceivedFrom(buffer, len, flags, ip, port);
	}

	return nRecvLen;
}

int Socket::Send(char* buffer, int len, int flags)
{
	if (!this->onSending(buffer, len, flags)) {
		return -1;
	}

	SocketMutex locker(&m_Locker);
	int nSent = send(m_hSocket, buffer, len, flags);
	if (nSent <= 0) {
		m_nLastErr = WSAGetLastError();
		memset(m_szLastError, 0, MAX_SOCKET_ERR_DESC);
#if(defined(_WIN32) || defined(_WIN64) )
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(), 0, m_szLastError, MAX_SOCKET_ERR_DESC, nullptr);
#endif
	}
	else {
		this->onSent(buffer, len, flags);
	}

	return nSent;
}

int Socket::sendTo(char* buffer, int len, int flags, char* ip, unsigned short port)
{
	if (!this->onSendingTo(buffer, len, flags, ip, port)) {
		return -1;
	}

	sockaddr_in remoteAddr = { 0 };
	socklen_t addrLen = sizeof(remoteAddr);
	remoteAddr.sin_family = m_addr.sin_family;
	remoteAddr.sin_addr.s_addr = inet_addr(ip);
	remoteAddr.sin_port = htons(port);

	SocketMutex locker(&m_Locker);
	int nSentTo = sendto(m_hSocket, buffer, len, flags, (sockaddr*)&remoteAddr, sizeof(remoteAddr));

	if (nSentTo <= 0) {
		m_nLastErr = WSAGetLastError();
		memset(m_szLastError, 0, MAX_SOCKET_ERR_DESC);
#if(defined(_WIN32) || defined(_WIN64) )
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(), 0, m_szLastError, MAX_SOCKET_ERR_DESC, nullptr);
#endif
	}
	else {
		this->onSentTo(buffer, len, flags, ip, port);
	}

	return nSentTo;
}

bool Socket::Shutdown(ESocketShutdowFlag how)
{
	bool res = false;
	if (!this->onShutingDown(how)) {
		return res;
	}

	SocketMutex locker(&m_Locker);
	m_nLastErr = shutdown(m_hSocket, how);
	if (SocketError == m_nLastErr) {
		memset(m_szLastError, 0, MAX_SOCKET_ERR_DESC);
#if(defined(_WIN32) || defined(_WIN64) )
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(), 0, m_szLastError, MAX_SOCKET_ERR_DESC, nullptr);
#endif
		res = false;
	}
	else {
		this->onShutDown(how);
		res = true;
	}

	return res;
}

bool Socket::Close()
{
	bool res = false;
	if (!this->onClosing()) {
		return res;
	}

	SocketMutex locker(&m_Locker);
	m_nLastErr = CloseSocket(m_hSocket);
	if (SocketError == m_nLastErr) {
		memset(m_szLastError, 0, MAX_SOCKET_ERR_DESC);
#if(defined(_WIN32) || defined(_WIN64) )
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(), 0, m_szLastError, MAX_SOCKET_ERR_DESC, nullptr);
#endif
		res = false;
	}
	else {
		m_hSocket = InvalidSocket;
		this->onClosed();
		res = true;
	}

	return res;
}

bool Socket::readyToReceive()
{
	if (InvalidSocket == m_hSocket) {
		return false;
	}

	int res = 0;
	fd_set	read;
	struct timeval to;
	to.tv_sec = 1;
	to.tv_usec = 1;

	FD_ZERO(&read);
	FD_SET(m_hSocket, &read);

	SocketMutex locker(&m_Locker);
	res = select(0, &read, nullptr, nullptr, &to);

	if ((res > 0) && (FD_ISSET(m_hSocket, &read))) {
		return true;
	}
	else {
		return false;
	}
}

bool Socket::readyToSend()
{
	if (InvalidSocket == m_hSocket) {
		return false;
	}

	int res = 0;
	fd_set	write;
	struct timeval to;
	to.tv_sec = 1;
	to.tv_usec = 1;

	FD_ZERO(&write);
	FD_SET(m_hSocket, &write);

	SocketMutex locker(&m_Locker);
	res = select(0, nullptr, &write, nullptr, &to);

	if ((res > 0) && (FD_ISSET(m_hSocket, &write))) {
		return true;
	}
	else {
		return false;
	}
}

bool Socket::readyToTransmition()
{
	if (InvalidSocket == m_hSocket) {
		return false;
	}

	int res = 0;
	fd_set	read, write;
	struct timeval to;
	to.tv_sec = 1;
	to.tv_usec = 1;

	FD_ZERO(&read);
	FD_ZERO(&write);
	FD_SET(m_hSocket, &read);
	FD_SET(m_hSocket, &write);

	SocketMutex locker(&m_Locker);
	res = select(0, &read, &write, nullptr, &to);

	if ((res > 0) && (FD_ISSET(m_hSocket, &read) || FD_ISSET(m_hSocket, &write))) {
		return true;
	}
	else {
		return false;
	}
}
////////////////////////////////////////////
};