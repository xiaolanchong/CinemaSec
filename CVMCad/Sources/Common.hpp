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


// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0501		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0500		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0510 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0500	// Change this to the appropriate value to target IE 5.0 or later.
#endif

// turns off MFC's hiding of some common and often safely ignored warning messages
//#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

//CrashDebugInfo
#include <tchar.h>
#include <dbghelp.h>

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <gdiplus.h>

//MSXML support
#include <msxml2.h>
#import <msxml2.dll>

//PropTree support
#include <PropTree.h>

//Shell support
#include <shlwapi.h>

//ATL support
#include<atlimage.h>


//STL Support
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <sstream>
#include <list>
#include <utility>
#include <map>
#include <iomanip>
#include <exception>
#include <new>

//Boost support
#include <boost\scoped_ptr.hpp>
#include <boost\shared_ptr.hpp>

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
