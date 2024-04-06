#ifndef __NETSTRUCT_H__
#define __NETSTRUCT_H__

#include <vector>
#include <list>
#include "NetAddr.h"
#include "../../CSChair/Common/Exception.h"

MACRO_EXCEPTION(NetException, CommonException);
MACRO_EXCEPTION(NetSysErrorException, NetException);
MACRO_EXCEPTION(NetAssertException, NetException);

inline void Sys_Error (char *error, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	vsprintf (text, error, argptr);
	va_end (argptr);
#ifdef USE_INT3
	__asm int 3 
#endif
	throw NetSysErrorException(text);
}

inline void Sys_Debug (char *msg, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	vsprintf (text, msg, argptr);
	va_end (argptr);

	OutputDebugStringA(text);
}

#define Assert(x) {if( !(x) )  Sys_Error( "Assert exception: %s, %s", __FILE__, __LINE__);  }

typedef std::pair< netadr_t, std::vector<BYTE> >	NetMessage_t;
typedef std::list< NetMessage_t >					NetIncomeBuffer_t;

typedef	std::vector< netadr_t >						AddrBuffer_t;
/*
enum 
{
NS_SERVER = 0,
};*/

// This is the payload plus any header info (excluding UDP header)
// Pad this to next higher 16 byte boundary
// This is the largest packet that can come in/out over the wire, before processing the header
//  bytes will be stripped by the networking channel layer
#define	NET_MAX_PAYLOAD	80000


#define PAD_NUMBER(x, y)	( ( (x) + (y) ) & ~(y)) 

#define	NET_MAX_MESSAGE				PAD_NUMBER( NET_MAX_PAYLOAD, 16 )
#define MAX_ROUTEABLE_PACKET		1400
#define SPLIT_SIZE					(MAX_ROUTEABLE_PACKET - sizeof(SPLITPACKET))
#define NET_REAL_MAX_MESSAGE		MAX_SPLITPACKET_SPLITS * SPLIT_SIZE

// Split long packets.  Anything over 1460 is failing on some routers
typedef struct
{
	int		currentSequence;
	int		splitCount;
	int		totalSize;
	char	buffer[ NET_MAX_MESSAGE ];	// This has to be big enough to hold the largest message
} LONGPACKET;

// Use this to pick apart the network stream, must be packed
#pragma pack(1)
typedef struct
{
	__int32		magicID;			// magic number 'csnw'
	__int32		protoVersion;		// version of the network protocol
	__int32		netID;				// -2 = splitted packet, 0 - smth else
	__int32		sequenceNumber;		// underlayed proto packet
	__int32		packetID;			// MAKELONG( current packet number in sequence, packet total number  )
} SPLITPACKET;
#pragma pack()

#define MAX_SPLITPACKET_SPLITS ( NET_MAX_MESSAGE / SPLIT_SIZE )
#define SPLIT_PACKET_STALE_TIME		15.0f

#define	PORT_ANY	-1

#endif //__NETSTRUCT_H__