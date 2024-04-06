// chcs.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#ifndef ELVEES_CHCS_H__INCLUDED_
#define ELVEES_CHCS_H__INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER < 0x51E) // MS.NET 2003
#error Too old compiler! Use new one...
#endif

#pragma once

/////////////////////////////////////////////////////////////////////////////
// Change these values to use different versions

#define STRICT
#define WINVER          0x0500
#define _WIN32_WINNT    0x0500
#define _WIN32_IE       0x0501
#define _RICHEDIT_VER   0x0100

#define VC_EXTRALEAN	// Exclude rarely-used stuff from Windows headers

/////////////////////////////////////////////////////////////////////////////
// Win32

#include <winsock2.h>
#include <tchar.h>

#include <vfw.h>
#include <time.h>

#include <commctrl.h>
#include <shellapi.h>

#include <dbghelp.h>

/////////////////////////////////////////////////////////////////////////////
// ATL

#define _ATL_FREE_THREADED
#include <atlbase.h>

//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module

extern CComModule _Module;
#include <atlcom.h>
#include <atlwin.h>

// ATL OLEDB Consumer Templates header.
#include <oledb.h>
#include <oledberr.h>
#include <atldbcli.h>

#if _ATL_VER < 0x710
#error You have to use ATL ver 7.1 or above...
#endif

/////////////////////////////////////////////////////////////////////////////
// ElveesTools

#include "ElveesTools\ElveesTools.h"

/////////////////////////////////////////////////////////////////////////////
// Dll interfaces

#include "chcsva.h"
#include "Manager.h"
//class CManager;

extern CManager* GetManager();

/////////////////////////////////////////////////////////////////////////////
// Macroses

#define countof(x) (sizeof(x)/sizeof((x)[0]))

#ifdef UNICODE
typedef	WCHAR* UuidString;
#else
typedef	UCHAR* UuidString;
#endif

#define SAFE_RELEASE(x) \
	if(x)\
	{\
		(x)->Release();\
		(x) = NULL;\
	}

#define WIDTHBYTES(bits) ((DWORD)(((bits)+31) & (~31)) / 8)
#define DIBWIDTHBYTES(bi) (DWORD)WIDTHBYTES((DWORD)(bi).biWidth * (DWORD)(bi).biBitCount)
#define _DIBSIZE(bi) (DIBWIDTHBYTES(bi) * (DWORD)(bi).biHeight)
#define DIBSIZE(bi) ((bi).biHeight < 0 ? (-1)*(_DIBSIZE(bi)) : _DIBSIZE(bi))

bool StringToUuid(LPTSTR StringUuid, UUID* Uuid);

#endif // ELVEES_CHCS_H__INCLUDED_
