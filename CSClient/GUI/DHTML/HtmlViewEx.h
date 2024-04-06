// HtmlViewEx.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __HTMLVIEWEX_H__
#define __HTMLVIEWEX_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __mshtmhst_h__
	#include <mshtmhst.h>
#endif

#include <mshtml.h>			// IE/MSHTML Interface Definitions

// Forward Declarations
class CHtmlViewExOccManager;

// The available commands
#define HTMLID_FIND 1
#define HTMLID_VIEWSOURCE 2
#define HTMLID_OPTIONS 3

// Names for SetSecureLockIcon() values
#define SECURELOCKICON_UNSECURE 						0
#define SECURELOCKICON_MIXED								1
#define SECURELOCKICON_SECURE_UNKNOWNBITS		2
#define SECURELOCKICON_SECURE_40BIT					3
#define SECURELOCKICON_SECURE_56BIT					4
#define SECURELOCKICON_SECURE_FORTEZZA			5
#define SECURELOCKICON_SECURE_128BIT				6

// Kill some of the MFC garbage
#undef  AFX_DATA
#define AFX_DATA


/////////////////////////////////////////////////////////////////////////////
// CHtmlViewEx class

class _INS_EXT_CLASS CHtmlViewEx : public CFormView
{
	DECLARE_DYNCREATE(CHtmlViewEx)

///////////////////////////////////////////////////////////////////
// Construction
///////////////////////////////////////////////////////////////////
public:
	CHtmlViewEx();

///////////////////////////////////////////////////////////////////
// Attributes
///////////////////////////////////////////////////////////////////
protected:
	CHtmlViewExOccManager* m_pHtmlViewExOccManager;
	int m_nFontSize;
	BOOL m_bNoStatusText;
	BOOL m_bOfflineIfNotConnected;
	BOOL m_bSilentMode;
	CString m_strUserAgent;

public:
	void				SetNoStatusText(BOOL bNoStatus = TRUE);
	int					GetFontSize() const;
	void				SetFontSize(int fontSize = 2);
	CString			GetType() const;
	long				GetLeft() const;
	void				SetLeft(long nNewValue);
	long				GetTop() const;
	void				SetTop(long nNewValue);
	long				GetHeight() const;
	void				SetHeight(long nNewValue);
	void				SetVisible(BOOL bNewValue);
	BOOL				GetVisible() const;
	CString			GetLocationName() const;
	READYSTATE	GetReadyState() const;
	BOOL				GetOffline() const;
	void				SetOffline(BOOL bNewValue);
	BOOL				GetSilent() const;
	void				SetSilent(BOOL bNewValue);
	BOOL				GetTopLevelContainer() const;
	CString			GetLocationURL() const;
	BOOL				GetBusy() const;
	LPDISPATCH	GetApplication() const;
	LPDISPATCH	GetParentBrowser() const;
	LPDISPATCH	GetContainer() const;
	LPDISPATCH	GetHtmlDocument() const;
	CString			GetFullName() const;
	int					GetToolBar() const;
	void				SetToolBar(int nNewValue);
	BOOL				GetMenuBar() const;
	void				SetMenuBar(BOOL bNewValue);
	BOOL				GetFullScreen() const;
	void				SetFullScreen(BOOL bNewValue);
	OLECMDF			QueryStatusWB(OLECMDID cmdID) const;
	BOOL				GetRegisterAsBrowser() const;
	void				SetRegisterAsBrowser(BOOL bNewValue);
	BOOL				GetRegisterAsDropTarget() const;
	void				SetRegisterAsDropTarget(BOOL bNewValue);
	BOOL				GetTheaterMode() const;
	void				SetTheaterMode(BOOL bNewValue);
	BOOL				GetAddressBar() const;
	void				SetAddressBar(BOOL bNewValue);
	BOOL				GetStatusBar() const;
	void				SetStatusBar(BOOL bNewValue);
	CString			GetUserAgent() const;
	void				SetUserAgent(LPCTSTR strNewValue);

	// heuz
	void				SetHostInfo_HostCss(LPCWSTR szHostCss );

///////////////////////////////////////////////////////////////////
// Operations
///////////////////////////////////////////////////////////////////

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

///////////////////////////////////////////////////////////////////
// New CHtmlViewEx Operations
//
public:
	// Helper for OLE Commands to the WebBrowser
	void				ExecCmdTarget(DWORD nCmdID);

	// Helper for parsing POST Data
	CString			GetValueFromPostData(LPCTSTR strPostData, LPCTSTR strValName);

	///////////////////////////////////////////////////////////////////
	// Setup Functions
	///////////////////////////////////////////////////////////////////
	// These functions are primarily for doing the initial setup
	// of a form upon load.  Most other actions should be performed
	// within the form page itself.
	///////////////////////////////////////////////////////////////////
public:
	IDispatch*	GetInputElementByName(LPCTSTR lpszElementName);
	IDispatch*	GetSelectElementByName(LPCTSTR lpszElementName);
	IDispatch*	GetTextAreaElementByName(LPCTSTR lpszElementName);

protected:
	IHTMLOptionElementFactory*	GetOptionElementFactory();

public:
	///////////////////////////////////////////////////////////////////
	// General Input Controls
	BOOL				SetInputTextValue(LPCTSTR lpszFieldName, LPCTSTR lpszValue);
	CString			GetInputTextValue(LPCTSTR lpszFieldName);

	///////////////////////////////////////////////////////////////////
	// Drop-down list / list box
	BOOL				RemoveAllOptionElements(LPCTSTR lpszFieldName);
	BOOL				AddOptionElement(LPCTSTR lpszFieldName, LPCTSTR lpszOptionText, LPCTSTR lpszOptionValue = NULL,	BOOL bSelected = FALSE,	BOOL bDefault = FALSE, long nBefore = -1);
	BOOL				ClearSelection(LPCTSTR lpszFieldName);
	BOOL				SetOptionSelected(LPCTSTR lpszFieldName,LPCTSTR lpszOptionText);

	///////////////////////////////////////////////////////////////////
	// Checkbox Specific
	BOOL				SetCheck(LPCTSTR lpszFieldName);
	BOOL				ClearCheck(LPCTSTR lpszFieldName);

	///////////////////////////////////////////////////////////////////
	// Regular Button Specific
	BOOL				SetButtonAction(LPCTSTR lpszFieldName, LPCTSTR lpszAction);

	///////////////////////////////////////////////////////////////////
	// Radio Button Specific
	BOOL				SetRadioSelected(LPCTSTR lpszFieldName, BOOL bSelected);
	BOOL				SetRadioDisabled(LPCTSTR lpszFieldName, BOOL bDisabled);

public:
	///////////////////////////////////////////////////////////////////
	// NOTE: If you use these functions, you must navigate to a 
	//       page for them to take effect!
	//
	// Use "Navigate2(GetLocationURL(), 0, NULL, NULL);" to navigate
	// to the page currently displayed
	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	// DocHostUIHandler DoubleClick interface functions
	DWORD				GetUI_DblClk() const;
	void				SetUI_DblClk(DWORD dwSet);

	///////////////////////////////////////////////////////////////////
	// DocHostUIHandler Flags interface functions
	DWORD				GetUI_Flags() const;
	void				SetUI_Flags(DWORD dwSet);

	BOOL				GetUIFlag_Dialog() const;
	BOOL				GetUIFlag_DisableHelpMenu() const;
	BOOL				GetUIFlag_No3dBorder() const;
	BOOL				GetUIFlag_NoScrollbar() const;
	BOOL				GetUIFlag_DisableScriptInactive() const;
	BOOL				GetUIFlag_OpenNewWin() const;
	BOOL				GetUIFlag_DisableOffscreen() const;
	BOOL				GetUIFlag_FlatScrollbar() const;
	BOOL				GetUIFlag_DivBlockDefault() const;
	BOOL				GetUIFlag_ActivateClientHitOnly() const;

	void				SetUIFlag_Dialog(BOOL bSet);
	void				SetUIFlag_DisableHelpMenu(BOOL bSet);
	void				SetUIFlag_No3dBorder(BOOL bSet);
	void				SetUIFlag_NoScrollbar(BOOL bSet);
	void				SetUIFlag_DisableScriptInactive(BOOL bSet);
	void				SetUIFlag_OpenNewWin(BOOL bSet);
	void				SetUIFlag_DisableOffscreen(BOOL bSet);
	void				SetUIFlag_FlatScrollbar(BOOL bSet);
	void				SetUIFlag_DivBlockDefault(BOOL bSet);
	void				SetUIFlag_ActivateClientHitOnly(BOOL bSet);

///////////////////////////////////////////////////////////////////
// Basic CHtmlView Operations
//
public:
	void				GoBack();
	void				GoForward();
	void				GoHome();
	void				GoSearch();
	void				Navigate(LPCTSTR URL, DWORD dwFlags = 0, LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL, DWORD dwPostDataLen = 0);
	void				Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags = 0, LPCTSTR lpszTargetFrameName = NULL);
	void				Navigate2(LPCTSTR lpszURL, DWORD dwFlags = 0, LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL, DWORD dwPostDataLen = 0);
	void				Navigate2(LPCTSTR lpszURL, DWORD dwFlags, CByteArray& baPostedData, LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeader = NULL);

	void				NavigateFromString(LPCWSTR lpszDoc);
	void				NavigateFromString(LPCSTR lpszDoc);
	void				NavigateFromStream(IStream* pStream);

	void				Refresh();
	void				Refresh2(int nLevel);
	void				Stop();
	void				PutProperty(LPCTSTR lpszProperty, const VARIANT& vtValue);
	void				PutProperty(LPCTSTR lpszPropertyName, double dValue);
	void				PutProperty(LPCTSTR lpszPropertyName, LPCTSTR lpszValue);
	void				PutProperty(LPCTSTR lpszPropertyName, long lValue);
	void				PutProperty(LPCTSTR lpszPropertyName, short nValue);
	BOOL				GetProperty(LPCTSTR lpszProperty, CString& strValue);
	COleVariant GetProperty(LPCTSTR lpszProperty);
	void				ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt, VARIANT* pvaIn, VARIANT* pvaOut);
	BOOL				LoadFromResource(LPCTSTR lpszResource);
	BOOL				LoadFromResource(UINT nRes);

///////////////////////////////////////////////////////////////////
// Overrides
///////////////////////////////////////////////////////////////////
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlViewEx)
	public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,CCreateContext* pContext = NULL);
	virtual BOOL OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar);
	//}}AFX_VIRTUAL

///////////////////////////////////////////////////////////////////
// Overridable Event Notifications
//
public:
	virtual void OnNavigateComplete2(LPCTSTR strURL);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,	LPCTSTR lpszTargetFrameName, LPCTSTR strPostedData,	LPCTSTR lpszHeaders, BOOL* pbCancel);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,	LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,	LPCTSTR lpszHeaders, BOOL* pbCancel);
	virtual void OnStatusTextChange(LPCTSTR lpszText);
	virtual void OnProgressChange(long nProgress, long nProgressMax);
	virtual void OnCommandStateChange(long nCommand, BOOL bEnable);
	virtual void OnDownloadBegin();
	virtual void OnDownloadComplete();
	virtual void OnTitleChange(LPCTSTR lpszText);
	virtual void OnPropertyChange(LPCTSTR lpszProperty);
	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
	virtual void OnDocumentComplete(LPCTSTR lpszURL);
	virtual void OnQuit();
	virtual void OnVisible(BOOL bVisible);
	virtual void OnToolBar(BOOL bToolBar);
	virtual void OnMenuBar(BOOL bMenuBar);
	virtual void OnStatusBar(BOOL bStatusBar);
	virtual void OnFullScreen(BOOL bFullScreen);
	virtual void OnTheaterMode(BOOL bTheaterMode);
	virtual void OnFilePrint();

	// Formerly unimplemented
	virtual void OnAddressBar(BOOL bAddressBar);
	virtual void OnAppCmd(LPCTSTR lpszAppCmd, LPCTSTR PostData);
	virtual void OnAdvancedContextMenu(DWORD dwID, CPoint ptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit);
	virtual void OnShowHelp(HWND hwnd, LPCTSTR pszHelpFile,UINT uCommand,DWORD dwData,POINT ptMouse,	IDispatch __RPC_FAR *pDispatchObjectHit);
	virtual void OnShowMessage(HWND hwnd, LPCTSTR lpstrText,	LPCTSTR lpstrCaption,	DWORD dwType,	LPCTSTR lpstrHelpFile,	DWORD dwHelpContext, LRESULT __RPC_FAR *plResult);
	virtual void OnViewSource();
	virtual void OnToolsInternetOptions();
	virtual void OnEditCut();
	virtual void OnEditCopy();
	virtual void OnEditPaste();
	virtual void OnEditSelectall();
	virtual void OnEditFindOnThisPage();

	virtual void OnWindowSetResizable(BOOL Resizable);
	virtual void OnWindowClosing(BOOL IsChildWindow, BOOL* Cancel);
	virtual void OnWindowSetLeft(long left);
	virtual void OnWindowSetTop(long Top);
	virtual void OnWindowSetWidth(long Width);
	virtual void OnWindowSetHeight(long Height);
	virtual void OnClientToHostWindow(long* CX, long* CY);
	virtual void OnSetSecureLockIcon(long SecureLockIcon);
	virtual void OnFileDownload(BOOL* Cancel);

///////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////
protected:
	IWebBrowser2* m_pBrowserApp;

public:
	
	virtual ~CHtmlViewEx();
	CWnd m_wndBrowser;

///////////////////////////////////////////////////////////////////
// Event reflectors (not normally overridden)
protected:
	virtual void NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL);
	virtual void BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers,   BOOL* Cancel);
	virtual void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);

///////////////////////////////////////////////////////////////////
// Internal Helper Functions
protected:
	char					CharTokenToChar(const CString &val);
	CString				ProcessCharCodes(LPCTSTR lpszInput);
	void					RemoveAllOptionElements_(IHTMLSelectElement* pSelectElem);

	// Custom UI Toggles
	void				UseCustomHelp(BOOL bSet);
	void				UseCustomMessage(BOOL bSet);
	void				UseCustomContextMenu(BOOL bSet);
	void				UseAdvancedContextMenu(BOOL bSet);

///////////////////////////////////////////////////////////////////
// Generated message map functions
protected:
	//{{AFX_MSG(CHtmlViewEx)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
};

#undef  AFX_DATA
#define AFX_DATA

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

///////////////////////////////////////////////////////////////////
// Inline Functions (They looked too messy in this file)
#include "HtmlViewEx.inl"

#endif // __HTMLVIEWEX_H__

