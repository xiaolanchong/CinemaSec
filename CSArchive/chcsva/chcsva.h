// chcsva.h: common interfaces in chcsva.dll
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEES_CHCSVA_H__INCLUDED_
#define ELVEES_CHCSVA_H__INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifdef  CHCSVA_EXPORTS
#define CHCSVA_API __declspec(dllexport)
#else
#define CHCSVA_API __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////

#define INVALID_SOURCE_ID (-1)

//////////////////////////////////////////////////////////////////////
// Namespace: CHCS

namespace CHCS {

//////////////////////////////////////////////////////////////////////
// CHCS Frame Interface
//////////////////////////////////////////////////////////////////////

struct IFrame
{
public:
	virtual long AddRef() = 0;
	virtual long Release() = 0;

	// StreamID or -1 if undefined
	virtual long GetSourceID() = 0;

	// Current frame signal detection
	//   0 Horizontal sync is not locked.
	//   1 Horizontal sync is locked.
	//  -1 Can't be determine
	virtual long IsSignalLocked() = 0;

	// Time in ms
	// since stream start (period 2^32 ms ~= 49 days)
	virtual unsigned GetRelativeTime() = 0;

	// Get UNIX-style time in sec
	// See _time64 documentation for details
	virtual INT64 GetTime() = 0;

	// Copy frame format to your buffer
	// If pbi is zero, returns the size of the structure. 
	// If pbi is nonzero, returns -1 if an error occured.
	virtual long GetFormat(BITMAPINFO* pbi) = 0;

    // Return pointer to frame bytes
	// Valid untill final release
	// NOTE: read only
	virtual const void* GetBytes() = 0;
	
	// Draw frame
	virtual bool Draw(HDC hDC, long x, long y, long cx, long cy) = 0;

	// Copy frame data
	virtual bool CreateCopy(IFrame **ppFrame) = 0;
};

//////////////////////////////////////////////////////////////////////
// CHCS Stream Interface
//////////////////////////////////////////////////////////////////////

struct IStream
{
public:
	virtual long AddRef() = 0;
	virtual long Release() = 0;

	virtual bool Start() = 0;
	virtual void Stop() = 0;

	virtual bool IsStarted() = 0;

	virtual long SeekTo(INT64 timePos) = 0;

	// Wait for frame in stream
	// uTimeout = INFINITE not recommended
	virtual bool GetNextFrame(IFrame** ppFrame, unsigned uTimeout) = 0;
};

// Output system message level
//

#define CHCS_TRACE		0
#define CHCS_INFO		1
#define CHCS_WARNING	2
#define CHCS_ERROR		3
#define CHCS_CRITICAL 	4	// Need to restart if 

// Dll output function prototype
//  max message len is 1024

typedef void (__cdecl *OutputProc)(DWORD_PTR, int, LPCWSTR);

#define CHCS_MODE_NONE			0x00000000		// Do nothing

// basic functions
#define CHCS_MODE_DEVICENEW		0x00000001		// Install local devices (if any)
#define CHCS_MODE_DEVICE		0x00000002		// Init local devices
#define CHCS_MODE_STORE			0x00000004		// Save video to archive

// network functions
#define CHCS_MODE_NETSERVER		0x00000010		// Send video to network
#define CHCS_MODE_NETCLIENT		0x00000020		// Receive network streams

// Allow database operations, if not enabled
//  set stream id manually (for testing only)
#define CHCS_MODE_DATABASE		0x00000100		

// operating modes
#define CHCS_MODE_GUI			0x00001000		// Allow GUI
#define CHCS_MODE_DEBUG			0x10000000		// Force DEBUG output

// Complex modes
//

#define CHCS_MODE_SERVER		CHCS_MODE_NETSERVER | CHCS_MODE_STORE | CHCS_MODE_DEVICE | CHCS_MODE_DATABASE | CHCS_MODE_GUI
#define CHCS_MODE_CLIENT		CHCS_MODE_NETCLIENT | CHCS_MODE_DATABASE | CHCS_MODE_GUI

#define CHCS_MODE_ALGOTEST		CHCS_MODE_DEVICE | CHCS_MODE_DEBUG | CHCS_MODE_GUI | CHCS_MODE_STORE | CHCS_MODE_NETSERVER | CHCS_MODE_NETCLIENT

// Initialization struct
//

typedef struct tagINITCHCS {
	DWORD      dwSize;     // size of this structure
	DWORD      dwMode;     // operation mode
	DWORD      dwReserved; //
	DWORD_PTR  dwUserData; // output user data
	OutputProc procOutput; // output message function
} INITCHCS, *LPINITCHCS;

//////////////////////////////////////////////////////////////////////
// Dll access
//////////////////////////////////////////////////////////////////////

CHCSVA_API bool __stdcall InitStreamManager(LPINITCHCS lpInitCHCS);
CHCSVA_API bool __stdcall FreeStreamManager();

// Failed untill InitStreamManager call
CHCSVA_API bool __stdcall GetStreamByID(IStream** ppStream, long lStream);
CHCSVA_API bool __stdcall GetStreamByUID(IStream** ppStream, UUID* StreamUID);
CHCSVA_API bool __stdcall GetAvailableStreams(CAUUID* pUIDs);
CHCSVA_API bool __stdcall ShowStream(UUID* StreamUID, LPCWSTR stTitle);

CHCSVA_API bool __stdcall ShowStreamByNameA(LPCSTR stStream,  LPCSTR stTitle);
CHCSVA_API bool __stdcall ShowStreamByNameW(LPCWSTR stStream, LPCWSTR stTitle);

#ifdef _UNICODE
#define ShowStreamByName ShowStreamByNameW;
#else
#define ShowStreamByName ShowStreamByNameA;
#endif

CHCSVA_API DWORD __stdcall SetStreamDraw(LPCSTR stStream, HWND hWndOwner);
CHCSVA_API void  __stdcall KillStreamDraw(DWORD dwDraw);

CHCSVA_API bool  __stdcall StartArchiveStream(IStream** ppStream, long lStream, INT64 startPos);

// Functions prototypes
//

typedef bool (__stdcall *funcInitStreamManager)(LPINITCHCS);
typedef bool (__stdcall *funcFreeStreamManager)();
typedef bool (__stdcall *funcGetStreamByID) (IStream**, long);
typedef bool (__stdcall *funcGetStreamByUID)(IStream**, UUID*);
typedef bool (__stdcall *funcGetAvailableStreams)(CAUUID*);
typedef bool (__stdcall *funcShowStream)(UUID*, LPCWSTR);
typedef bool (__stdcall *funcStartArchiveStream)(IStream**, long, INT64);

} // End of namespace CHCS

//
// Example
//

#if 0
{
	HMODULE hCSVA = LoadLibrary(TEXT("chcsva.dll"));

	CHCS::funcInitStreamManager InitManager = 
			(CHCS::funcInitStreamManager)GetProcAddress(hCSVA, "InitStreamManager");

	CHCS::funcFreeStreamManager FreeManager = 
			(CHCS::funcFreeStreamManager)GetProcAddress(hCSVA, "FreeStreamManager");

	CHCS::funcGetAvailableStreams GetAvailableStreams = 
		(CHCS::funcGetAvailableStreams)GetProcAddress(hCSVA, "GetAvailableStreams");

	CHCS::funcGetStreamByUID GetStream = 
			(CHCS::funcGetStreamByUID)GetProcAddress(hCSVA, "GetStreamByUID");

	CHCS::INITCHCS initCHCS;
	initCHCS.dwSize = sizeof(initCHCS);
	initCHCS.dwMode = CHCS_MODE_SERVER;
	initCHCS.dwReserved = 0;
	initCHCS.dwUserData = 0;
	initCHCS.procOutput = NULL;	// Use OutputDebugString

	CAUUID stremsUIDs;
	CHCS::IFrame* pFrame;
	CHCS::IStream* pStream;

	do
	{
		if(!InitManager(&initCHCS))
			break;
	
		if(!GetAvailableStreams(&stremsUIDs))
			break;

		for(int i=0; i<stremsUIDs.cElems; i++)
		{
			if(GetStream(pStream, &stremsUIDs.pElems[i]))
			{
				// add to list or..

				for(int i=0; pStream->GetNextFrame(&pFrame, 100) && i<10; i++)
				{
					pFrame->Draw(HDC, 0, 0, 352, 288);
					pFrame->Release();
				}

				pStream->Release();
			}
		}

		// Release memory
		CoTaskMemFree(stremsUIDs.pElems);
	}
	while(false);

	FreeManager();
	FreeLibrary(hCSVA)
}
#endif

#endif // ELVEES_CHCSVA_H__INCLUDED_