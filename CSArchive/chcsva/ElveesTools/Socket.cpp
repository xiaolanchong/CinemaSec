#include "Socket.h"
#include "Utils.h"

#include "assert.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees {

///////////////////////////////////////////////////////////////////////////////
// CSocket
///////////////////////////////////////////////////////////////////////////////

CSocket::CSocket() : m_socket(INVALID_SOCKET)
{
}

CSocket::CSocket(SOCKET theSocket) : m_socket(theSocket)
{
	assert(INVALID_SOCKET != m_socket);
}
      
CSocket::~CSocket()
{
	if(INVALID_SOCKET != m_socket)
		AbortiveClose();
}

bool CSocket::Create(int nAF, int nType, int nProtocol, LPWSAPROTOCOL_INFO lpInfo, DWORD dwFlags)
{
	if(INVALID_SOCKET != m_socket)
		AbortiveClose();

	m_socket = ::WSASocket(nAF, nType, nProtocol, lpInfo, 0, dwFlags);

	return (m_socket != INVALID_SOCKET);
}

void CSocket::Attach(SOCKET theSocket)
{
	assert(INVALID_SOCKET != theSocket);
	assert(INVALID_SOCKET == m_socket);

	m_socket = theSocket;
}

SOCKET CSocket::Detatch()
{
	SOCKET theSocket = m_socket;

	m_socket = INVALID_SOCKET;

	return theSocket;
}

bool CSocket::IsValid()
{
	return (m_socket != INVALID_SOCKET);
}

void CSocket::Close()
{
	if(0 != ::closesocket(m_socket))
		OutputF(TError, TEXT("CSocket::Close() failed Res=0x%lX"), ::WSAGetLastError());
}

void CSocket::AbortiveClose()
{
	LINGER lingerStruct;

	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;

	if(!SetSockOpt(SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct)))
		OutputF(TError, TEXT("CSocket::AbortiveClose() failed Res=0x%lX"), ::WSAGetLastError());

	Close();
}

bool CSocket::Shutdown(int how)
{
	if(0 != ::shutdown(m_socket, how))
	{
		OutputF(TError, TEXT("CSocket::Shutdown() failed Res=0x%lX"), ::WSAGetLastError());
		return false;
	}

	return true;
}

bool CSocket::Listen(int backlog)
{
	if(SOCKET_ERROR == ::listen(m_socket, backlog))
	{
		OutputF(TError, TEXT("CSocket::Listen() failed Res=0x%lX"), ::WSAGetLastError());
		return false;
	}

	return true;
}

bool CSocket::ReuseAddress(bool reuse /* = true */)
{
	BOOL reuseAddress = reuse;
	return SetSockOpt(SO_REUSEADDR, (char *)&reuseAddress, sizeof(BOOL));
}

bool CSocket::Bind(const SOCKADDR_IN &address)
{
	if(SOCKET_ERROR == ::bind(m_socket, reinterpret_cast<struct sockaddr *>(const_cast<SOCKADDR_IN*>(&address)), sizeof(SOCKADDR_IN)))
	{
		OutputF(TError, TEXT("CSocket::Bind() failed Res=0x%lX"), ::WSAGetLastError());
		return false;
	}

	return true;
}

bool CSocket::Bind(const struct sockaddr &address, unsigned addressLength)
{
	if(SOCKET_ERROR == ::bind(m_socket, const_cast<struct sockaddr *>(&address), int(addressLength)))
	{
		OutputF(TError, TEXT("CSocket::Bind() failed Res=0x%lX"), ::WSAGetLastError());
		return false;
	}

	return true;
}

bool CSocket::Connect(const SOCKADDR_IN &address)
{
	if(SOCKET_ERROR == ::connect(m_socket, reinterpret_cast<struct sockaddr *>(const_cast<SOCKADDR_IN*>(&address)), sizeof(SOCKADDR_IN)))
	{
		OutputF(TError, TEXT("CSocket::Conect(%hs) failed Res=0x%lX"),
			inet_ntoa(address.sin_addr), ::WSAGetLastError());

		return false;
	}

	return true;
}

bool CSocket::Connect(const struct sockaddr &address, unsigned addressLength)
{
	if(SOCKET_ERROR == ::connect(m_socket, const_cast<struct sockaddr *>(&address), int(addressLength)))
	{
		OutputF(TError, TEXT("CSocket::Conect() failed Res=0x%lX"), ::WSAGetLastError());
		return false;
	}

	return true;
}

bool CSocket::SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel)
{
	if(SOCKET_ERROR == ::setsockopt(m_socket, nLevel, nOptionName, (const char*)lpOptionValue, nOptionLen))
	{
		OutputF(TError,	TEXT("CSocket::SetSockOpt() failed Res=0x%lX"), ::WSAGetLastError());
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// CSocket::InternetAddress
///////////////////////////////////////////////////////////////////////////////

CSocket::InternetAddress::InternetAddress(
	const unsigned long address,
	const unsigned short port)
{
	this->sin_family = AF_INET;
	this->sin_port   = htons(port);
	this->sin_addr.s_addr = htonl(address);
}

CSocket::InternetAddress::InternetAddress(
	LPCTSTR lpAddress,
	const unsigned short port)
{
	const char *pAddress;

#ifndef UNICODE
	pAddress = lpAddress;
#else
	CHAR stAddress[MAX_PATH];
		
	WideCharToMultiByte(CP_ACP, 0, lpAddress, lstrlen(lpAddress) + 1,
		stAddress, MAX_PATH, NULL, NULL);

	pAddress = stAddress;
#endif
   
	sin_family = AF_INET;
	sin_port   = htons(port);
	sin_addr.s_addr = inet_addr(pAddress);

	if(this->sin_addr.s_addr == INADDR_NONE)
	{
		// address wasn't dotted ip, try to resolve name...
		HOSTENT *pHostEnt = ::gethostbyname(pAddress);

		if(pHostEnt)
			CopyMemory(&sin_addr, pHostEnt->h_addr_list[0], pHostEnt->h_length);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
