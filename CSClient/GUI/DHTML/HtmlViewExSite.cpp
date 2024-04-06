// HtmlViewExSite.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
//#include "Resource.h"

#undef AFX_DATA
#define AFX_DATA AFX_DATA_IMPORT
#if _MSC_VER > 1200
#include <afxocc.h>
#else
#include <..\src\occimpl.h>
#endif
#undef AFX_DATA
#define AFX_DATA AFX_DATA_EXPORT
#include "HtmlViewEx.h"
#include "HtmlViewExSite.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewExSite

CHtmlViewExSite::~CHtmlViewExSite()
{
	// Nothing...
}

BEGIN_MESSAGE_MAP(CHtmlViewExSite, COleControlSite)
	//{{AFX_MSG_MAP(CHtmlViewExSite)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CHtmlViewExSite, COleControlSite)
	INTERFACE_PART(CHtmlViewExSite, IID_IDocHostUIHandler, DocHostUIHandler )
	INTERFACE_PART(CHtmlViewExSite, IID_IDocHostShowUI,    DocHostShowUI    )
END_INTERFACE_MAP()

//////////////////////////////////////////////////////////////////////
// Basic Methods for IUnknown Interfaces
// See Globals.h for macro definition
//////////////////////////////////////////////////////////////////////
IMPLEMENT_IUNKNOWN( CHtmlViewExSite, DocHostShowUI    );
IMPLEMENT_IUNKNOWN( CHtmlViewExSite, DocHostUIHandler );

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewExSite Helper Functions

/////////////////////////////////////////////////////////////////////////////
// GetDispatch()
//
// This function obtains the dispatch interface of the document 
// attached to the view this site contains.  My own implementation
// requires that the HTML form be linked to the document.  If you
// want to implement the IDispatch somewhere else, you will need
// to modify this code.
//
IDispatch* CHtmlViewExSite::GetDispatch() const
{ 
	IDispatch* pDisp = NULL;

	// Spread this out for easier debugging
	CView* pView = GetView();
	if(pView)
	{
		CDocument* pDoc = pView->GetDocument();
		if(pDoc)
		{
			// Just need the pointer, don't AddRef
			pDisp = pDoc->GetIDispatch(FALSE);
		}
	}

	return pDisp; 
}

/////////////////////////////////////////////////////////////////////////////
// GetView()
//
// This function obtains the view this site contains. It is 
// really only used twice within the code: once above in 
// GetDispatch and once again in ShowContextMenu.
//
CView* CHtmlViewExSite::GetView() const
{ 
	CView* pView = NULL;

	// Spread this out for easier debugging
	CWnd* pWnd = m_pCtrlCont->m_pWnd;
	if(pWnd)
	{
		pView = (CView*)pWnd;
	}

	return pView; 
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewExSite diagnostics

#ifdef _DEBUG
void CHtmlViewExSite::AssertValid() const
{
	// TODO: add object validation code here
	// or call the base class
	COleControlSite::AssertValid();
}

void CHtmlViewExSite::Dump(CDumpContext& dc) const
{
	// TODO: add object dumping code here
	// or call the base class
	COleControlSite::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewExSite::XDocHostUIHandler Functions
//
// Save our warning level
#pragma warning(push)	
// Disable the annoying warning 4100 - Unused Formal Parameter!
#pragma warning(disable:4100)

///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::GetExternal
// 
// Purpose:
//   This is where the IDispatch gets assigned, I reccommend
// you use a member variable in your application's document
// object as it reduces the MFC headaches.
//
// If you plan on moving it somewhere else, modify the function
// CHtmlViewExSite::GetDispatch() to return a pointer to your 
// dispatch interface.
///////////////////////////////////////////////////////////////////

STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::GetExternal( 
	/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
{
	METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler);

	#ifdef _DEBUG
		TRACE( _T("XDocHostUIHandler::GetExternal Called - "));
	#endif // _DEBUG

	IDispatch* pDisp = pThis->GetDispatch();
	
	if (pDisp !=NULL)
	{ // The pointer is set, so return it
		pDisp->AddRef();
		*ppDispatch = pDisp;
		#ifdef _DEBUG
			TRACE(_T("Succeeded!\n"));
		#endif // _DEBUG
		return S_OK;
	}
	else
	{ // Handle this gracefully
		*ppDispatch = NULL;
		#ifdef _DEBUG
			TRACE(_T("Failed!\n"));
		#endif // _DEBUG
		return E_PENDING;		// Dispatch Not Available Yet
	}

	#if (defined _DEBUG) &&  _MSC_VER <= 1200
		TRACE(_T("ARGGG!\n"));
	#endif // _DEBUG
}


///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::GetHostInfo
// 
// Purpose: 
//		Called at initialization to get various configuration items
//	for (in this case) the browser control.
///////////////////////////////////////////////////////////////////
// HRESULT FAR EXPORT CHtmlViewExSite::XDocHostUIHandler::GetHostInfo( DOCHOSTUIINFO* pInfo )
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::GetHostInfo( DOCHOSTUIINFO* pInfo )
{
	METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)

// Options include :
//	DOCHOSTUIFLAG_DIALOG 
//		IE4/MSHTML will not allow selection of the text in the form. 
//	DOCHOSTUIFLAG_DISABLE_HELP_MENU 
//		IE4/MSHTML will not display context menus. 
//	DOCHOSTUIFLAG_NO3DBORDER 
//		IE4/MSHTML does not use 3-D borders. 
//	DOCHOSTUIFLAG_SCROLL_NO 
//		IE4/MSHTML does not have scroll bars. 
//	DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE 
//		IE4/MSHTML will not execute any script when loading pages. 
//	DOCHOSTUIFLAG_OPENNEWWIN 
//		IE4/MSHTML will open a site in a new window when a link 
//		is clicked rather than browse to the new site using the
//		same browser window. 
//	DOCHOSTUIFLAG_DISABLE_OFFSCREEN 
//		Not implemented or used. 
//	DOCHOSTUIFLAG_FLAT_SCROLLBAR 
//		IE4/MSHTML will use flat scroll bars for any UI it 
//		displays. Not currently supported. 
//	DOCHOSTUIFLAG_DIV_BLOCKDEFAULT 
//		IE4/MSHTML will insert the <DIV> tag if a return is 
//		entered in edit mode. Without this flag, IE4/MSHTML 
//		will use the <P> tag. 
//	DOCHOSTUIFLAG_ACTIVATE_CLIENTHIT_ONLY 
//		IE4/MSHTML will only become UI-active if the mouse 
//		is clicked in the client area of the window. It will 
//		not become UI-active if the mouse is clicked on a 
//		nonclient area, such as a scroll bar. 

	pInfo->dwFlags = pThis->m_dwFlags;

// Options include :
// DOCHOSTUIDBLCLK_DEFAULT 					// Perform the default action. 
// DOCHOSTUIDBLCLK_SHOWPROPERTIES		// Show the item's properties. 
// DOCHOSTUIDBLCLK_SHOWCODE					// Show the page's source. 

  pInfo->dwDoubleClick = pThis->m_dwDoubleClick;
  // heuz
  if( !pThis->m_sHostCss.empty()  )
  {
	  size_t nSize = pThis->m_sHostCss.size();
	WCHAR* pMem = (WCHAR*)CoTaskMemAlloc(  (nSize+1) * sizeof( WCHAR) );
	memcpy( pMem, pThis->m_sHostCss.c_str(), nSize * sizeof( WCHAR) );
	pMem[nSize] = L'\0';
	pInfo->pchHostCss		= pMem;
  }

  return S_OK;
}

///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::ShowUI
//
// Purpose: Called when IE4/MSHTML shows its UI
///////////////////////////////////////////////////////////////////
// HRESULT FAR EXPORT  
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::ShowUI(
	DWORD dwID, 
	IOleInPlaceActiveObject * /*pActiveObject*/,
	IOleCommandTarget * pCommandTarget,
	IOleInPlaceFrame * /*pFrame*/,
	IOleInPlaceUIWindow * /*pDoc*/)
{
	#ifdef _DEBUG
		TRACE(_T("XDocHostUIHandler::ShowUI Called!\n"));
	#endif // _DEBUG

	//METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)
	// We've got our own UI in place so just return S_OK
  return S_OK;
}

///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::HideUI
//
// Purpose: Called when IE4/MSHTML hides its UI
///////////////////////////////////////////////////////////////////
// HRESULT FAR EXPORT  
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::HideUI(void)
{
	#ifdef _DEBUG
		TRACE(_T("XDocHostUIHandler::HideUI Called!\n"));
	#endif // _DEBUG

	//METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)
	// We've got our own UI in place so just return S_OK
  return S_OK;
}

///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::UpdateUI
//
// Purpose: Called when IE4/MSHTML updates its UI
///////////////////////////////////////////////////////////////////
// HRESULT FAR EXPORT  
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::UpdateUI(void)
{
	#ifdef _DEBUG
		// I'm turning this off - it gets called a LOT
		//TRACE("XDocHostUIHandler::UpdateUI Called!\n");
	#endif // _DEBUG

	// METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)

	// MFC is pretty good about updating the UI in it's Idle 
	// loop so we don't do anything here, just return S_OK
	return S_OK;
}

///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::EnableModeless
//
// Purpose: Called from IE4/MSHTML's 
// IOleInPlaceActiveObject::EnableModeless
///////////////////////////////////////////////////////////////////
// HRESULT FAR EXPORT  
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::EnableModeless(
	BOOL /*fEnable*/)
{
	#ifdef _DEBUG
		TRACE(_T("XDocHostUIHandler::EnableModeless Called!\n"));
	#endif // _DEBUG


	METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::OnDocWindowActivate
//
// Purpose: 
// Called from IE4/MSHTML's 
// IOleInPlaceActiveObject::OnDocWindowActivate
///////////////////////////////////////////////////////////////////
// HRESULT FAR EXPORT  
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::OnDocWindowActivate(
	BOOL /*fActivate*/)
{
	#ifdef _DEBUG
		TRACE(_T("XDocHostUIHandler::OnDocWindowActivate Called!\n"));
	#endif // _DEBUG


	METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::OnFrameWindowActivate
//
// Purpose: 
// Called from IE4/MSHTML's 
// IOleInPlaceActiveObject::OnFrameWindowActivate
///////////////////////////////////////////////////////////////////
// HRESULT FAR EXPORT  
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::OnFrameWindowActivate(
	BOOL /*fActivate*/)
{
	#ifdef _DEBUG
		TRACE(_T("XDocHostUIHandler::OnFrameWindowActivate Called!\n"));
	#endif // _DEBUG

	METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::ResizeBorder
//
// Purpose: Called from IE4/MSHTML's 
// IOleInPlaceActiveObject::ResizeBorder
///////////////////////////////////////////////////////////////////
// HRESULT FAR EXPORT  
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::ResizeBorder(
	LPCRECT /*prcBorder*/, 
	IOleInPlaceUIWindow* /*pUIWindow*/,
	BOOL /*fRameWindow*/)
{
	#ifdef _DEBUG
		TRACE(_T("XDocHostUIHandler::ResizeBorder Called!\n"));
	#endif // _DEBUG

	METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::ShowContextMenu
//
// Purpose: 
// Called when IE4/MSHTML would normally display its context menu
///////////////////////////////////////////////////////////////////
// HRESULT FAR EXPORT  
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::ShowContextMenu(
	DWORD dwID, 
	POINT* pptPosition,
	IUnknown* pCommandTarget,
	IDispatch* pDispatchObjectHit)
{
	#ifdef _DEBUG
		TRACE(_T("XDocHostUIHandler::ShowContextMenu Called!\n"));
	#endif // _DEBUG

	METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)

	// Place your own context menu commands here
	// or use my view callback
	CPoint pos(*pptPosition);

	if(pThis->m_bUseCustomContextMenu)
	{
		if(pThis->GetView() != NULL)
		{
			if(pThis->m_bAdvancedContextMenuHandler)
			{
				// Advanced option - call the user override
				((CHtmlViewEx*)(pThis->GetView()))->OnAdvancedContextMenu(
						dwID, pos, pCommandTarget, pDispatchObjectHit);
			}
			else
			{
				// Regular handler - Send a message to the view
				HWND hWnd = pThis->GetView()->GetSafeHwnd();
				LPARAM lParam; 
				lParam = MAKELPARAM(pos.x, pos.y);
				::SendMessage(hWnd,WM_CONTEXTMENU,NULL,lParam);
			}
			
		}
		else
		{
#ifdef _DEBUG
			TRACE(_T("ShowContextMenu - View is null!\n"));
#endif // _DEBUG
		}
			
	  return S_OK; 
	}
	else
	{
		// Return E_NOTIMPL or S_FALSE if you want the web browser
		// control to show it's own menu.
		return E_NOTIMPL;
	}
}



///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::TranslateAccelerator
//
// Purpose: 
// Called from IE4/MSHTML's TranslateAccelerator routines
///////////////////////////////////////////////////////////////////
// HRESULT FAR EXPORT  
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::TranslateAccelerator(LPMSG lpMsg,
	/* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
	/* [in] */ DWORD nCmdID)
{
	#ifdef _DEBUG
	// I'm turning this off!
	// It gets called everytime a key is pressed :-O
	// TRACE("XDocHostUIHandler::TranslateAccelerator Called!\n");
	#endif // _DEBUG

	METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)

	return S_FALSE;
}

///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::GetOptionKeyPath
//
// Purpose: 
// Returns the registry key under which IE4/MSHTML stores user
// preferences. Returns S_OK if successful, or S_FALSE otherwise.
// If S_FALSE, IE4/MSHTML will default to its own user options.
// 
// Parameters
// pchKey
//	 Address of an LPOLESTR that receives the registry subkey
//   string where the host stores its default options. This
//   subkey will be under the HKEY_CURRENT_USER key. The 
//   implementation must allocate this memory using CoTaskMemAlloc.
//   The calling application is responsible for freeing this
//   memory using CoTaskMemFree. Even if the method fails, 
//   this parameter should be set to NULL. 
// dwReserved 
//   Reserved for future use. Not currently used. 
//
// If pchKey is null on return from this function, then IE4/MSHTML
// uses the default location for storing options in the registry. 
///////////////////////////////////////////////////////////////////
// HRESULT FAR EXPORT  
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::GetOptionKeyPath(
	LPOLESTR *pchKey, 
	DWORD /*dwReserved*/)
{
	#ifdef _DEBUG
		TRACE(_T("XDocHostUIHandler::GetOptionKeyPath Called!\n"));
	#endif // _DEBUG

	METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)

//	return E_NOTIMPL;

	CString strAppKey;

	// Load up a sub key to the running application
	strAppKey.Format( _T("Software\\%s\\%s\\WebBrowser"),
		AfxGetApp()->m_pszRegistryKey, AfxGetApp()->m_pszAppName);
	
#ifdef _DEBUG
	TRACE1("GetOptionKeyPath Returning '%s'\n",strAppKey);
#endif // _DEBUG

	LPOLESTR szAppKey = NULL;

	szAppKey = (LPOLESTR)strAppKey.AllocSysString();

	TRACE0("szAppKey = ");
	TRACE(FROM_OLE_STRING(szAppKey));
	TRACE0("\n");

	*pchKey = (OLECHAR*)CoTaskMemAlloc(sizeof(OLECHAR)*(wcslen(szAppKey)+1));

	if (*pchKey)
	{
		wcscpy(*pchKey,szAppKey);

		TRACE0("pchKey = ");
		TRACE(FROM_OLE_STRING(*pchKey));
		TRACE0("\n");
		
		return S_OK;
	}
	else
	{
		*pchKey = NULL;
		return E_OUTOFMEMORY;
	}
	
}

///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::GetDropTarget
//
// Purpose: 
// If you need to monkey around with Drag & Drop functionality
// here is where you need to do it.  Otherwise, leave it alone!
//
// See IDocHostUIHandler::GetDropTarget in the Platform SDK
// for more information
///////////////////////////////////////////////////////////////////
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::GetDropTarget( 
	/* [in] */ IDropTarget __RPC_FAR *pDropTarget,
	/* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget)
{
	#ifdef _DEBUG
		TRACE0("XDocHostUIHandler::GetDropTarget Called!\n");
	#endif // _DEBUG


	METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::TranslateUrl
//
// Purpose: 
// Called by IE4/MSHTML to allow the host an opportunity to 
// modify the URL to be loaded. 
// Return S_OK if the URL was translated, S_FALSE if the URL was
// not translated, or an OLE-defined error code if an error 
// occurred.
//
// Parameters
//	dwTranslate 
//		Reserved for future use. 
//	pchURLIn 
//		Address of a string supplied by IE4/MSHTML that represents
//		the URL to be translated. 
//	ppchURLOut 
//		Address of a string pointer that receives the address of 
//		the translated URL. The host allocates the buffer using 
//		the task memory allocator. The contents of this parameter
//		should always be initialized to NULL, even if the URL is 
//		not translated or the method fails. 
///////////////////////////////////////////////////////////////////
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::TranslateUrl( 
	/* [in] */ DWORD dwTranslate,
	/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
	/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut)
{
	#ifdef _DEBUG
		TRACE0("XDocHostUIHandler::TranslateUrl Called!\n");
	#endif // _DEBUG

	METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////
// XDocHostUIHandler::FilterDataObject
//
// Purpose: 
// Called on the host by IE4/MSHTML to allow the host to replace 
// IE4/MSHTML's data object. This allows the host to block 
// certain clipboard formats or support additional clipboard 
// formats. 
///////////////////////////////////////////////////////////////////
STDMETHODIMP CHtmlViewExSite::XDocHostUIHandler::FilterDataObject( 
	/* [in] */ IDataObject __RPC_FAR *pDO,
	/* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet)
{
	#ifdef _DEBUG
		TRACE0("XDocHostUIHandler::FilterDataObject Called!\n");
	#endif // _DEBUG

	METHOD_PROLOGUE(CHtmlViewExSite, DocHostUIHandler)
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewExSite::XDocHostShowUI Functions
//

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewExSite::XDocHostShowUI functions

/////////////////////////////////////////////////////////////////////////////
// ShowHelp
//
// Called by IE4/MSHTML when it needs to show Help. 
// 
// Return Values: 
//   S_OK  
//     Host displayed Help. IE4/MSHTML will not display 
//     its Help.  
//   S_FALSE  
//     Host did not display Help. IE4/MSHTML will 
//     display its Help.  
//
// Parameters:
//   hwnd 
//     Handle to the owner window. 
//   pszHelpFile 
//     Help file name. 
//   uCommand 
//     Type of Help. See the WinHelp function. 
//   dwData 
//     Additional data. See the WinHelp function. 
//   ptMouse 
//     Mouse position in screen coordinates. 
//   pDispatchObjectHit 
//     IDispatch of the object at the screen coordinates. 
//
// The first four parameters are the same as those passed 
// to the WinHelp function. The ptMouse parameter gives 
// the screen coordinates, and the pDispatchObjectHit 
// parameter allows the host to differentiate the object
// at the screen coordinates to a greater degree than 
// IE4/MSHTML. 
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CHtmlViewExSite::XDocHostShowUI::ShowHelp(
	/* [in ] */	HWND hwnd,
	/* [in ] */	LPOLESTR pszHelpFile,
	/* [in ] */	UINT uCommand,
	/* [in ] */	DWORD dwData,
	/* [in ] */	POINT ptMouse,
	/* [out] */	IDispatch __RPC_FAR *pDispatchObjectHit)
{
	#ifdef _DEBUG
		TRACE0("XDocHostShowUI::ShowHelp Called!\n");
	#endif // _DEBUG

	METHOD_PROLOGUE(CHtmlViewExSite, DocHostShowUI)

	CString strHelpFile;
	if(pszHelpFile !=NULL)
		strHelpFile = FROM_OLE_STRING(pszHelpFile);

	if(pThis->m_bCustomHelp)
	{
		// Re-direct to let the view handle it
		((CHtmlViewEx*)pThis->GetView())->OnShowHelp(
			hwnd, strHelpFile, uCommand, dwData, 
			ptMouse,	pDispatchObjectHit);
		return S_OK;
	}
	else  //  Use the default implementation
		return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ShowMessage
//
// Called by IE4/MSHTML when it needs to display a message box.
//
// Return Values: 
//   S_OK  
//     Host displayed its UI. IE4/MSHTML will not display
//     its message box.  
//   S_FALSE  
//     Host did not display its UI. IE4/MSHTML will 
//     display its message box.  
//
// Parameters:
//   hwnd 
//     Handle to the owner window. 
//   lpstrText 
//     Text for the message box. 
//   lpstrCaption 
//     Caption for the message box. 
//   dwType 
//     Type flags (taken from theMessageBox MB_xxxx constants). 
//   lpstrHelpFile 
//     Help file name. 
//   dwHelpContext 
//     Help context identifier. 
//   plResult 
//     Button clicked by user (taken from theMessageBox IDxxx constants).
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CHtmlViewExSite::XDocHostShowUI::ShowMessage(
	/* [in ] */	HWND hwnd,
	/* [in ] */	LPOLESTR lpstrText,
	/* [in ] */	LPOLESTR lpstrCaption,
	/* [in ] */	DWORD dwType,
	/* [in ] */	LPOLESTR lpstrHelpFile,
	/* [in ] */	DWORD dwHelpContext,
	/* [out] */	LRESULT __RPC_FAR *plResult)
{
	#ifdef _DEBUG
		TRACE0("XDocHostShowUI::ShowMessage Called!\n");
	#endif // _DEBUG

	METHOD_PROLOGUE(CHtmlViewExSite, DocHostShowUI)

	CString strText;
	if(lpstrText !=NULL)
		strText = FROM_OLE_STRING(lpstrText);

	CString strCaption;
	if(lpstrCaption !=NULL)
		strCaption = FROM_OLE_STRING(lpstrCaption);

	CString strHelpFile;
	if(lpstrHelpFile !=NULL)
		strHelpFile = FROM_OLE_STRING(lpstrHelpFile);

	if(pThis->m_bCustomMessage)
	{
	// Re-direct to let the view handle it
		((CHtmlViewEx*)pThis->GetView())->OnShowMessage(
			hwnd, strText, strCaption, dwType,
			strHelpFile, dwHelpContext, plResult);
		return S_OK;
	}
	else  //  Use the default implementation
		return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// We're done, restore the warning level
#pragma warning(pop)
/////////////////////////////////////////////////////////////////////////////

