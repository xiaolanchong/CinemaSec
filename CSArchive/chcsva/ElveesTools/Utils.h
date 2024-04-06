#ifndef ELVEES_UTILS_INCLUDED__
#define ELVEES_UTILS_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#include <wctype.h>

//////////////////////////////////////////////////////////////////////////
// Debugging defines...
//////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_ONLY
#ifdef _DEBUG
#define DEBUG_ONLY(x)   x
#else
#define DEBUG_ONLY(x)
#endif
#endif

#ifndef _LPCBYTE_DEFINED
#define _LPCBYTE_DEFINED
typedef const BYTE *LPCBYTE;
#endif

//////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
//////////////////////////////////////////////////////////////////////////

namespace Elvees {

//////////////////////////////////////////////////////////////////////////
// Assistant functions
//////////////////////////////////////////////////////////////////////////

bool __cdecl IsFileExists(LPCTSTR stFile);

bool __cdecl HexToString(LPCBYTE pBuffer, int nBytes, LPTSTR lpDest, int cchDest);
bool __cdecl StringToHex(LPCTSTR lpSource, BYTE *pBuffer, int nBytes);

LPCTSTR __cdecl GetLastErrorMessage(DWORD dwError);
LPCTSTR __cdecl GetCurrentDirectory();
LPCTSTR __cdecl GetDateTimeStamp();

LPCTSTR __cdecl DumpData(LPCBYTE pData, 
		int dataLength, 
		int lineLength = 0);

LPCTSTR __cdecl GetComputerName();
LPCTSTR __cdecl GetUserName();

//////////////////////////////////////////////////////////////////////////
// Output functions
//////////////////////////////////////////////////////////////////////////

typedef enum
{
	TTrace,
	TWarning,
	TInfo,
	TError,
	TCritical
}
TOutput;

// Only prototype
extern void __cdecl Output(TOutput type, LPCTSTR stText);
void  __cdecl OutputF(TOutput type, LPCTSTR stFormat, ...);

} // End of namespace Elvees

#endif // ELVEES_UTILS_INCLUDED__
