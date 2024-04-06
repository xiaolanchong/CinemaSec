// Created by Microsoft (R) C/C++ Compiler Version 13.10.3077
//
// d:\projects\cvmcad\sources\common.mrg.hpp
// compiler-generated file created 05/28/05 at 23:49:40
//
// This C++ source file is intended to be a representation of the
// source code injected by the compiler.  It may not compile or
// run exactly as the original source file.
//


//+++ Start Injected Code
[no_injected_text(true)];      // Suppress injected text, it has already been injected
#pragma warning(disable: 4543) // Suppress warnings about skipping injected text
#pragma warning(disable: 4199) // Suppress warnings from attribute providers

// M00PRAGMA("\n\nNOTE: This merged source file should be visually inspected for correctness.\n\n")
//--- End Injected Code



//+++ Start Injected Code
#injected_line 56 "d:\\projects\\cvmcad\\sources\\common.hpp"

#include <comdef.h>

//--- End Injected Code
/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Common.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-04-27
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0501		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <gdiplus.h>

//MSXML support
#import "msxml4.dll"
#include <msxml2.h>

//STL Support
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <sstream>
#include <list>
#include <utility>


//Boost support
#include <boost\scoped_ptr.hpp>
#include <boost\shared_ptr.hpp>

//


//DirectX Support
#define D3D_DEBUG_INFO
#include <math.h>
#include <d3d9.h>
#include <dxerr9.h>
#include <d3dx9.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "CommonTypes.hpp"

#endif //__COMMON_HPP__
