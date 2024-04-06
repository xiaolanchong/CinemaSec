#include "StdAfx.h"
#include ".\netaddr.h"

//=============================================================================

void NetadrToSockadr (netadr_t *a, struct sockaddr *s)
{
	memset (s, 0, sizeof(*s));

	if (a->type == NA_BROADCAST)
	{
		((struct sockaddr_in *)s)->sin_family = AF_INET;
		((struct sockaddr_in *)s)->sin_port = a->port;
		((struct sockaddr_in *)s)->sin_addr.s_addr = INADDR_BROADCAST;
	}
	else if (a->type == NA_IP)
	{
		((struct sockaddr_in *)s)->sin_family = AF_INET;
		((struct sockaddr_in *)s)->sin_addr.s_addr = *(int *)&a->ip;
		((struct sockaddr_in *)s)->sin_port = a->port;
	}
}

void SockadrToNetadr (struct sockaddr *s, netadr_t *a)
{
	if (s->sa_family == AF_INET)
	{
		a->type = NA_IP;
		*(int *)&a->ip = ((struct sockaddr_in *)s)->sin_addr.s_addr;
		a->port = ((struct sockaddr_in *)s)->sin_port;
	}
}


bool	NET_CompareAdr (netadr_t a, netadr_t b)
{
	if (a.type != b.type)
		return false;

	if (a.type == NA_LOOPBACK)
		return TRUE;

	if (a.type == NA_IP)
	{
		if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3] && a.port == b.port)
			return true;
		return false;
	}
	return false;
}

bool	NET_CompareClassBAdr (netadr_t a, netadr_t b)
{
	if (a.type != b.type)
		return false;

	if (a.type == NA_LOOPBACK)
		return true;

	if (a.type == NA_IP)
	{
		if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] )
			return true;
		return false;
	}
	return false;
}

// reserved addresses are not routeable, so they can all be used in a LAN game
bool	NET_IsReservedAdr (netadr_t a)
{
	if (a.type == NA_LOOPBACK)
		return true;

	if (a.type == NA_IP)
	{
		if ( (a.ip[0] == 10) ||										// 10.x.x.x is reserved
			(a.ip[0] == 127) ||									// 127.x.x.x 
			(a.ip[0] == 172 && a.ip[1] >= 16 && a.ip[1] <= 31) ||	// 172.16.x.x  - 172.31.x.x 
			(a.ip[0] == 192 && a.ip[1] >= 168) ) 					// 192.168.x.x
			return true;
		return false;
	}
	return false;
}
/*
===================
NET_CompareBaseAdr

Compares without the port
===================
*/
bool	NET_CompareBaseAdr (netadr_t a, netadr_t b)
{
	if (a.type != b.type)
		return false;

	if (a.type == NA_LOOPBACK)
		return TRUE;

	if (a.type == NA_IP)
	{
		if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3])
			return true;
		return false;
	}
	return false;
}

std::tstring NET_AdrToString (netadr_t a)
{
	TCHAR	s[64];

	memset(s, 0, 64 * sizeof(TCHAR) );

	if (a.type == NA_LOOPBACK)
		_sntprintf (s, sizeof( s )/sizeof(TCHAR), _T("loopback"));
	else if (a.type == NA_IP)
		_sntprintf (s, sizeof( s )/sizeof(TCHAR), _T("%i.%i.%i.%i:%i"), a.ip[0], a.ip[1], a.ip[2], a.ip[3], ntohs(a.port));

	return std::tstring(s);
}

std::string NET_AdrToStringA (netadr_t a)
{
	char	s[64];

	memset(s, 0, 64 * sizeof(char) );

	if (a.type == NA_LOOPBACK)
		_snprintf (s, sizeof( s )/sizeof(char), ("loopback"));
	else if (a.type == NA_IP)
		_snprintf (s, sizeof( s )/sizeof(char), ("%i.%i.%i.%i:%i"), a.ip[0], a.ip[1], a.ip[2], a.ip[3], ntohs(a.port));

	return std::string(s);
}

std::tstring NET_BaseAdrToString (netadr_t a)
{
	TCHAR	s[64];

	memset(s, 0, 64 * sizeof(TCHAR));

	if (a.type == NA_LOOPBACK)
		_sntprintf(s, sizeof( s )/sizeof(TCHAR), _T("loopback"));
	else if (a.type == NA_IP)
		_sntprintf(s, sizeof( s )/sizeof(TCHAR), _T("%i.%i.%i.%i"), a.ip[0], a.ip[1], a.ip[2], a.ip[3]);
	return std::tstring(s);
}

std::string	NET_BaseAdrToStringA (netadr_t a)
{
	char	s[64];
	memset(s, 0, 64 * sizeof(char));

	if (a.type == NA_LOOPBACK)
		_snprintf(s, sizeof( s )/sizeof(char), ("loopback"));
	else if (a.type == NA_IP)
		_snprintf(s, sizeof( s )/sizeof(char), ("%i.%i.%i.%i"), a.ip[0], a.ip[1], a.ip[2], a.ip[3]);
	return std::string(s);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *s - 
//			*sadr - 
// Output : bool	NET_StringToSockaddr
//-----------------------------------------------------------------------------
bool	NET_StringToSockaddr ( const TCHAR *s, struct sockaddr *sadr )
{
	struct hostent	*h;
	TCHAR	*colon;
	TCHAR	copy[128];

	memset (sadr, 0, sizeof(*sadr));
	((struct sockaddr_in *)sadr)->sin_family = AF_INET;
	((struct sockaddr_in *)sadr)->sin_port = 0;

	_tcscpy (copy, s);
	// strip off a trailing :port if present
	for (colon = copy ; *colon ; colon++)
		if (*colon == _T(':'))
		{
			*colon = 0;
			((struct sockaddr_in *)sadr)->sin_port = htons((short)_ttoi(colon+1));	
		}

		char temp[128];
		wcstombs( temp, copy, 128 );

		if (temp[0] >= ('0') && temp[0] <= ('9') && strstr( temp, (".") ) )
		{
			*(int *)&((struct sockaddr_in *)sadr)->sin_addr = inet_addr(temp);
		}
		else
		{
			if (! (h = gethostbyname(temp)) )
				return 0;
			*(int *)&((struct sockaddr_in *)sadr)->sin_addr = *(int *)h->h_addr_list[0];
		}

		return true;
}

/*
=============
NET_StringToAdr

localhost
idnewt
idnewt:28000
192.246.40.70
192.246.40.70:28000
=============
*/
bool	NET_StringToAdr ( const TCHAR *s, netadr_t *a)
{
	struct sockaddr sadr;

	if (!_tcscmp (s, _T("localhost")))
	{
		memset (a, 0, sizeof(*a));
		a->type = NA_LOOPBACK;
		return true;
	}

	if (!NET_StringToSockaddr (s, &sadr))
		return false;

	SockadrToNetadr (&sadr, a);

	return true;
}

bool	NET_StringToAdr ( const TCHAR *s, WORD port, netadr_t *a)
{
	bool res = NET_StringToAdr( s, a );
	a->port = htons(port);
	return res;
}

bool	NET_IsLocalAddress (netadr_t adr)
{
	return adr.type == NA_LOOPBACK;
}

bool	NET_GetOwnAddress( short port, netadr_t* a )
{
	struct hostent	*h;
	char Name[128];
	int res = gethostname( Name, 128 );
	if (! (h = gethostbyname(Name)) ) return false;
	a->type = NA_IP;
	*(int *)&a->ip = *(int *)h->h_addr_list[0];
	return true;
}
