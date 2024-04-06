// HtmlViewEx.cpp : implementation file
//
// This class is an extended and enhanced version of CHtmlView
// designed originally for hosting HTML forms.  I am sure there
// are many other uses for this class, but it was designed with
// my own application's needs in mind.
//
// While heavily modified, this code still bears some resemblance
// to the CHtmlView code from the Microsoft Foundation Classes
// Library which are:
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.  
//
// The rest of the code is: 
// Copyright (C) 1999-2000 Infinity Networking Solutions, 
// All Rights Reserved.
//
// See the file "License.txt" for more information.
// 
// This code was designed for and compiles under Microsoft Visual
// C++ Version 6.0 SP4.  There are no guarantees that it will
// compile in any other environment.
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
//#include "Resource.h"

#include <afxdisp.h>		// MFC IDispatch & ClassFactory support
#include <atlconv.h>		// ATL UNICODE<->ANSI translation helpers
#include <mshtml.h>			// IE/MSHTML Interface Definitions
#include <mshtmdid.h>		// Standard IE/MSHTML DISPID Definitions
#include <idispids.h>		// More IE/MSHTML DISPID Definitions
#include <exdispid.h>		// DISPIDs for IExplorer Events
#include <mshtmcid.h>		// IE/MSHTML Command IDs
#include <initguid.h>
#include <mshtmhst.h>

#include "HtmlViewEx.h"

#undef AFX_DATA
#define AFX_DATA AFX_DATA_IMPORT
#if _MSC_VER > 1200
#include <afxocc.h>
#else
#include <..\src\occimpl.h>
#endif
#undef AFX_DATA
#define AFX_DATA AFX_DATA_EXPORT
#include "HtmlViewExSite.h"

// Names for OnAdvancedContextMenu() ID values 
// Comment out the ones in your mshtmhst.h
////////////////////////////////////////////////////

//	#define CONTEXT_MENU_DEFAULT        0
//	#define CONTEXT_MENU_IMAGE          1
//	#define CONTEXT_MENU_CONTROL        2
//	#define CONTEXT_MENU_TABLE          3
//	#define CONTEXT_MENU_TEXTSELECT     4
//	#define CONTEXT_MENU_ANCHOR         5
//	#define CONTEXT_MENU_UNKNOWN        6

	#define CONTEXT_MENU_VSCROLL        10
	#define CONTEXT_MENU_HSCROLL        11

/////////////////////////////////////////////////////////////////////////////
// These are not needed, why do people keep including them?
// #import <mshtml.tlb>
// #import <shdocvw.dll> exclude("tagREADYSTATE")
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif //_DEBUG

#ifdef _DEBUG
// Uncomment to see DISPID messages for OnAmbientProperties
//#define _TRACE_AMB_PROPRERIES
#endif //_DEBUG

#include "Globals.h"

/////////////////////////////////////////////////////////////////////////////
// Define an app: protocol for catching navigation events that you want
// to be processed by the application.  Override the OnAppCmd() to use
// this feature.
//
// Another great idea from the mind of Paul Dilascia!
//
#define APP_PROTOCOL _T("app:")

/////////////////////////////////////////////////////////////////////////////
// Define the default User Agent String, 
// just in case you don't set it to anything.
// You can change this in your application using
// SetUserAgent()
//
#ifndef DEF_HTMLFORM_USERAGENT
	#define DEF_HTMLFORM_USERAGENT _T("CHtmlViewEx 1.0")
#endif

/////////////////////////////////////////////////////////////////////////////
// Define the command ID for the web browser
// for issuing the WebBrowser commands
//
//#ifndef CGID_IWebBrowser
// CGID_WebBrowser: {ED016940-BD5B-11cf-BA4E-00C04FD70816}
DEFINE_GUID(CGID_IWebBrowser,0xED016940L,0xBD5B,0x11cf,0xBA,0x4E,0x00,0xC0,0x4F,0xD7,0x08,0x16);
//#endif

/////////////////////////////////////////////////////////////////////////////
// Helper function to issue the WebBrowser commands
//
void CHtmlViewEx::ExecCmdTarget(DWORD nCmdID)
{
	if (m_pBrowserApp != NULL)
	{
		LPOLECOMMANDTARGET lpTarget = NULL;
		LPDISPATCH lpDisp = GetHtmlDocument();

		if (lpDisp != NULL)
		{
			if (SUCCEEDED(lpDisp->QueryInterface(IID_IOleCommandTarget,
					(LPVOID*) &lpTarget)))
			{
				// Invoke the given command id for the WebBrowser control
				if(SUCCEEDED(lpTarget->Exec(&CGID_IWebBrowser, nCmdID, 0, NULL, NULL)))
				{
					#ifdef _DEBUG
					TRACE(_T("ExecCmdTarget Successful\n"));
					#endif // _DEBUG
				}
				else
				{
					#ifdef _DEBUG
					TRACE(_T("ExecCmdTarget Unsuccessful\n"));
					#endif // _DEBUG
				}
				lpTarget->Release();
			}
			lpDisp->Release();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewEx Construction/Destruction

CHtmlViewEx::CHtmlViewEx()
	: CFormView((LPCTSTR) NULL)
{
	m_nFontSize = -1;
	m_pBrowserApp = NULL;
	m_pHtmlViewExOccManager = NULL;
	m_bNoStatusText = FALSE;

	// These are the original defaults anyway
	m_bOfflineIfNotConnected = FALSE;
	m_bSilentMode = FALSE;

	m_strUserAgent = DEF_HTMLFORM_USERAGENT;
}

CHtmlViewEx::~CHtmlViewEx()
{
	// NOTE: DON'T delete m_pHtmlViewExOccManager -
	// it is deleted automatically by the framework!

	if (m_pBrowserApp != NULL)
		m_pBrowserApp->Release();
}

IMPLEMENT_DYNCREATE(CHtmlViewEx, CFormView)

BEGIN_MESSAGE_MAP(CHtmlViewEx, CFormView)
	//{{AFX_MSG_MAP(CHtmlViewEx)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(IDM_VIEWSOURCE, OnViewSource)
	ON_COMMAND(IDM_OPTIONS, OnToolsInternetOptions)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy) 
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste) 
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectall)
	ON_COMMAND(IDM_FIND, OnEditFindOnThisPage)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CHtmlViewEx, CFormView)
	//{{AFX_EVENTSINK_MAP(CHtmlViewEx)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 102 /* StatusTextChange */, OnStatusTextChange, VTS_BSTR)
	//																				103		 DISPID_QUIT
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 104 /* DownloadComplete */, OnDownloadComplete, VTS_NONE)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 105 /* CommandStateChange */, OnCommandStateChange, VTS_I4 VTS_BOOL)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 106 /* DownloadBegin */, OnDownloadBegin, VTS_NONE)
  //																				107    DISPID_NEWWINDOW
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 108 /* ProgressChange */, OnProgressChange, VTS_I4 VTS_I4)
	//																				109    DISPID_WINDOWMOVE
	//																				110		 DISPID_WINDOWRESIZE
	//																				111    DISPID_WINDOWACTIVATE
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 112 /* PropertyChange */, OnPropertyChange, VTS_BSTR)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 113 /* TitleChange */, OnTitleChange, VTS_BSTR)
	//																				114    DISPID_TITLEICONCHANGE
	//																				200    DISPID_FRAMEBEFORENAVIGATE
	//																				201    DISPID_FRAMENAVIGATECOMPLETE
	//																				204    DISPID_FRAMENEWWINDOW

	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 250 /* BeforeNavigate2 */, BeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 251 /* NewWindow2 */, OnNewWindow2, VTS_PDISPATCH VTS_PBOOL)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 252 /* NavigateComplete2 */, NavigateComplete2, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 253 /* OnQuit */, OnQuit, VTS_NONE)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 254 /* OnVisible */, OnVisible, VTS_BOOL)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 255 /* OnToolBar */, OnToolBar, VTS_BOOL)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 256 /* OnMenuBar */, OnMenuBar, VTS_BOOL)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 257 /* OnStatusBar */, OnStatusBar, VTS_BOOL)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 258 /* OnFullScreen */, OnFullScreen, VTS_BOOL)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 259 /* DocumentComplete */, DocumentComplete, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 260 /* OnTheaterMode */, OnTheaterMode, VTS_BOOL)

	// These events are not fully documented...
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 261 /* OnAddressBar */, OnAddressBar, VTS_BOOL)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 262 /* WindowSetResizable */, OnWindowSetResizable, VTS_BOOL)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 263 /* WindowClosing */, OnWindowClosing, VTS_BOOL VTS_PBOOL)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 264 /* WindowSetLeft */, OnWindowSetLeft, VTS_I4)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 265 /* WindowSetTop */, OnWindowSetLeft, VTS_I4)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 266 /* WindowSetWidth */, OnWindowSetLeft, VTS_I4)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 267 /* WindowSetHeight */, OnWindowSetLeft, VTS_I4)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 268 /* ClientToHostWindow */, OnClientToHostWindow, VTS_PI4 VTS_PI4)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 269 /* SetSecureLockIcon */, OnSetSecureLockIcon, VTS_I4)
	ON_EVENT(CHtmlViewEx, AFX_IDW_PANE_FIRST, 270 /* FileDownload */, OnFileDownload, VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewEx diagnostics

#ifdef _DEBUG
void CHtmlViewEx::AssertValid() const
{
	// TODO: add object validation code here
	// or call the base class
	CFormView::AssertValid();
}

void CHtmlViewEx::Dump(CDumpContext& dc) const
{
	// TODO: add object dumping code here
	// or call the base class
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewEx drawing

void CHtmlViewEx::OnDraw(CDC* /* pDC */)
{
	// this class should never do its own drawing;
	// the browser control should handle everything
	ASSERT(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewEx printing

void CHtmlViewEx::OnFilePrint()
{
	// get the HTMLDocument

	if (m_pBrowserApp != NULL)
	{
		LPOLECOMMANDTARGET lpTarget = NULL;
		LPDISPATCH lpDisp = GetHtmlDocument();

		if (lpDisp != NULL)
		{
			// the control will handle all printing UI

			if (SUCCEEDED(lpDisp->QueryInterface(IID_IOleCommandTarget,
					(LPVOID*) &lpTarget)))
			{
				lpTarget->Exec(NULL, OLECMDID_PRINT, 0, NULL, NULL);
				lpTarget->Release();
			}
			lpDisp->Release();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CHtmlViewEx message handlers

void CHtmlViewEx::OnDestroy()
{
	RELEASE(m_pBrowserApp);
}

void CHtmlViewEx::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndBrowser.m_hWnd))
	{
		// need to push non-client borders out of the client area
		CRect rect;
		GetClientRect(rect);
		::AdjustWindowRectEx(rect,
			m_wndBrowser.GetStyle(), FALSE, WS_EX_CLIENTEDGE);
		m_wndBrowser.SetWindowPos(NULL, rect.left, rect.top,
			rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CHtmlViewEx::OnPaint()
{
	Default();
}

BOOL CHtmlViewEx::OnEraseBkgnd(CDC* pDC) 
{ 
	UNREFERENCED_PARAMETER(pDC); 
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewEx operations

BOOL CHtmlViewEx::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
						DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
						UINT nID, CCreateContext* pContext)
{
	// create the view window itself
	m_pCreateContext = pContext;
	if (!CView::Create(lpszClassName, lpszWindowName,
				dwStyle, rect, pParentWnd,  nID, pContext))
	{
		return FALSE;
	}

	// Set our control containment up but using our control container 
	// management class instead of MFC's default
	m_pHtmlViewExOccManager = new CHtmlViewExOccManager();

	AfxEnableControlContainer(m_pHtmlViewExOccManager);

	RECT rectClient;
	GetClientRect(&rectClient);

	// create the control window
	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	if (!m_wndBrowser.CreateControl(CLSID_WebBrowser, lpszWindowName,
				WS_VISIBLE | WS_CHILD, rectClient, this, AFX_IDW_PANE_FIRST))
	{
		DestroyWindow();
		return FALSE;
	}

	// Hook up our WebBrowser2 interface
	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
	if (!SUCCEEDED(hr))
	{
		m_pBrowserApp = NULL;
		m_wndBrowser.DestroyWindow();
		DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewEx properties

CString CHtmlViewEx::GetType() const
{
	ASSERT(m_pBrowserApp != NULL);

	BSTR bstr;
	m_pBrowserApp->get_Type(&bstr);
	CString retVal(bstr);
	return retVal;
}

long CHtmlViewEx::GetLeft() const
{
	ASSERT(m_pBrowserApp != NULL);

	long result;
	m_pBrowserApp->get_Left(&result);
	return result;
}


long CHtmlViewEx::GetTop() const
{
	ASSERT(m_pBrowserApp != NULL);
	long result;
	m_pBrowserApp->get_Top(&result);
	return result;
}

int CHtmlViewEx::GetToolBar() const
{
	ASSERT(m_pBrowserApp != NULL);
	int result;
	m_pBrowserApp->get_ToolBar(&result);
	return result;
}

long CHtmlViewEx::GetHeight() const
{
	ASSERT(m_pBrowserApp != NULL);
	long result;
	m_pBrowserApp->get_Height(&result);
	return result;
}

BOOL CHtmlViewEx::GetVisible() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_Visible(&result);
	return result;
}

CString CHtmlViewEx::GetLocationName() const
{
	ASSERT(m_pBrowserApp != NULL);

	BSTR bstr;
	m_pBrowserApp->get_LocationName(&bstr);
	CString retVal(bstr);
	return retVal;
}

CString CHtmlViewEx::GetLocationURL() const
{
	ASSERT(m_pBrowserApp != NULL);

	BSTR bstr;
	m_pBrowserApp->get_LocationURL(&bstr);
	CString retVal(bstr);
	return retVal;
}

BOOL CHtmlViewEx::GetBusy() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_Busy(&result);
	return result;
}

READYSTATE CHtmlViewEx::GetReadyState() const
{
	ASSERT(m_pBrowserApp != NULL);

	READYSTATE result;
	m_pBrowserApp->get_ReadyState(&result);
	return result;
}

BOOL CHtmlViewEx::GetOffline() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_Offline(&result);
	return result;
}

BOOL CHtmlViewEx::GetSilent() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_Silent(&result);

	return result;
}

LPDISPATCH CHtmlViewEx::GetApplication() const
{
	ASSERT(m_pBrowserApp != NULL);

	LPDISPATCH result;
	m_pBrowserApp->get_Application(&result);
	return result;
}


LPDISPATCH CHtmlViewEx::GetParentBrowser() const
{
	ASSERT(m_pBrowserApp != NULL);

	LPDISPATCH result;
	m_pBrowserApp->get_Parent(&result);
	return result;
}

LPDISPATCH CHtmlViewEx::GetContainer() const
{
	ASSERT(m_pBrowserApp != NULL);

	LPDISPATCH result;
	m_pBrowserApp->get_Container(&result);
	return result;
}

LPDISPATCH CHtmlViewEx::GetHtmlDocument() const
{
	ASSERT(m_pBrowserApp != NULL);

	LPDISPATCH result;
	m_pBrowserApp->get_Document(&result);
	return result;
}

BOOL CHtmlViewEx::GetTopLevelContainer() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_TopLevelContainer(&result);
	return result;
}

BOOL CHtmlViewEx::GetMenuBar() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_MenuBar(&result);
	return result;
}

BOOL CHtmlViewEx::GetFullScreen() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_FullScreen(&result);
	return result;
}

BOOL CHtmlViewEx::GetStatusBar() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_StatusBar(&result);
	return result;
}

OLECMDF CHtmlViewEx::QueryStatusWB(OLECMDID cmdID) const
{
	ASSERT(m_pBrowserApp != NULL);

	OLECMDF result;
	m_pBrowserApp->QueryStatusWB(cmdID, &result);
	return result;
}

void CHtmlViewEx::ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt,
	VARIANT* pvaIn, VARIANT* pvaOut)
{
	ASSERT(m_pBrowserApp != NULL);

	m_pBrowserApp->ExecWB(cmdID, cmdexecopt, pvaIn, pvaOut);
}

BOOL CHtmlViewEx::GetRegisterAsBrowser() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_RegisterAsBrowser(&result);
	return result;
}

BOOL CHtmlViewEx::GetRegisterAsDropTarget() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_RegisterAsDropTarget(&result);
	return result;
}

BOOL CHtmlViewEx::GetTheaterMode() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_TheaterMode(&result);
	return result;
}

BOOL CHtmlViewEx::GetAddressBar() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_AddressBar(&result);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewEx operations

BOOL CHtmlViewEx::LoadFromResource(LPCTSTR lpszResource)
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);

	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];

	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%s"), lpszModule, lpszResource);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
		bRetVal = FALSE;

	delete [] lpszModule;
	return bRetVal;
}

BOOL CHtmlViewEx::LoadFromResource(UINT nRes)
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);

	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];

	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%d"), lpszModule, nRes);
#if 1
		Navigate(strResourceURL, 0, 0, 0);
#else
		Navigate2(strResourceURL, 0, 0, 0);
#endif
	}
	else
		bRetVal = FALSE;

	delete [] lpszModule;
	return bRetVal;
}

void CHtmlViewEx::Navigate(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */ ,
	LPCTSTR lpszHeaders /* = NULL */, LPVOID lpvPostData /* = NULL */,
	DWORD dwPostDataLen /* = 0 */)
{
	CString strURL(lpszURL);
	BSTR bstrURL = strURL.AllocSysString();

	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
			dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}

	m_pBrowserApp->Navigate(bstrURL,
		COleVariant((long) dwFlags, VT_I4),
		COleVariant(lpszTargetFrameName, VT_BSTR),
		vPostData,
		COleVariant(lpszHeaders, VT_BSTR));
}

void CHtmlViewEx::Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */)
{
	ASSERT(m_pBrowserApp != NULL);

	COleVariant vPIDL(pIDL);
	COleVariant empty;

	m_pBrowserApp->Navigate2(vPIDL,
		COleVariant((long) dwFlags, VT_I4),
		COleVariant(lpszTargetFrameName, VT_BSTR),
		empty, empty);
}

void CHtmlViewEx::Navigate2(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */,
	LPCTSTR lpszHeaders /* = NULL */,
	LPVOID lpvPostData /* = NULL */, DWORD dwPostDataLen /* = 0 */)
{
	ASSERT(m_pBrowserApp != NULL);

	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
			dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}

	COleVariant vURL(lpszURL, VT_BSTR);
	COleVariant vHeaders(lpszHeaders, VT_BSTR);
	COleVariant vTargetFrameName(lpszTargetFrameName, VT_BSTR);
	COleVariant vFlags((long) dwFlags, VT_I4);

	m_pBrowserApp->Navigate2(vURL,
		vFlags, vTargetFrameName, vPostData, vHeaders);
}

void CHtmlViewEx::Navigate2(LPCTSTR lpszURL, DWORD dwFlags,
	CByteArray& baPostData, LPCTSTR lpszTargetFrameName /* = NULL */,
	LPCTSTR lpszHeaders /* = NULL */)
{
	ASSERT(m_pBrowserApp != NULL);

	COleVariant vPostData = baPostData;
	COleVariant vURL(lpszURL, VT_BSTR);
	COleVariant vHeaders(lpszHeaders, VT_BSTR);
	COleVariant vTargetFrameName(lpszTargetFrameName, VT_BSTR);
	COleVariant vFlags((long) dwFlags, VT_I4);

	ASSERT(m_pBrowserApp != NULL);

	m_pBrowserApp->Navigate2(vURL, vFlags, vTargetFrameName,
		vPostData, vHeaders);
}

void CHtmlViewEx::PutProperty(LPCTSTR lpszProperty, const VARIANT& vtValue)
{
	ASSERT(m_pBrowserApp != NULL);

	CString strProp(lpszProperty);
	BSTR bstrProp = strProp.AllocSysString();
	m_pBrowserApp->PutProperty(bstrProp, vtValue);
	::SysFreeString(bstrProp);
}

BOOL CHtmlViewEx::GetProperty(LPCTSTR lpszProperty, CString& strValue)
{
	ASSERT(m_pBrowserApp != NULL);

	CString strProperty(lpszProperty);
	BSTR bstrProperty = strProperty.AllocSysString();

	BOOL bResult = FALSE;
	VARIANT vReturn;
	vReturn.vt = VT_BSTR;
	vReturn.bstrVal = NULL;
	HRESULT hr = m_pBrowserApp->GetProperty(bstrProperty, &vReturn);

	if (SUCCEEDED(hr))
	{
		strValue = CString(vReturn.bstrVal);
		bResult = TRUE;
	}

	::SysFreeString(bstrProperty);
	return bResult;
}

COleVariant CHtmlViewEx::GetProperty(LPCTSTR lpszProperty)
{
	COleVariant result;

	static BYTE parms[] =
		VTS_BSTR;
	m_wndBrowser.InvokeHelper(0x12f, DISPATCH_METHOD,
		VT_VARIANT, (void*)&result, parms, lpszProperty);

	return result;
}

CString CHtmlViewEx::GetFullName() const
{
	ASSERT(m_pBrowserApp != NULL);

	BSTR bstr;
	m_pBrowserApp->get_FullName(&bstr);
	CString retVal(bstr);
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewEx event reflectors

void CHtmlViewEx::NavigateComplete2(LPDISPATCH /* pDisp */, VARIANT* URL)
{
	ASSERT(V_VT(URL) == VT_BSTR);

	USES_CONVERSION;

	CString str = OLE2T(V_BSTR(URL));
	OnNavigateComplete2(str);
}

void CHtmlViewEx::BeforeNavigate2(LPDISPATCH /* pDisp */, VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName,
		VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	ASSERT(V_VT(URL) == VT_BSTR);
	ASSERT(V_VT(TargetFrameName) == VT_BSTR);
	ASSERT(V_VT(PostData) == (VT_VARIANT | VT_BYREF));
	ASSERT(V_VT(Headers) == VT_BSTR);
	ASSERT(Cancel != NULL);

	USES_CONVERSION;

	VARIANT* vtPostedData = V_VARIANTREF(PostData);
	CByteArray array;
	CString strPostData;

	if (V_VT(vtPostedData) & VT_ARRAY)
	{
		// must be a vector of bytes
		ASSERT(vtPostedData->parray->cDims == 1 && vtPostedData->parray->cbElements == 1);

		vtPostedData->vt |= VT_UI1;
		COleSafeArray safe(vtPostedData);

		DWORD dwSize = safe.GetOneDimSize();
		LPVOID pVoid;
		safe.AccessData(&pVoid);

		// Make the CByteArray
		array.SetSize(dwSize);
		LPBYTE lpByte = array.GetData();
		memcpy(lpByte, pVoid, dwSize);

		// Make the String
		LPTSTR lpStr = strPostData.GetBuffer(dwSize);
		memcpy(lpStr, pVoid, dwSize);
		strPostData.ReleaseBuffer(-1);
		
		safe.UnaccessData();
	}
	// make real parameters out of the notification

	CString strTargetFrameName(V_BSTR(TargetFrameName));
	CString strURL = V_BSTR(URL);
	CString strHeaders = V_BSTR(Headers);
	DWORD nFlags = V_I4(Flags);

	// notify the user's class
	CString strAppProtocol = APP_PROTOCOL;

	int len = strAppProtocol.GetLength();
	if ( strURL.Find(strAppProtocol) == 0 ) 
		{
			CString AppCmd = 	strURL.Right(strURL.GetLength() - len);
			OnAppCmd(AppCmd, strPostData);
			*Cancel = TRUE;
		}

	OnBeforeNavigate2(strURL, nFlags, strTargetFrameName,
		array, strHeaders, Cancel);
	OnBeforeNavigate2(strURL, nFlags, strTargetFrameName, 
			strPostData, strHeaders, Cancel);
}

void CHtmlViewEx::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	UNUSED_ALWAYS(pDisp);
	ASSERT(V_VT(URL) == VT_BSTR);

	if(m_nFontSize > -1)
	{
		SetFontSize(m_nFontSize);
	}

	CString str(V_BSTR(URL));
	OnDocumentComplete(str);
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewEx Events

void CHtmlViewEx::OnAppCmd(LPCTSTR lpszAppCmd, LPCTSTR PostData)
{
	// User will override to process application commands
	UNUSED_ALWAYS(lpszAppCmd);
	UNUSED_ALWAYS(PostData);
}

void CHtmlViewEx::OnProgressChange(long lProgress, long lProgressMax)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lProgress);
	UNUSED_ALWAYS(lProgressMax);
}

void CHtmlViewEx::OnCommandStateChange(long lCommand, BOOL bEnable)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lCommand);
	UNUSED_ALWAYS(bEnable);
}

void CHtmlViewEx::OnDownloadBegin()
{
	// user will override to handle this notification
}

void CHtmlViewEx::OnDownloadComplete()
{
	// user will override to handle this notification
}

void CHtmlViewEx::OnTitleChange(LPCTSTR lpszText)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszText);
}

void CHtmlViewEx::OnPropertyChange(LPCTSTR lpszProperty)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszProperty);
}

void CHtmlViewEx::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* bCancel)
{
	// default to continuing
	bCancel = FALSE;

	// user will override to handle this notification
	UNUSED_ALWAYS(ppDisp);
}

void CHtmlViewEx::OnDocumentComplete(LPCTSTR lpszURL)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
}

void CHtmlViewEx::OnQuit()
{
	// user will override to handle this notification
}

void CHtmlViewEx::OnVisible(BOOL bVisible)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bVisible);
}

void CHtmlViewEx::OnToolBar(BOOL bToolBar)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bToolBar);
}

void CHtmlViewEx::OnMenuBar(BOOL bMenuBar)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bMenuBar);
}

void CHtmlViewEx::OnStatusBar(BOOL bStatusBar)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bStatusBar);
}

void CHtmlViewEx::OnFullScreen(BOOL bFullScreen)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bFullScreen);
}

void CHtmlViewEx::OnTheaterMode(BOOL bTheaterMode)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bTheaterMode);
}

void CHtmlViewEx::OnAddressBar(BOOL bAddressBar)
{
	// I don't know why, but this event sink was left out
	// of the original Microsoft Source! I can only guess
	// about this implementation, there are *no* docs in
	// the Platform SDK about it.

	// user will override to handle this notification
	UNUSED_ALWAYS(bAddressBar);
}

void CHtmlViewEx::OnWindowSetResizable(BOOL bResizable)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bResizable);
}

void CHtmlViewEx::OnWindowClosing(BOOL bIsChildWindow, BOOL* pbCancel)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bIsChildWindow);
	UNUSED_ALWAYS(pbCancel);
}

void CHtmlViewEx::OnWindowSetLeft(long Left) 
{
	// user will override to handle this notification
	UNUSED_ALWAYS(Left);
}

void CHtmlViewEx::OnWindowSetTop(long Top)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(Top);
}

void CHtmlViewEx::OnWindowSetWidth(long Width)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(Width);
}

void CHtmlViewEx::OnWindowSetHeight(long Height)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(Height);
}

void CHtmlViewEx::OnClientToHostWindow(long* CX, long* CY)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(CX);
	UNUSED_ALWAYS(CY);
}

void CHtmlViewEx::OnSetSecureLockIcon(long SecureLockIcon)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(SecureLockIcon);
}

void CHtmlViewEx::OnFileDownload(BOOL* pbCancel)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(pbCancel);
}

void CHtmlViewEx::OnNavigateComplete2(LPCTSTR lpszURL)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
}

void CHtmlViewEx::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
	LPCTSTR lpszTargetFrameName, CByteArray& baPostData,
	LPCTSTR lpszHeaders, BOOL* bCancel)
{
	// default to continuing
	bCancel = FALSE;

	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
	UNUSED_ALWAYS(nFlags);
	UNUSED_ALWAYS(lpszTargetFrameName);
	UNUSED_ALWAYS(baPostData);
	UNUSED_ALWAYS(lpszHeaders);
}

void CHtmlViewEx::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,	
	LPCTSTR lpszTargetFrameName, LPCTSTR strPostedData,
	LPCTSTR lpszHeaders, BOOL* bCancel)
{
	// default to continuing
	bCancel = FALSE;
	
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
	UNUSED_ALWAYS(nFlags);
	UNUSED_ALWAYS(lpszTargetFrameName);
	UNUSED_ALWAYS(strPostedData);
	UNUSED_ALWAYS(lpszHeaders);
}

void CHtmlViewEx::OnStatusTextChange(LPCTSTR pszText)
{
	if(!m_bNoStatusText)
	{
		// try to set the status bar text via the frame
		
		CFrameWnd* pFrame = GetParentFrame();
		if (pFrame != NULL)
			pFrame->SetMessageText(pszText);
	}
}


// Advanced context menu handler
void CHtmlViewEx::OnAdvancedContextMenu(DWORD dwID, CPoint ptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit)
{
#ifdef _DEBUG
	TRACE(_T("No OnAdvancedContextMenu override! Menu ID = "));

	switch (dwID)
	{
		case CONTEXT_MENU_DEFAULT:		// 0
			TRACE(_T("CONTEXT_MENU_DEFAULT"));
			break;
		case CONTEXT_MENU_IMAGE:			// 1
			TRACE(_T("CONTEXT_MENU_IMAGE"));
			break;
		case CONTEXT_MENU_CONTROL:		// 2
			TRACE(_T("CONTEXT_MENU_CONTROL"));
			break;
		case CONTEXT_MENU_TABLE:			// 3
			TRACE(_T("CONTEXT_MENU_TABLE"));
			break;
		case CONTEXT_MENU_TEXTSELECT:			// 4
			TRACE(_T("CONTEXT_MENU_TEXTSELECT"));
			break;
		case CONTEXT_MENU_ANCHOR:		// 5
			TRACE(_T("CONTEXT_MENU_ANCHOR"));
			break;
		case CONTEXT_MENU_UNKNOWN:			// 6
			TRACE(_T("CONTEXT_MENU_UNKNOWN"));
			break;
		case CONTEXT_MENU_VSCROLL:	// 10
			TRACE(_T("CONTEXT_MENU_VSCROLL"));
			break;
		case CONTEXT_MENU_HSCROLL:	// 11
			TRACE(_T("CONTEXT_MENU_HSCROLL"));
			break;
		default: // UNKNOWN
			TRACE1("UNKNOWN (code = %d)",dwID);
			break;
	}
	TRACE(_T("\n"));
#endif // _DEBUG

	// user will override to handle this notification
	UNUSED_ALWAYS(dwID); 
	UNUSED_ALWAYS(ptPosition); 
	UNUSED_ALWAYS(pCommandTarget); 
	UNUSED_ALWAYS(pDispatchObjectHit);
}

void CHtmlViewEx::OnShowHelp(HWND hwnd, LPCTSTR pszHelpFile,UINT uCommand,DWORD dwData,POINT ptMouse,	IDispatch __RPC_FAR *pDispatchObjectHit)
{
#ifdef _DEBUG
	TRACE(_T("No OnShowHelp override!\n"));
#endif // _DEBUG

	// user will override to handle this notification
	UNUSED_ALWAYS(hwnd);
	UNUSED_ALWAYS(pszHelpFile);
	UNUSED_ALWAYS(uCommand);
	UNUSED_ALWAYS(dwData);
	UNUSED_ALWAYS(ptMouse);
	UNUSED_ALWAYS(pDispatchObjectHit);
}

void CHtmlViewEx::OnShowMessage(HWND hwnd, LPCTSTR lpstrText,	LPCTSTR lpstrCaption,	DWORD dwType,	LPCTSTR lpstrHelpFile,	DWORD dwHelpContext, LRESULT __RPC_FAR *plResult)
{
#ifdef _DEBUG
	TRACE(_T("No OnShowMessage override!\n"));
#endif // _DEBUG
	// The default function will more or less duplicate the 
	// normal IE/MSHTML messagebox

	// You can change this to display whatever you want -
	// or nothing at all!

	UNUSED_ALWAYS(lpstrHelpFile);
	UNUSED_ALWAYS(dwHelpContext);

	int retval = ::MessageBox(hwnd,lpstrText,lpstrCaption,dwType);
	*plResult = (LRESULT)retval;
}

// IDocHostUIHandler DblClk interface
DWORD CHtmlViewEx::GetUI_DblClk() const 
{
	return m_pHtmlViewExOccManager->m_pHtmlViewExSite->GetHostInfo_DblClk();
}

void CHtmlViewEx::SetUI_DblClk(DWORD dwSet) 
{
	m_pHtmlViewExOccManager->m_pHtmlViewExSite->SetHostInfo_DblClk(dwSet);
}

// DocHostUIHandler Flags interface
DWORD CHtmlViewEx::GetUI_Flags() const 
{
	return m_pHtmlViewExOccManager->m_pHtmlViewExSite->GetHostInfo_Flags();
}

void CHtmlViewEx::SetUI_Flags(DWORD dwSet) 
{
	m_pHtmlViewExOccManager->m_pHtmlViewExSite->SetHostInfo_Flags(dwSet);
}

void	CHtmlViewEx::SetHostInfo_HostCss(LPCWSTR szHostCss )
{
	m_pHtmlViewExOccManager->m_pHtmlViewExSite->SetHostInfo_HostCss(szHostCss);
}

BOOL CHtmlViewEx::GetUIFlag_Dialog() const 
{
	return GetUI_Flags() & DOCHOSTUIFLAG_DIALOG;
}

BOOL CHtmlViewEx::GetUIFlag_DisableHelpMenu() const 
{
	return GetUI_Flags() & DOCHOSTUIFLAG_DISABLE_HELP_MENU;
}

BOOL CHtmlViewEx::GetUIFlag_No3dBorder() const 
{
	return GetUI_Flags() & DOCHOSTUIFLAG_NO3DBORDER;
}

BOOL CHtmlViewEx::GetUIFlag_NoScrollbar() const 
{
	return GetUI_Flags() & DOCHOSTUIFLAG_SCROLL_NO;
}

BOOL CHtmlViewEx::GetUIFlag_DisableScriptInactive() const 
{
	return GetUI_Flags() & DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE;
}

BOOL CHtmlViewEx::GetUIFlag_OpenNewWin() const 
{
	return GetUI_Flags() & DOCHOSTUIFLAG_OPENNEWWIN;
}

BOOL CHtmlViewEx::GetUIFlag_DisableOffscreen() const 
{
	return GetUI_Flags() & DOCHOSTUIFLAG_DISABLE_OFFSCREEN;
}

BOOL CHtmlViewEx::GetUIFlag_FlatScrollbar() const 
{
	return GetUI_Flags() & DOCHOSTUIFLAG_FLAT_SCROLLBAR;
}

BOOL CHtmlViewEx::GetUIFlag_DivBlockDefault() const 
{
	return GetUI_Flags() & DOCHOSTUIFLAG_DIV_BLOCKDEFAULT;
}

BOOL CHtmlViewEx::GetUIFlag_ActivateClientHitOnly() const 
{
	return GetUI_Flags() & DOCHOSTUIFLAG_ACTIVATE_CLIENTHIT_ONLY;
}

void CHtmlViewEx::SetUIFlag_Dialog(BOOL bSet) 
{
	DWORD dwFlags = GetUI_Flags();
	bSet ? dwFlags |= DOCHOSTUIFLAG_DIALOG : dwFlags &= ~DOCHOSTUIFLAG_DIALOG;
	SetUI_Flags(dwFlags);
}

void CHtmlViewEx::SetUIFlag_DisableHelpMenu(BOOL bSet) 
{
	DWORD dwFlags = GetUI_Flags();
	bSet ? dwFlags |= DOCHOSTUIFLAG_DISABLE_HELP_MENU : dwFlags &= ~DOCHOSTUIFLAG_DISABLE_HELP_MENU;
	SetUI_Flags(dwFlags);
}

void CHtmlViewEx::SetUIFlag_No3dBorder(BOOL bSet) 
{
	DWORD dwFlags = GetUI_Flags();
	bSet ? dwFlags |= DOCHOSTUIFLAG_NO3DBORDER : dwFlags &= ~DOCHOSTUIFLAG_NO3DBORDER;
	SetUI_Flags(dwFlags);
}

void CHtmlViewEx::SetUIFlag_NoScrollbar(BOOL bSet) 
{
	DWORD dwFlags = GetUI_Flags();
	bSet ? dwFlags |= DOCHOSTUIFLAG_SCROLL_NO : dwFlags &= ~DOCHOSTUIFLAG_SCROLL_NO;
	SetUI_Flags(dwFlags);
}

void CHtmlViewEx::SetUIFlag_DisableScriptInactive(BOOL bSet) 
{
	DWORD dwFlags = GetUI_Flags();
	bSet ? dwFlags |= DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE : dwFlags &= ~DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE;
	SetUI_Flags(dwFlags);
}

void CHtmlViewEx::SetUIFlag_OpenNewWin(BOOL bSet) 
{
	DWORD dwFlags = GetUI_Flags();
	bSet ? dwFlags |= DOCHOSTUIFLAG_OPENNEWWIN : dwFlags &= ~DOCHOSTUIFLAG_OPENNEWWIN;
	SetUI_Flags(dwFlags);
}

void CHtmlViewEx::SetUIFlag_DisableOffscreen(BOOL bSet) 
{
	DWORD dwFlags = GetUI_Flags();
	bSet ? dwFlags |= DOCHOSTUIFLAG_DISABLE_OFFSCREEN : dwFlags &= ~DOCHOSTUIFLAG_DISABLE_OFFSCREEN;
	SetUI_Flags(dwFlags);
}

void CHtmlViewEx::SetUIFlag_FlatScrollbar(BOOL bSet) 
{
	DWORD dwFlags = GetUI_Flags();
	bSet ? dwFlags |= DOCHOSTUIFLAG_FLAT_SCROLLBAR : dwFlags &= ~DOCHOSTUIFLAG_FLAT_SCROLLBAR;
	SetUI_Flags(dwFlags);
}

void CHtmlViewEx::SetUIFlag_DivBlockDefault(BOOL bSet) 
{
	DWORD dwFlags = GetUI_Flags();
	bSet ? dwFlags |= DOCHOSTUIFLAG_DIV_BLOCKDEFAULT : dwFlags &= ~DOCHOSTUIFLAG_DIV_BLOCKDEFAULT;
	SetUI_Flags(dwFlags);
}

void CHtmlViewEx::SetUIFlag_ActivateClientHitOnly(BOOL bSet) 
{
	DWORD dwFlags = GetUI_Flags();
	bSet ? dwFlags |= DOCHOSTUIFLAG_ACTIVATE_CLIENTHIT_ONLY : dwFlags &= ~DOCHOSTUIFLAG_ACTIVATE_CLIENTHIT_ONLY;
	SetUI_Flags(dwFlags);
}

void CHtmlViewEx::UseCustomHelp(BOOL bSet)
{
	m_pHtmlViewExOccManager->m_pHtmlViewExSite->UseCustomHelp(bSet);
}

void CHtmlViewEx::UseCustomMessage(BOOL bSet)
{
	m_pHtmlViewExOccManager->m_pHtmlViewExSite->UseCustomMessage(bSet);
}

void CHtmlViewEx::UseCustomContextMenu(BOOL bSet)
{
	m_pHtmlViewExOccManager->m_pHtmlViewExSite->UseCustomContextMenu(bSet);
}

void CHtmlViewEx::UseAdvancedContextMenu(BOOL bSet)
{
	m_pHtmlViewExOccManager->m_pHtmlViewExSite->UseAdvancedContextMenu(bSet);
}

CString CHtmlViewEx::ProcessCharCodes(LPCTSTR lpszInput)
{
	CString input(lpszInput);
	CString output = _T("");
	CString iChar;
	CString iTemp;
	TCHAR oChar;

	for (int i = 0;i < input.GetLength();i++) 
	{
		iTemp = _T("");
		iChar = input[i];

		if (iChar == _T("%"))
		{	// Start of a character token
			iTemp += iChar;
			i++;
			iChar = input[i];
			iTemp += iChar;
			i++;
			iChar = input[i];
			iTemp += iChar;
			oChar = CharTokenToChar(iTemp);
			output += oChar;
		}
		else
		{ // Not a character token
			output += iChar;
		}
	}
	return output;
}

char CHtmlViewEx::CharTokenToChar(const CString &val)
{
	long ret;
	// TRACE(_T("Input %s\n",val);
	CString input(val);
	input.Replace(_T("%"),_T("0x"));
	ret = _tcstol((LPCTSTR)input,NULL,0);
	// TRACE("Output %d\n",ret);
	return (char)ret;
}

CString CHtmlViewEx::GetValueFromPostData(LPCTSTR strPostData, LPCTSTR strValName)
{
	CString strPost(strPostData);
	CString retvalue = _T("");
	int found = strPost.Find(strValName);

	if(found == -1)
		// Field name not found
		retvalue = _T("");
	else
	{	// Field name found
		int eq = strPost.Find(_T("="),found);
		if (eq == -1)
			retvalue = _T("");
		else
		{
			for(int i = eq + 1; i < strPost.GetLength(); i++)
			{
				if (strPost[i] == '&')
					break;
				else 
					retvalue += strPost[i];	
			}
		}
	}

	// Fix the spaces first, then run though
	// the charcode processor befor returning
	retvalue.Replace(_T("+"),_T(" "));
	return ProcessCharCodes(retvalue);
}

void CHtmlViewEx::SetFontSize(int fontSize)
{
#ifdef _DEBUG
	TRACE1("CHtmlViewEx::SetFontSize(%d)\n", fontSize);
#endif // _DEBUG

	// The documentation says that font sizes 0 through 5 are
	// valid but you can only set 0 through 4
	if (fontSize < 0)
	{
		#ifdef _DEBUG
		TRACE1("Font Size (%d) too small! Setting to (0).\n", fontSize);
		#endif // _DEBUG
		fontSize = 0;
	}
	if (fontSize > 4)
	{
		#ifdef _DEBUG
		TRACE1("Font Size (%d) too large! Setting to (4).\n", fontSize);
		#endif // _DEBUG
		fontSize = 4;
	}

	LPDISPATCH pDisp = NULL;
	
	if (!m_wndBrowser.m_hWnd)
	{
		#ifdef _DEBUG
		TRACE(_T("Web Browser Control not yet created.\n"));
		#endif // _DEBUG
		return;
	}
	
	VARIANT vaZoomFactor;					// input arguments
	VariantInit(&vaZoomFactor);
	V_VT(&vaZoomFactor) = VT_I4;
	V_I4(&vaZoomFactor) = fontSize;
	
	if(!SUCCEEDED(
		m_pBrowserApp->ExecWB(OLECMDID_ZOOM, 
		OLECMDEXECOPT_DONTPROMPTUSER, &vaZoomFactor, NULL)
		))
	{
		#ifdef _DEBUG
		TRACE1("Unable to set the font size to (%d).\n", fontSize);
		#endif // _DEBUG
	}
	else
	{
		#ifdef _DEBUG
		TRACE1("Font size set to (%d).\n", fontSize);
		#endif // _DEBUG
	}

	VariantClear(&vaZoomFactor);

	// If this fails, the variable will be set and it will
	// be set when the document is completely loaded
	m_nFontSize = fontSize;

	RELEASE(pDisp); // release document's dispatch interface
}

int CHtmlViewEx::GetFontSize() const
{
	#ifdef _DEBUG
	TRACE(_T("CHtmlViewEx::GetFontSize()\n"));
	#endif // _DEBUG

	LPDISPATCH pDisp = NULL;
	int fontSize;
	
	if (!m_wndBrowser.m_hWnd)
	{
		#ifdef _DEBUG
		TRACE(_T("Web Browser Control not yet created.\n"));
		#endif // _DEBUG
		return -1;
	}
	
	VARIANT vaZoomFactor;					
	
	if(!SUCCEEDED(
		m_pBrowserApp->ExecWB(OLECMDID_ZOOM,	OLECMDEXECOPT_DONTPROMPTUSER,	
		NULL, &vaZoomFactor)
		))
	{
		#ifdef _DEBUG
		TRACE(_T("Unable to get the font size.\n"));
		#endif // _DEBUG
	}

	ASSERT(V_VT(&vaZoomFactor) == VT_I4);
	fontSize = V_I4(&vaZoomFactor);
	#ifdef _DEBUG
	TRACE1("Font size is %d.\n", fontSize);
	#endif // _DEBUG

	VariantClear(&vaZoomFactor);

	RELEASE(pDisp); // release document's dispatch interface

	return fontSize;
}
/////////////////////////////////////////////////////////
// OnAmbientProperty
//
// In the interest of relative completeness, I have 
// included an implementation of OnAmbientProperty
// to allow you to tweak some more options of the 
// control.
//
BOOL CHtmlViewEx::OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar) 
{
	#ifdef _TRACE_AMB_PROPRERIES
	TRACE1(_T("OnAmbientProperty : '%s'\n"),StringDISPID(dispid));
	#endif // _TRACE_AMB_PROPRERIES

	// Check the Dispatch ID to see if we handle it
	switch(dispid)
	{
		case DISPID_AMBIENT_OFFLINEIFNOTCONNECTED:
			{
				#ifdef _TRACE_AMB_PROPRERIES
				TRACE(_T("DISPID_AMBIENT_OFFLINEIFNOTCONNECTED Handled\n"));
				#endif // _TRACE(_T_AMB_PROPRERIES
				
				pvar->vt = VT_BOOL;
				pvar->boolVal = (VARIANT_BOOL)m_bOfflineIfNotConnected;
				
				return TRUE;
			}
		case DISPID_AMBIENT_SILENT:
			{
				#ifdef _TRACE_AMB_PROPRERIES
				TRACE(_T("DISPID_AMBIENT_SILENT Handled\n"));
				#endif // _TRACE(_T_AMB_PROPRERIES
				
				pvar->vt = VT_BOOL;
				pvar->boolVal = (VARIANT_BOOL)m_bSilentMode;
				
				return TRUE;
			}
		case DISPID_AMBIENT_USERMODE:
			{
				// Let OLE know we are in run mode 
				// Not needed but we have it here anyway
				#ifdef _TRACE_AMB_PROPRERIES
				TRACE(_T("DISPID_AMBIENT_USERMODE Handled\n"));
				#endif // _TRACE(_T_AMB_PROPRERIES
				
				pvar->vt = VT_BOOL;
				pvar->boolVal = TRUE;
				
				return TRUE;
			}
		case DISPID_AMBIENT_DLCONTROL:
			{
				// Change download properties - 
				// no java, no scripts... whatever you want
				#ifdef _TRACE_AMB_PROPRERIES
				TRACE(_T("DISPID_AMBIENT_DLCONTROL Handled\n"));
				#endif // _TRACE(_T_AMB_PROPRERIES
				pvar->vt = VT_I4;
				pvar->lVal = DLCTL_DLIMAGES;
				//| DLCTL_NO_SCRIPTS
				//| DLCTL_NO_JAVA
				//| DLCTL_NO_RUNACTIVEXCTLS 
				//| DLCTL_NO_DLACTIVEXCTLS;
				return TRUE;
			}
		case DISPID_AMBIENT_USERAGENT:
			{
				// Change user agent for this web browser host 
				// during hyperlinks
				#ifdef _TRACE_AMB_PROPRERIES
				TRACE(_T("DISPID_AMBIENT_USERAGENT Handled\n"));
				#endif // _TRACE_AMB_PROPRERIES

				pvar->vt = VT_BSTR;
				pvar->bstrVal = m_strUserAgent.AllocSysString();
				
				return TRUE;
			}
		default:
			#ifdef _TRACE_AMB_PROPRERIES
			TRACE1(_T("OnAmbientProperty Unhandled : '%s'\n"), StringDISPID(dispid));
			#endif // _TRACE(_T_AMB_PROPRERIES
			break;
	}

	return CFormView::OnAmbientProperty(pSite, dispid, pvar);
}


IDispatch* CHtmlViewEx::GetInputElementByName(LPCTSTR lpszElementName)
{
	LPDISPATCH pDocDisp;
	//IDispatch* pOutDisp = NULL;

	IHTMLDocument2* pHTMLDocument = NULL;
	IHTMLElementCollection* pHTMLElements = NULL;
	long cHTMLElements = 0;
	BOOL bReady = TRUE;  // always the optimist :)

	if(SUCCEEDED(m_pBrowserApp->get_Document(&pDocDisp)))
	{	
		if(SUCCEEDED(pDocDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDocument)))
		{	
			if(SUCCEEDED(pHTMLDocument->get_all(&pHTMLElements)))
			{	
				if(!SUCCEEDED(pHTMLElements->get_length(&cHTMLElements)))
				{
					cHTMLElements = 0;
					bReady = FALSE;
				}
			}	else bReady = FALSE;
		}	else bReady = FALSE;
	} else bReady = FALSE;

	if (!bReady)
	{
		// The code above bombed, how odd!
		// Abort the routine
		RELEASE(pHTMLElements);
		RELEASE(pHTMLDocument);
		RELEASE(pDocDisp);
		return NULL;
	}

	// The Interface is ready, so use it already
	////////////////////////////////////////////////////////

#ifdef _DEBUG
//	TRACE(_T("GetInputElementByName('%s') found %d total elements\n",
//	lpszElementName,cHTMLElements);
#endif // _DEBUG

	// This is a mess I know, but it works!

	for (int i = 0; i < cHTMLElements; i++)
	{
		// Local Variables for iteration
		VARIANT varIndex;				// For element iteration
		varIndex.vt = VT_UINT;
		varIndex.lVal = i;
		VARIANT var2;						// Not really used, but we need it
		VariantInit( &var2 );
		IDispatch* pDisp;				// Temporary IDispatch for iteration
		
		if (SUCCEEDED(pHTMLElements->item(varIndex, var2, &pDisp )))
		{
			BSTR bstr;
			IHTMLInputElement* pElem;

			if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLInputElement, (void **)&pElem )))
			{
				if (SUCCEEDED(pElem->get_name(&bstr)))
				{	// Is it the *RIGHT* element?
					CString nametemp = bstr;
					if (nametemp == lpszElementName)
					{	// Yes it is!
						#ifdef _DEBUG
						//TRACE(_T("GetInputElementByName('%s') found element '%s' at Element %d\n"
						//,lpszElementName,nametemp,i);
						#endif // _DEBUG

						RELEASE(pElem);
						RELEASE(pHTMLElements);
						RELEASE(pHTMLDocument);
						RELEASE(pDocDisp);

						return pDisp;
					}
				}
				RELEASE(pElem);
			}
			RELEASE(pDisp);
		}
	}

	RELEASE(pHTMLElements);
	RELEASE(pHTMLDocument);
	RELEASE(pDocDisp);

	return NULL;
}

IDispatch* CHtmlViewEx::GetSelectElementByName(LPCTSTR lpszElementName)
{
	LPDISPATCH pDocDisp;
	//IDispatch* pOutDisp = NULL;

	IHTMLDocument2* pHTMLDocument = NULL;
	IHTMLElementCollection* pHTMLElements = NULL;
	long cHTMLElements = 0;
	BOOL bReady = TRUE;  // always the optimist :)

	if(SUCCEEDED(m_pBrowserApp->get_Document(&pDocDisp)))
	{	
		if(SUCCEEDED(pDocDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDocument)))
		{	
			if(SUCCEEDED(pHTMLDocument->get_all(&pHTMLElements)))
			{	
				if(!SUCCEEDED(pHTMLElements->get_length(&cHTMLElements)))
				{
					cHTMLElements = 0;
					bReady = FALSE;
				}
			}	else bReady = FALSE;
		}	else bReady = FALSE;
	} else bReady = FALSE;

	if (!bReady)
	{
		// The code above bombed, how odd!
		// Abort the routine
		RELEASE(pHTMLElements);
		RELEASE(pHTMLDocument);
		RELEASE(pDocDisp);
		return NULL;
	}

	// The Interface is ready, so use it already
	////////////////////////////////////////////////////////

	#ifdef _DEBUG
//	TRACE(_T("GetInputElementByName('%s') found %d total elements\n"
//		,lpszElementName,cHTMLElements);
	#endif // _DEBUG

	// This is a mess I know, but it works!

	for (int i = 0; i < cHTMLElements; i++)
	{
		// Local Variables for iteration
		VARIANT varIndex;				// For element iteration
		varIndex.vt = VT_UINT;
		varIndex.lVal = i;
		VARIANT var2;						// Not really used, but we need it
		VariantInit( &var2 );
		IDispatch* pDisp;				// Temporary IDispatch for iteration
		
		if (SUCCEEDED(pHTMLElements->item(varIndex, var2, &pDisp )))
		{
			BSTR bstr;
			IHTMLSelectElement* pElem;

			if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLSelectElement, (void **)&pElem )))
			{
				if (SUCCEEDED(pElem->get_name(&bstr)))
				{	// Is it the *RIGHT* element?
					CString nametemp = bstr;
					if (nametemp == lpszElementName)
					{	// Yes it is!
						#ifdef _DEBUG
//						TRACE(_T("GetSelectElementByName('%s') found element '%s' at Element %d\n"
//						,lpszElementName,nametemp,i);
						#endif // _DEBUG

						RELEASE(pElem);
						RELEASE(pHTMLElements);
						RELEASE(pHTMLDocument);
						RELEASE(pDocDisp);

						return pDisp;
					}
				}
				RELEASE(pElem);
			}
			RELEASE(pDisp);
		}
	}

	RELEASE(pHTMLElements);
	RELEASE(pHTMLDocument);
	RELEASE(pDocDisp);

	return NULL;
}

IDispatch* CHtmlViewEx::GetTextAreaElementByName(LPCTSTR lpszElementName)
{
	LPDISPATCH pDocDisp;
	//IDispatch* pOutDisp = NULL;

	IHTMLDocument2* pHTMLDocument = NULL;
	IHTMLElementCollection* pHTMLElements = NULL;
	long cHTMLElements = 0;
	BOOL bReady = TRUE;  // always the optimist :)

	if(SUCCEEDED(m_pBrowserApp->get_Document(&pDocDisp)))
	{	
		if(SUCCEEDED(pDocDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDocument)))
		{	
			if(SUCCEEDED(pHTMLDocument->get_all(&pHTMLElements)))
			{	
				if(!SUCCEEDED(pHTMLElements->get_length(&cHTMLElements)))
				{
					cHTMLElements = 0;
					bReady = FALSE;
				}
			}	else bReady = FALSE;
		}	else bReady = FALSE;
	} else bReady = FALSE;

	if (!bReady)
	{
		// The code above bombed, how odd!
		// Abort the routine
		RELEASE(pHTMLElements);
		RELEASE(pHTMLDocument);
		RELEASE(pDocDisp);
		return NULL;
	}

	// The Interface is ready, so use it already
	////////////////////////////////////////////////////////

	#ifdef _DEBUG
//	TRACE(_T("GetTextAreaElementByName('%s') found %d total elements\n"
//		,lpszElementName,cHTMLElements);
	#endif // _DEBUG

	// This is a mess I know, but it works!

	for (int i = 0; i < cHTMLElements; i++)
	{
		// Local Variables for iteration
		VARIANT varIndex;				// For element iteration
		varIndex.vt = VT_UINT;
		varIndex.lVal = i;
		VARIANT var2;						// Not really used, but we need it
		VariantInit( &var2 );
		IDispatch* pDisp;				// Temporary IDispatch for iteration
		
		if (SUCCEEDED(pHTMLElements->item(varIndex, var2, &pDisp )))
		{
			BSTR bstr;
			IHTMLTextAreaElement* pElem;

			if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLTextAreaElement, (void **)&pElem )))
			{
				if (SUCCEEDED(pElem->get_name(&bstr)))
				{	// Is it the *RIGHT* element?
					CString nametemp = bstr;
					if (nametemp == lpszElementName)
					{	// Yes it is!
						#ifdef _DEBUG
//						TRACE(_T("GetTextAreaElementByName('%s') found element '%s' at Element %d\n"
//						,lpszElementName,nametemp,i);
						#endif // _DEBUG

						RELEASE(pElem);
						RELEASE(pHTMLElements);
						RELEASE(pHTMLDocument);
						RELEASE(pDocDisp);

						return pDisp;
					}
				}
				RELEASE(pElem);
			}
			RELEASE(pDisp);
		}
	}

	RELEASE(pHTMLElements);
	RELEASE(pHTMLDocument);
	RELEASE(pDocDisp);

	return NULL;
}

BOOL CHtmlViewEx::SetInputTextValue(LPCTSTR lpszFieldName, LPCTSTR lpszValue)
{
	BOOL bSuccess = FALSE;
	IDispatch* pDisp;
	pDisp = GetInputElementByName(lpszFieldName);

	if(!pDisp)
		return FALSE;

	IHTMLInputElement* pInputElem;

	if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLInputElement, (void **)&pInputElem )))
	{	
		CString putText(lpszValue);
		if (SUCCEEDED(pInputElem->put_value(putText.AllocSysString())))
		{
			bSuccess = TRUE;
			// Cool, we're done, wind our way out
		}
	}

	RELEASE(pInputElem);
	RELEASE(pDisp);

	return bSuccess;
}

CString CHtmlViewEx::GetInputTextValue(LPCTSTR lpszFieldName)
{
	CString strReturn = _T("");

	BOOL bSuccess = FALSE;
	IDispatch* pDisp;
	pDisp = GetInputElementByName(lpszFieldName);

	if(!pDisp)
		return strReturn;

	IHTMLInputElement* pInputElem;

	if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLInputElement, (void **)&pInputElem )))
	{	
		BSTR getValue;
		if (SUCCEEDED(pInputElem->get_value(&getValue)))
		{
			strReturn = getValue;
			bSuccess = TRUE;
		}
	}

	RELEASE(pInputElem);
	RELEASE(pDisp);

	return strReturn;
}

//////////////////////////////////////////////////////
// Delete the current options list
void CHtmlViewEx::RemoveAllOptionElements_(IHTMLSelectElement* pSelectElem)
{
	long elemSize;
	if (SUCCEEDED(pSelectElem->get_length(&elemSize)))
	{ 
		for(int qq = 0; qq < elemSize; qq++) 
			pSelectElem->remove(0); 
	}
}

BOOL CHtmlViewEx::RemoveAllOptionElements(LPCTSTR lpszFieldName)
{
	BOOL bSuccess = FALSE;
	IDispatch* pDisp;
	pDisp = GetSelectElementByName(lpszFieldName);

	if(!pDisp)
		return FALSE;

	IHTMLSelectElement* pSelectElem;

	if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLSelectElement, (void **)&pSelectElem )))
	{	
		//////////////////////////////////////////////////////
		// Delete the current list
		RemoveAllOptionElements_(pSelectElem);
		bSuccess = TRUE;
	}

	RELEASE(pSelectElem);
	RELEASE(pDisp);

	return bSuccess;
}

IHTMLOptionElementFactory* CHtmlViewEx::GetOptionElementFactory()
{
	LPDISPATCH pDocDisp;
	IHTMLDocument2* pHTMLDocument = NULL;
	IHTMLWindow2* pHTMLWindow = NULL;
	IHTMLOptionElementFactory* pOptionFactory = NULL;

	if(SUCCEEDED(m_pBrowserApp->get_Document(&pDocDisp)))
	{	
		if(SUCCEEDED(pDocDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDocument)))
		{	
			// Get our option factory
			if(SUCCEEDED(pHTMLDocument->get_parentWindow(&pHTMLWindow)))
			{
				if(SUCCEEDED(pHTMLWindow->get_Option(&pOptionFactory)))
				{
					RELEASE(pHTMLWindow);
					RELEASE(pHTMLDocument);
					RELEASE(pDocDisp);

					return pOptionFactory;
				}
			}
		}
	}

	RELEASE(pHTMLWindow);
	RELEASE(pHTMLDocument);
	RELEASE(pDocDisp);

	return NULL;
}

BOOL CHtmlViewEx::AddOptionElement(
	LPCTSTR lpszFieldName, 
	LPCTSTR lpszOptionText, 
	LPCTSTR lpszOptionValue,
	BOOL bSelected,
	BOOL bDefault,
	long nBefore	)
{
	BOOL bSuccess = FALSE;

	IDispatch* pDisp;
	pDisp = GetSelectElementByName(lpszFieldName);
	IHTMLOptionElementFactory* pOptionFactory = GetOptionElementFactory();

	if(!pDisp)
		return FALSE;

	IHTMLSelectElement* pSelectElem;

	if (SUCCEEDED(pDisp->QueryInterface(IID_IHTMLSelectElement, (void **)&pSelectElem )))
	{	
		IHTMLOptionElement* pOptionElement;

		pOptionFactory->create(
			COleVariant(lpszOptionText),
			COleVariant(lpszOptionValue),
			COleVariant((VARIANT_BOOL)bDefault),
			COleVariant((VARIANT_BOOL)bSelected), 
			&pOptionElement);
		pSelectElem->add((IHTMLElement*)pOptionElement,COleVariant(nBefore));

		RELEASE(pOptionElement);
		RELEASE(pOptionFactory);
		bSuccess = TRUE;
	}

	RELEASE(pSelectElem);
	RELEASE(pDisp);

	return bSuccess;
}

BOOL CHtmlViewEx::ClearSelection(LPCTSTR lpszFieldName)
{
	// TODO : Finish This
	UNUSED_ALWAYS(lpszFieldName); 

	return FALSE;
}

BOOL CHtmlViewEx::SetOptionSelected(
	LPCTSTR lpszFieldName,
	LPCTSTR lpszOptionText)
{
	// TODO : Finish This
	UNUSED_ALWAYS(lpszFieldName); 
	UNUSED_ALWAYS(lpszOptionText);

	return FALSE;
}

BOOL CHtmlViewEx::SetCheck(LPCTSTR lpszFieldName)
{
	// TODO : Finish This
	UNUSED_ALWAYS(lpszFieldName); 
	return FALSE;
}

BOOL CHtmlViewEx::ClearCheck(LPCTSTR lpszFieldName)
{
	// TODO : Finish This
	UNUSED_ALWAYS(lpszFieldName); 

	return FALSE;
}

BOOL CHtmlViewEx::SetButtonAction(LPCTSTR lpszFieldName, LPCTSTR lpszAction)
{
	// TODO : Finish This
	UNUSED_ALWAYS(lpszFieldName); 
	UNUSED_ALWAYS(lpszAction); 

	return FALSE;
}

BOOL CHtmlViewEx::SetRadioSelected(LPCTSTR lpszFieldName, BOOL bSelected)
{
	// TODO : Finish This
	UNUSED_ALWAYS(lpszFieldName); 
	UNUSED_ALWAYS(bSelected); 

	return FALSE;
}

BOOL CHtmlViewEx::SetRadioDisabled(LPCTSTR lpszFieldName, BOOL bDisabled)
{
	// TODO : Finish This
	UNUSED_ALWAYS(lpszFieldName); 
	UNUSED_ALWAYS(bDisabled); 

	return FALSE;
}

static HRESULT LoadWebOCFromStream(IHTMLDocument2* pDoc, IStream* pStream)
{
HRESULT hr;
IPersistStreamInit* pPersistStreamInit = NULL;

    // Retrieve the document object.


        // Query for IPersistStreamInit.
        hr = pDoc->QueryInterface( IID_IPersistStreamInit,  (void**)&pPersistStreamInit );
        if ( SUCCEEDED(hr) )
        {
            // Initialize the document.
            hr = pPersistStreamInit->InitNew();
            if ( SUCCEEDED(hr) )
            {
                // Load the contents of the stream.
                hr = pPersistStreamInit->Load( pStream );
            }
            pPersistStreamInit->Release();
        }
	return hr;
}

void	CHtmlViewEx::	NavigateFromString(LPCWSTR lpszDoc)
{
	IDispatch* pDisp = GetHtmlDocument();

	if( !pDisp )
	{
		Navigate(_T("about:blank"));
		pDisp = GetHtmlDocument();
	}
	
	CComQIPtr<IHTMLDocument2>	pDoc( pDisp );

	IStream* pStream = NULL;
	HRESULT hr;
	HGLOBAL hHTMLText;

	int nLength = ::lstrlenW(lpszDoc);
    hHTMLText = GlobalAlloc( GPTR, (nLength) * sizeof(WCHAR) );
	if ( hHTMLText )
    {
		LPWSTR szHTMLText = (LPWSTR)hHTMLText;
		lstrcpynW( szHTMLText, lpszDoc, nLength );
        hr = CreateStreamOnHGlobal( hHTMLText, TRUE, &pStream );
        if ( SUCCEEDED(hr) )
        {
            // Call the helper function to load the WebOC from the stream.
            LoadWebOCFromStream( pDoc, pStream  );
            pStream->Release();
        }
	}
	pDoc.Release();
}

void	CHtmlViewEx::	NavigateFromString(LPCSTR lpszDoc)
{
	IDispatch* pDisp = GetHtmlDocument();

	if( !pDisp )
	{
		Navigate(_T("about:blank"));
		pDisp = GetHtmlDocument();
	}

	CComQIPtr<IHTMLDocument2>	pDoc( pDisp );

	IStream* pStream = NULL;
	HRESULT hr;
	HGLOBAL hHTMLText;

	int nLength = ::lstrlenA(lpszDoc);
	hHTMLText = GlobalAlloc( GPTR, (nLength) * sizeof(CHAR) );
	if ( hHTMLText )
	{
		char* szHTMLText = (LPSTR)hHTMLText;
		memcpy( szHTMLText, lpszDoc, nLength * sizeof(CHAR) );
		hr = CreateStreamOnHGlobal( hHTMLText, TRUE, &pStream );
		if ( SUCCEEDED(hr) )
		{
			// Call the helper function to load the WebOC from the stream.
			LoadWebOCFromStream( pDoc, pStream  );
			pStream->Release();
		}
	}
	pDoc.Release();
}

void	CHtmlViewEx::NavigateFromStream(IStream* pStream)
{
	IDispatch* pDisp = GetHtmlDocument();
	if( !pDisp )
	{
		Navigate( _T("about:blank") );
		pDisp = GetHtmlDocument();
	}
	CComQIPtr<IHTMLDocument2>	pDoc( pDisp );
	LoadWebOCFromStream( pDoc, pStream );
}