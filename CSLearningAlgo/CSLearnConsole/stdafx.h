// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

/*
#pragma once


#include <iostream>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
*/

#pragma once

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // some CString constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN    // Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER          // Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0500   // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0500 // Change this to the appropriate value to target Windows 2000 or later.
#endif            

#ifndef _WIN32_WINDOWS        // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0500 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE         // Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400  // Change this to the appropriate value to target IE 5.0 or later.
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>   // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT



#include <string>
#include <vector>
#include <memory>

#include <mmsystem.h>
#include <afxmt.h>
#include <conio.h>
#include <direct.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <shlwapi.h>

#include "../../CSUtility/csutility.h"
#include "../../CSChair/ChairSerializer.h"
#include "../../CSalgo/CSAlgoCommon/CSAlgoCommon.h"


// TODO: reference additional headers your program requires here
