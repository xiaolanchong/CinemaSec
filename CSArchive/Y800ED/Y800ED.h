// Y800ED.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef Y800ED_H__INCLUDED__
#define Y800ED_H__INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#define WINVER          0x0500
#define _WIN32_WINNT    0x0500

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <vfw.h>

/////////////////////////////////////////////////////////////////////////////
// FOURCC

// see http://www.fourcc.org for details
// fourcc

#define FOURCC_Y800		mmioFOURCC('Y','8','0','0')
#define FOURCC_Y8		mmioFOURCC('Y','8',' ',' ')
#define FOURCC_GRAY		mmioFOURCC('G','R','A','Y')

// Compressed Y8 (my fourcc)
//#define FOURCC_Y8ZL		mmioFOURCC('Y','8','Z','L')
#define FOURCC_Y8BZ		mmioFOURCC('Y','8','B','Z')

/////////////////////////////////////////////////////////////////////////////
// Macroses

#define countof(x) (sizeof(x)/sizeof((x)[0]))

#ifndef DEBUG_ONLY
#ifdef _DEBUG
#define DEBUG_ONLY(x)   x
#else
#define DEBUG_ONLY(x)
#endif
#endif

// To allow #pragma TODO(blah blah) to give a 'clickable' message in the build
// window.

#define TODOSTRINGIZE(L) #L
#define TODOMAKESTRING(M,L) M(L)
#define TODOLINE TODOMAKESTRING( TODOSTRINGIZE, __LINE__)

#define TODO(_msg) message(__FILE__ "(" TODOLINE ") : TODO : " _msg)
#define BUG(_msg)  message(__FILE__ "(" TODOLINE ") : BUG : " _msg)

/////////////////////////////////////////////////////////////////////////////
// Valiables

extern HMODULE g_hModuleY800;
extern BOOL    g_bDebuginfo;

#endif // Y800ED_H__INCLUDED__
