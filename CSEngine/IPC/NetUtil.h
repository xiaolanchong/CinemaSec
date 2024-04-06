#ifndef __NETUTIL_H__
#define __NETUTIL_H__

const char *	NET_ErrorString (int code);
double			Plat_FloatTime();

// Convert from host to network byte ordering
unsigned short NET_HostToNetShort( unsigned short us_in );
// and vice versa
unsigned short NET_NetToHostShort( unsigned short us_in );

#endif //__NETUTIL_H__