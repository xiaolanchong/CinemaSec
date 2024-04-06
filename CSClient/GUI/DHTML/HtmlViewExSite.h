// HtmlViewExSite.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __HTMLFORMSITE_H__
#define __HTMLFORMSITE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __mshtmhst_h__
	#include <mshtmhst.h>
#endif

#include <string>

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewExSite class
//

#undef  AFX_DATA
#define AFX_DATA

class _INS_EXT_CLASS CHtmlViewExSite : public COleControlSite
{

// Construction
public:
	CHtmlViewExSite(COleControlContainer *pCnt)
		: COleControlSite(pCnt)
	{ 
		// Default Settings
		m_bCustomHelp = FALSE;
		m_bCustomMessage = FALSE;
		m_bUseCustomContextMenu = FALSE;
		m_bAdvancedContextMenuHandler = FALSE;

		m_dwFlags = 0;
		m_dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
	};
	
// Attributes
protected:
	// DOCHOSTUIINFO Holders
	std::wstring	m_sHostCss;
	DWORD m_dwFlags;
	DWORD m_dwDoubleClick;

	// Custom Help/Message Toggles
	BOOL m_bCustomHelp;
	BOOL m_bCustomMessage;
	BOOL m_bUseCustomContextMenu;
	BOOL m_bAdvancedContextMenuHandler;

public:
	// DOCHOSTUIINFO Holders Set/Get
	DWORD				GetHostInfo_Flags() const;
	void				SetHostInfo_Flags(DWORD flags);
	DWORD				GetHostInfo_DblClk() const;
	void				SetHostInfo_DblClk(DWORD DoubleClick);

	void				SetHostInfo_HostCss(LPCWSTR szHostCss);

	// Custom UI Toggles
	void				UseCustomHelp(BOOL bSet);
	void				UseCustomMessage(BOOL bSet);
	void				UseCustomContextMenu(BOOL bSet);
	void				UseAdvancedContextMenu(BOOL bSet);

public:
	// Helpers to get pointers to the document dispatch
	// and the attached view
	IDispatch*	GetDispatch() const;
	CView*			GetView() const;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlViewExSite)
	//}}AFX_VIRTUAL

// Implementation
public:
	~CHtmlViewExSite();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CHtmlViewExSite)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// IDocHostUIHandler Interface
protected:
	BEGIN_INTERFACE_PART(DocHostUIHandler, IDocHostUIHandler)
		INIT_INTERFACE_PART(CHtmlViewExSite, DocHostUIHandler)
		STDMETHOD(ShowContextMenu)(/* [in ] */ DWORD hWndID,
		/* [in ] */ POINT __RPC_FAR *ppt,
		/* [in ] */ IUnknown __RPC_FAR *pcmdtReserved,
		/* [in ] */ IDispatch __RPC_FAR *pdispReserved);
		STDMETHOD(GetHostInfo)( 
		/* [out][in ] */ DOCHOSTUIINFO __RPC_FAR *pInfo);
		STDMETHOD(ShowUI)( 
		/* [in ] */ DWORD dwID,
		/* [in ] */ IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
		/* [in ] */ IOleCommandTarget __RPC_FAR *pCommandTarget,
		/* [in ] */ IOleInPlaceFrame __RPC_FAR *pFrame,
		/* [in ] */ IOleInPlaceUIWindow __RPC_FAR *pDoc);
		STDMETHOD(HideUI)(void);
		STDMETHOD(UpdateUI)(void);
		STDMETHOD(EnableModeless)(
		/* [in ] */ BOOL fEnable);
		STDMETHOD(OnDocWindowActivate)(
		/* [in ] */ BOOL fEnable);
		STDMETHOD(OnFrameWindowActivate)(
		/* [in ] */ BOOL fEnable);
		STDMETHOD(ResizeBorder)( 
		/* [in ] */ LPCRECT prcBorder,
		/* [in ] */ IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
		/* [in ] */ BOOL fRameWindow);
		STDMETHOD(TranslateAccelerator)( 
		/* [in ] */ LPMSG lpMsg,
		/* [in ] */ const GUID __RPC_FAR *pguidCmdGroup,
		/* [in ] */ DWORD nCmdID);
		STDMETHOD(GetOptionKeyPath)( 
		/* [out] */ LPOLESTR __RPC_FAR *pchKey,
		/* [in ] */ DWORD dw);
		STDMETHOD(GetDropTarget)(
		/* [in ] */ IDropTarget __RPC_FAR *pDropTarget,
		/* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget);
		STDMETHOD(GetExternal)( 
		/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
		STDMETHOD(TranslateUrl)( 
		/* [in ] */ DWORD dwTranslate,
		/* [in ] */ OLECHAR __RPC_FAR *pchURLIn,
		/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut);
		STDMETHOD(FilterDataObject)( 
		/* [in ] */ IDataObject __RPC_FAR *pDO,
		/* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet);
	END_INTERFACE_PART(DocHostUIHandler)

/////////////////////////////////////////////////////////////////////////////
// IDocHostShowUI Interface
protected:
	BEGIN_INTERFACE_PART(DocHostShowUI, IDocHostShowUI)
		INIT_INTERFACE_PART(CHtmlViewExSite, DocHostShowUI)
		STDMETHOD(ShowHelp)(
		/* [in ] */	HWND hwnd,
		/* [in ] */	LPOLESTR pszHelpFile,
		/* [in ] */	UINT uCommand,
		/* [in ] */	DWORD dwData,
		/* [in ] */	POINT ptMouse,
		/* [out] */	IDispatch __RPC_FAR *pDispatchObjectHit);
		STDMETHOD(ShowMessage)(
		/* [in ] */	HWND hwnd,
		/* [in ] */	LPOLESTR lpstrText,
		/* [in ] */	LPOLESTR lpstrCaption,
		/* [in ] */	DWORD dwType,
		/* [in ] */	LPOLESTR lpstrHelpFile,
		/* [in ] */	DWORD dwHelpContext,
		/* [out] */	LRESULT __RPC_FAR *plResult);
	END_INTERFACE_PART(DocHostShowUI)

/////////////////////////////////////////////////////////////////////////////

	DECLARE_INTERFACE_MAP()
};

#undef  AFX_DATA
#define AFX_DATA

inline DWORD CHtmlViewExSite::GetHostInfo_Flags() const
	{return m_dwFlags;}

inline void CHtmlViewExSite::SetHostInfo_Flags(DWORD flags)
	{m_dwFlags = flags;}

inline void CHtmlViewExSite::SetHostInfo_HostCss(LPCWSTR szHostCss)
	{m_sHostCss = szHostCss ? szHostCss : L"";}

inline DWORD CHtmlViewExSite::GetHostInfo_DblClk() const
	{ return m_dwDoubleClick;}

inline void CHtmlViewExSite::SetHostInfo_DblClk(DWORD DoubleClick)
	{m_dwDoubleClick = DoubleClick;}

inline void	CHtmlViewExSite::UseCustomHelp(BOOL bSet)
	{m_bCustomHelp = bSet;}

inline void	CHtmlViewExSite::UseCustomMessage(BOOL bSet)
	{m_bCustomMessage = bSet;}

inline void CHtmlViewExSite::UseCustomContextMenu(BOOL bSet)
	{m_bUseCustomContextMenu = bSet;}

inline void CHtmlViewExSite::UseAdvancedContextMenu(BOOL bSet)
	{m_bAdvancedContextMenuHandler = bSet;}

/////////////////////////////////////////////////////////////////////////////
// Insert the Html Form OccManager class here
// Beacause it directly uses this class
//
#ifndef __HTMLFORMOCCMANAGER__
#define __HTMLFORMOCCMANAGER__

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewExOccManager class

#undef  AFX_DATA
#define AFX_DATA

class _INS_EXT_CLASS CHtmlViewExOccManager : public COccManager
{
// Construction
public:
	CHtmlViewExSite* m_pHtmlViewExSite;

	CHtmlViewExOccManager() 
	{ 
		m_pHtmlViewExSite = NULL; 
	};

// Implementation
public:
	COleControlSite* CreateSite(COleControlContainer* pCtrlCont)
	{
		m_pHtmlViewExSite = new CHtmlViewExSite(pCtrlCont);
		return m_pHtmlViewExSite;
	}
};

#undef  AFX_DATA
#define AFX_DATA

#endif // __HTMLFORMOCCMANAGER__

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __HTMLFORMSITE_H__

