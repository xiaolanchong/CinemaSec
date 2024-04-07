////////////////////////////////////////////////////////////////
// PixieLib(TM) Copyright 1997-2005 Paul DiLascia
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio.NET 7.1 or greater. Set tabsize=3.
// 
// NOTE: PixieLib is NOT freeware! 
// If you didn't pay for your copy, you're violating my copyright!
//
// ---
// Implementation for CHtmlCtrl -- web browser in a control. Overrides
// CHtmlView so you don't need a frame--can use in dialog or any other kind of
// window.
//
// Features:
// - SetCmdMap lets you set a command map for "app:command" links.
// - SetHTML lets you set document contents (HTML) from string.
//
#include "stdafx.h"
#include "HtmlCtrl.h"
#include "FindWinList.h"
#include <atlsafe.h> // for CComSafeArray

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// macro to declare a typedef for ATL smart poitners; eg SPIHTMLDocument2
#define DECLARE_SMARTPTR(ifacename) typedef CComQIPtr<ifacename> SP##ifacename;
const TCHAR APP_PROTOCOL[] = _T("app:");
const TCHAR IEWNDCLASSNAME[] = _T("Internet Explorer_Server");
const UINT APP_PROTOCOL_LEN = _tcslen(APP_PROTOCOL);

// Return TRUE iff hwnd is internet explorer window.
inline BOOL IsIEWindow(HWND hwnd)
{
	TCHAR classname[32];
	GetClassName(hwnd, classname, sizeof(classname)/sizeof(TCHAR));
	return _tcscmp(classname, IEWNDCLASSNAME)==0;
}

inline BOOL IsAppProtocol(LPCTSTR lpszURL)
{
	return _tcsnicmp(lpszURL, APP_PROTOCOL, APP_PROTOCOL_LEN)==0;
}

// smart pointer to IHTMLDocument2
DECLARE_SMARTPTR(IHTMLDocument2)

// useful macro for checking HRESULTs
#define HRCHECK(x) hr = x; if (!SUCCEEDED(hr)) { \
	TRACE(_T("hr=%p\n"),hr);\
	return hr;\
}

IMPLEMENT_DYNAMIC(CHtmlCtrl, CHtmlView)
BEGIN_MESSAGE_MAP(CHtmlCtrl, CHtmlView)
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

//////////////////
// Create control in same position as an existing static control with given
// the same ID (could be any kind of control, really)
//
BOOL CHtmlCtrl::CreateFromStatic(UINT nID, CWnd* pParent)
{
	CStatic wndStatic;
	if (!wndStatic.SubclassDlgItem(nID, pParent))
		return FALSE;

	// Get static control rect, convert to parent's client coords.
	CRect rc;
	wndStatic.GetWindowRect(&rc);
	pParent->ScreenToClient(&rc);
	wndStatic.DestroyWindow();

	return Create(rc, pParent, nID);
}

////////////////
// Override to avoid CView stuff that assumes a frame.
//
void CHtmlCtrl::OnDestroy()
{
	// This is probably unnecessary since ~CHtmlView does it, but
	// safer to mimic CHtmlView::OnDestroy.
	if (m_pBrowserApp) {
		m_pBrowserApp = NULL;
	}
	CWnd::OnDestroy(); // bypass CView doc/frame stuff
}

////////////////
// Override to avoid CView stuff that assumes a frame.
//
int CHtmlCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT msg)
{
	// bypass CView doc/frame stuff
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, msg);
}

////////////////
// I got focus: to set focus to IE window.
//
void CHtmlCtrl::OnSetFocus(CWnd* /* pOldWnd */)
{
	CFindWinList fwl(m_hWnd, IEWNDCLASSNAME);
	int count = fwl.Fill();
	if (count>0) {								 // if any:
		ASSERT(count==1);						 // ..should only be one!
		::SetFocus(fwl[0]);					 // ..set focus to it
	}
}

//////////////////
// Override to trap "Internet Explorer_Server" window context menu messages.
//
BOOL CHtmlCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (m_bHideMenu) {
		switch (pMsg->message) {
		case WM_CONTEXTMENU:
		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
			if (IsIEWindow(pMsg->hwnd)) {
				if (pMsg->message==WM_RBUTTONUP)
					// let parent handle context menu
					GetParent()->SendMessage(WM_CONTEXTMENU, pMsg->wParam, pMsg->lParam);
				return TRUE; // eat it
			}
		}
	}
	return CHtmlView::PreTranslateMessage(pMsg);
}

//////////////////
// Override to pass "app:" links to virtual fn instead of browser.
//
void CHtmlCtrl::OnBeforeNavigate2( LPCTSTR lpszURL,
	DWORD /* nFlags */,
	LPCTSTR /* lpszTargetFrameName*/,
	CByteArray& /* baPostedData*/,
	LPCTSTR /* lpszHeaders */, BOOL* pbCancel )
{
	if (IsAppProtocol(lpszURL)) {
		OnAppCmd(lpszURL);					 // call virtual handler fn
		*pbCancel = TRUE;						 // cancel navigation
	}
}

//////////////////
// Called when the browser attempts to navigate to "app:foo". Default handler
// searches command map for "foo" command, and sends parent a WM_COMMAND
// message with the ID if found. Call SetCmdMap to set the command map. Only
// override OnAppCmd if you want to do something more complex.
//
void CHtmlCtrl::OnAppCmd(LPCTSTR lpszCmd)
{
	UINT nID = GetAppCommandID(lpszCmd);
	if (nID) {
		// Use PostMessage to avoid problems with exit command. (Let
		// browser finish navigation before issuing command.)
		m_pWndCmd->PostMessage(WM_COMMAND, nID);
	}
}

//////////////////
// Browser is trying to set the status bar text. If this is an "app:cmdname"
// link, replace with prompt string from resource file.
//
void CHtmlCtrl::OnStatusTextChange(LPCTSTR lpszText)
{
	if (lpszText && IsAppProtocol(lpszText)) {
		UINT nID = GetAppCommandID(lpszText);
		if (nID) {
			CString pmt;
			if (pmt.LoadString(nID)) {
				AfxExtractSubString(pmt, pmt, 0); // get first \n substring
				CFrameWnd* pFrame = GetParentFrame();
				if (pFrame != NULL) {
					pFrame->SetMessageText(pmt);
					return;	// eat it!
				}
			}
		}
	}
	CHtmlView::OnStatusTextChange(lpszText); // not mine: pass to base class.
}

//////////////////
// Get command ID from command name
//
UINT CHtmlCtrl::GetAppCommandID(LPCTSTR lpszCmd)
{
	lpszCmd += APP_PROTOCOL_LEN;
	if (m_cmdmap) {
		for (int i=0; m_cmdmap[i].name; i++) {
			if (_tcsicmp(lpszCmd, m_cmdmap[i].name) == 0)
				return m_cmdmap[i].nID;
		}
	}
	return 0;
}

//////////////////
// Set document contents from string
//
HRESULT CHtmlCtrl::SetHTML(LPCTSTR strHTML)
{
	HRESULT hr;

	// Get document object
	SPIHTMLDocument2 doc = GetHtmlDocument();

	// Create string as one-element BSTR safe array for IHTMLDocument2::write.
	CComSafeArray<VARIANT> sar;
	sar.Create(1,0);
	sar[0] = CComBSTR(strHTML);

	// open doc and write
	LPDISPATCH lpdRet;
	HRCHECK(doc->open(CComBSTR("text/html"),
		CComVariant(CComBSTR("_self")),
		CComVariant(CComBSTR("")),
		CComVariant((bool)1),
		&lpdRet));
	
	HRCHECK(doc->write(sar));	// write contents to doc
	HRCHECK(doc->close());		// close
	lpdRet->Release();			// release IDispatch returned

	return S_OK;
}
