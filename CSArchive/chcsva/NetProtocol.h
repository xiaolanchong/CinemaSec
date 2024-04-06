// NetProtocol.h
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define makeSynchro(ch0, ch1, ch2, ch3) \
	( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |	\
	( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

// Synchronization data
// Protocol version depent

#define CHCS_NET_SYNCHRO makeSynchro('C', 'S', '1', '5')

#define CHCS_NET_SYNCHRO_B1 (BYTE)( CHCS_NET_SYNCHRO & 0x000000FF)
#define CHCS_NET_SYNCHRO_B2 (BYTE)((CHCS_NET_SYNCHRO & 0x0000FF00) >> 8)
#define CHCS_NET_SYNCHRO_B3 (BYTE)((CHCS_NET_SYNCHRO & 0x00FF0000) >> 16)
#define CHCS_NET_SYNCHRO_B4 (BYTE)((CHCS_NET_SYNCHRO & 0xFF000000) >> 24)

// Packet type
#define NET_PACKET_STREAMINFO	0x00000001
#define NET_PACKET_STREAMDATA	0x00000002

// typical packet header
typedef struct tagNETPACKETHEADER {
	unsigned uSynchro;		// Synchronization data
	unsigned uTime;			// Packet Time (or random number)
	unsigned uPacket;		// Packet ID
	unsigned uLength;		// Length of data with headers
	unsigned uCheckSum;		// Check sum
} NetPacketHeader, *PNetPacketHeader;

// Video stream info header
typedef struct tagNETSTREAMINFO {
	UUID     uStreamUID;	// Unique ID
	unsigned uFOURCC;
	unsigned uFormatSize;
} NetStreamInfo, *PNetStreamInfo;

// Video frame data header
typedef struct tagNETSTREAMDATAINFO {
	UUID     uStreamUID;	// Unique ID
	unsigned uFrameSeq;		// MAKELONG(Frame num = 0 -> I-Frame, I-Frame sequence)
	unsigned uFrameSize;	// Size of data
} NetStreamDataInfo, *PNetStreamDataInfo;

// Const prototypes
typedef const NetPacketHeader*   PCNetPacketHeader;
typedef const NetStreamInfo*     PCNetStreamInfo;
typedef const NetStreamDataInfo* PCNetStreamDataInfo;