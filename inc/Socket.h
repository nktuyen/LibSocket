#ifndef __SOCKET_H__
#define	__SOCKET_H__
//////////////////////////////////////////
#include <mutex>
#include "SocketOptions.h"

namespace t{
//////////////////////////////////////////////////////////////////////////
#define MAX_SOCKET_ERR_DESC	255
class Socket
{
private:
	SocketOptions m_Options;
	SocketHandle m_hSocket;
	EAddressFamily m_eAddressFamily;
	EProtocol m_eProtocol;
	ESocketType m_eType;
	int m_nLastErr;
	char m_szLastError[MAX_SOCKET_ERR_DESC];
	std::mutex m_Locker;
public:
	explicit Socket(EAddressFamily af, ESocketType type, EProtocol proto);
	virtual ~Socket();
public:
	Socket* Accept();
	bool Attach(EAddressFamily af, SocketHandle hSocket, char* ip, UShort port);
	SocketHandle Detach();
	bool Bind(char* ip, unsigned short port);
	bool Connect(char* ip, unsigned short port);
	bool Create();
	int lastError() { return m_nLastErr; }
	char* lastErrorDesc() { return m_szLastError; }
	bool Listen(int backlog);
	int Receive(char* buffer, int len, int flags);
	int receiveFrom(char* buffer, int len, int flags, char* ip, unsigned short port);
	int Send(char* buffer, int len, int flags);
	int sendTo(char* buffer, int len, int flags, char* ip, unsigned short port);
	bool Shutdown(ESocketShutdowFlag how);
	bool readyToReceive();
	bool readyToSend();
	bool readyToTransmition();
	bool Close();
public:
	inline SocketHandle getHandle() { return m_hSocket; }
	inline const SocketOptions& Options() { return m_Options; }
	inline ESocketType getType() { return m_eType; }
	inline EAddressFamily getAddressFamily() { return m_eAddressFamily; }
	inline EProtocol getProtocol() { return m_eProtocol; }
protected:
	virtual void onAccepted(EAddressFamily af, char* ip, UShort por, Socket* pSocket) { ; }
	virtual bool onAccepting() { return true; }
	virtual void onAttached() { ; }
	virtual bool onAttaching(EAddressFamily af, SocketHandle hSocket, char* ip, UShort port) { return true; }
	virtual bool onBinding(char* ip, unsigned short port) { return true; }
	virtual void onBound() { ; }
	virtual void onConnected() { ; }
	virtual bool onConnecting(char* ip, unsigned short port) { return true; }
	virtual void onCreated() { ; }
	virtual bool onCreating(EAddressFamily af, ESocketType type, EProtocol proto) { return true; }
	virtual void onDetaching(SocketHandle hSocket) { ; }
	virtual void onDetached() { ; }
	virtual void onListened() { ; }
	virtual bool onListening(int backlog) { return true; }
	virtual void onReceived(char* buffer, int len, int flags) { ; }
	virtual void onReceivedFrom(char* buffer, int len, int flags, char* ip, unsigned short port) { ; }
	virtual bool onReceiving(char* buffer, int len, int flags) { return true; }
	virtual bool onReceivingFrom(char* buffer, int len, int flags, char* ip, unsigned short port) { return true; }
	virtual bool onSending(char* buffer, int len, int flags) { return true; }
	virtual bool onSendingTo(char* buffer, int len, int flags, char* ip, unsigned short port) { return true; }
	virtual void onSent(char* buffer, int len, int flags) { ; }
	virtual void onSentTo(char* buffer, int len, int flags, char* ip, unsigned short port) { ; }
	virtual void onShutDown(int how) { ; }
	virtual bool onShutingDown(ESocketShutdowFlag how) { return true; }
	virtual bool onClosing() { return true; }
	virtual void onClosed() { ; }
protected:
};

class SocketMutex
{
	friend class Socket;
private:
	std::mutex* pMutex;
	SocketMutex(std::mutex* mutex) {
		this->pMutex = mutex;
		if(nullptr != this->pMutex) {
			(*this->pMutex).lock();
		}
	}
	~SocketMutex() {
		if(nullptr != this->pMutex) {
			(*this->pMutex).unlock();
		}
	}

	void Lock() {
		if(nullptr != this->pMutex) {
			(*this->pMutex).lock();
		}
	}

	void UnLock() {
		if(nullptr != this->pMutex) {
			(*this->pMutex).unlock();
		}
	}

	bool TryLock() {
		if(nullptr != this->pMutex) {
			return (*this->pMutex).try_lock();
		}

		return false;
	}
};
//////////////////////////////////////////////////////////////////////////
};
//////////////////////////////////////////////////////////////////////////
#endif	//__SOCKET_H__