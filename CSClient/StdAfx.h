// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__60E6F8BC_4A9A_4070_9DAA_63B958757029__INCLUDED_)
#define AFX_STDAFX_H__60E6F8BC_4A9A_4070_9DAA_63B958757029__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning( disable : 4786 )
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <gdiplus.h>
#include <afxmt.h>
#include <afxhtml.h>			// MFC support for Windows 95 Common Controls
#include <atlsafe.h>
#include <afxhtml.h>
#include <vfw.h>
#include <atlconv.h>
#pragma warning( default : 4786 )

using Gdiplus::Graphics;
using Gdiplus::Point;
using Gdiplus::PointF;
using Gdiplus::Rect;
using Gdiplus::RectF;
using Gdiplus::Color;
using Gdiplus::Bitmap;
using Gdiplus::Image;
using Gdiplus::Pen;
using Gdiplus::SolidBrush;
using Gdiplus::Font;
using Gdiplus::FontFamily;
//using namespace Gdiplus;

#include <vector>
#include <set>
#include <map>
#include <functional>

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include <strsafe.h>

#if _MSC_VER < 1300
#undef min
#undef max
#else
#include <algorithm>
using std::max;
using std::min;
#define _cpp_min	min
#define _cpp_max	max
#endif

#include "../CSUtility/csutility.h"
class ICameraAnalyzer;
class IHallAnalyzer;
#include "..\CSAlgo\interfaces\background_accum_interface.h"
#include "..\CSAlgo\algorithm\base_chair.h"
#include "../CSAlgo/CSAlgo2/csalgo2.h"

#include "res/resource.h"

#define MYMESSAGE_UPDATE_STATUSBAR_TEXT        (WM_USER+0)
#define MYMESSAGE_UPDATE_FRAME_INDICATOR       (WM_USER+1)
#define MYMESSAGE_UPDATE_FPS_INDICATOR         (WM_USER+2)
#define MYMESSAGE_UPDATE_CURSOR_INDICATOR      (WM_USER+3)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__60E6F8BC_4A9A_4070_9DAA_63B958757029__INCLUDED_)
