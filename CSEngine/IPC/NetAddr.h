#ifndef __NETADDR_H__
#define __NETADDR_H__

//#include <winsock.h>
#include <winsock2.h>
#include <string>

#if (defined _UNICODE) || defined(UNICODE)
#define tstring wstring
#else
#define tstring string
#endif

typedef enum
{ 
	NA_UNUSED,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP
} netadrtype_t;

typedef struct netadr_s
{
	netadrtype_t	type;
	unsigned char	ip[4];
	unsigned short	port;
	netadr_s() : type( NA_UNUSED ) {}
} netadr_t;

// Compare addresses
bool	NET_CompareAdr (netadr_t a, netadr_t b);
bool	NET_CompareClassBAdr (netadr_t a, netadr_t b);
bool	NET_IsReservedAdr (netadr_t a);
bool	NET_IsLocalAddress (netadr_t adr);
bool	NET_CompareBaseAdr (netadr_t a, netadr_t b);



// Address conversion
std::tstring	NET_AdrToString (netadr_t a);
std::string		NET_AdrToStringA (netadr_t a);
std::tstring	NET_BaseAdrToString (netadr_t a);
std::string		NET_BaseAdrToStringA (netadr_t a);
bool			NET_StringToAdr ( const TCHAR *s, netadr_t *a);
bool			NET_StringToAdr ( const TCHAR *s, WORD port, netadr_t *a);

void	SockadrToNetadr (struct sockaddr *s, netadr_t *a);
void	NetadrToSockadr (netadr_t *a, struct sockaddr *s);
bool	NET_StringToSockaddr ( const TCHAR *s, struct sockaddr *sadr );

bool	NET_GetOwnAddress( short port, netadr_t* a );

inline bool	NET_IsOwnAddress(const netadr_t& Na)
{
	netadr_t Own;
	NET_GetOwnAddress(0, &Own);
	return NET_CompareBaseAdr( Own, Na );
}

#endif //__NETADDR_H__