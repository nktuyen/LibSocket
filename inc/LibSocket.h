#ifndef __SOCKET_DEFINITION_H__
#define	__SOCKET_DEFINITION_H__
////////////////////////////////////////////////////
#if (defined(_WIN32) || defined(_WIN64))
// Includes ----------------------
#include <Winsock2.h>
#include <Windows.h>
// Macros ----------------------
// Socket errors

#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define ENAMETOOLONG            WSAENAMETOOLONG
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define ENOTEMPTY               WSAENOTEMPTY
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE

#define SHUT_RD					SD_RECEIVE
#define SHUT_WR					SD_SEND
#define SHUT_RDWR				SD_BOTH

#define	SocketHandle	SOCKET
#define CloseSocket(sd)	closesocket(sd)
#define ThreadHandle	HANDLE
#define InvalidThread	INVALID_HANDLE_VALUE
#define CloseThread(t)	if(InvalidThread != t){CloseHandle(t); t = InvalidThread;}

// Type definition --------------
typedef int	socklen_t;
#else
// Includes ----------------------
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
// Macros ----------------------

#define	SocketHandle		int
#define CloseSocket(sd)		close(sd)
#define ThreadHandle	pthread_t*
#define ThreadObject	pthread_t
#define InvalidThread	(ThreadHandle)nullptr
#define CloseThread(t)	if(InvalidThread != t){delete t; t = InvalidThread;}

#define Sleep(milisec)	sleep(milisec/1000)

// Type definition --------------
typedef unsigned short ADDRESS_FAMILY;
#endif


#define InvalidSocket		(SocketHandle)(~0)
#define SocketError			(int)(-1)
#undef  NULL
#define	WSAGetLastError()	errno

#define MAX_IP_ADDR_LEN		64

namespace t{
//////////////////////////////////////////////////////////////////////////	
	typedef	char				Char;
	typedef	unsigned char		UChar;
	typedef	short				Short;
	typedef unsigned short		UShort;
	typedef	int					Int;
	typedef	unsigned int		UInt;
	typedef	bool				Bool;
	typedef	long				Long;
	typedef	unsigned long		ULong;

enum ESocketShutdowFlag
{
	EShutdowReceive = SHUT_RD,
	EShutdowSend = SHUT_WR,
	EShutdowBoth = SHUT_RDWR
};

enum ESocketType
{
	ESocketUnknow = 0,
	ESocketStream = SOCK_STREAM,
	ESocketDataGram = SOCK_DGRAM,
	ESocketRaw = SOCK_RAW,
	ESocketRDM = SOCK_RDM,
	ESocketSeqPacket = SOCK_SEQPACKET,
};

enum EProtocol
{
	EProtocolUnknow = -1,
	EProtocolICMP = IPPROTO_ICMP,
	EProtocolIGMP = IPPROTO_IGMP,
	EProtocolTCP = IPPROTO_TCP,
	EProtocolUDP = IPPROTO_UDP,
	EProtocolIPv6 = IPPROTO_IPV6,
	EProtocolRaw = IPPROTO_RAW,
	EProtocolMax = IPPROTO_MAX
};

enum EAddressFamily
{
	EAddressFamilyUnspecified = AF_UNSPEC,
	EAddressFamilyIPv4 = AF_INET,
	EAddressFamilyIPv6 = AF_INET6,
};

enum ESocketError
{
	ESocketErrSuccess = 0,
	ESocketErrConnReset = ECONNRESET
};

enum ESocketOption
{
	ESocketOption_AcceptConn = SO_ACCEPTCONN,
	ESocketOption_Broadcast = SO_BROADCAST
};

//////////////////////////////////////////////////////////////////////////
};
///////////////////////////////////////////////////
#endif	//__SOCKET_DEFINITION_H__