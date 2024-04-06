//#ifndef __HTMLVIEWEX_INLINES__
//#define __HTMLVIEWEX_INLINES__

/////////////////////////////////////////////////////////////////////////////
// Lots of inline function declarations

// Handy little tidbit to turn off the status bar updates
// The actual code which does this is in OnStatusTextChange()

inline void CHtmlViewEx::SetNoStatusText(BOOL bNoStatus)
{	
	m_bNoStatusText = bNoStatus; 
}

inline void CHtmlViewEx::SetRegisterAsBrowser(BOOL bNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->put_RegisterAsBrowser((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); 
}

inline void CHtmlViewEx::SetRegisterAsDropTarget(BOOL bNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->put_RegisterAsDropTarget((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); 
}

inline void CHtmlViewEx::SetTheaterMode(BOOL bNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->put_TheaterMode((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); 
}

inline void CHtmlViewEx::SetVisible(BOOL bNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->put_Visible((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); 
}

inline void CHtmlViewEx::SetMenuBar(BOOL bNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->put_MenuBar((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); 
}

inline void CHtmlViewEx::SetToolBar(int nNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_ToolBar(nNewValue); 
}

inline void CHtmlViewEx::SetOffline(BOOL bNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->put_Offline((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE));
	m_bOfflineIfNotConnected = bNewValue;
}

inline void CHtmlViewEx::SetSilent(BOOL bNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->put_Silent((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE));
	m_bSilentMode = bNewValue;
}

inline void CHtmlViewEx::GoBack()
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->GoBack(); 
}

inline void CHtmlViewEx::GoForward()
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->GoForward(); 
}

inline void CHtmlViewEx::GoHome()
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->GoHome(); 
}

inline void CHtmlViewEx::GoSearch()
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->GoSearch(); 
}

inline void CHtmlViewEx::Refresh()
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->Refresh(); 
}

inline void CHtmlViewEx::Refresh2(int nLevel)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->Refresh2(COleVariant((long) nLevel, VT_I4)); 
}

inline void CHtmlViewEx::Stop()
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->Stop(); 
}

inline void CHtmlViewEx::SetFullScreen(BOOL bNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->put_FullScreen((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); 
}

inline void CHtmlViewEx::SetAddressBar(BOOL bNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->put_AddressBar((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); 
}

inline void CHtmlViewEx::SetHeight(long nNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->put_Height(nNewValue); 
}

inline void CHtmlViewEx::PutProperty(LPCTSTR lpszPropertyName, long lValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	ASSERT(m_pBrowserApp != NULL); 
	PutProperty(lpszPropertyName, COleVariant(lValue, VT_UI4)); 
}

inline void CHtmlViewEx::PutProperty(LPCTSTR lpszPropertyName, short nValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	ASSERT(m_pBrowserApp != NULL); 
	PutProperty(lpszPropertyName, COleVariant(nValue, VT_UI2)); 
}

inline void CHtmlViewEx::PutProperty(LPCTSTR lpszPropertyName, LPCTSTR lpszValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	ASSERT(m_pBrowserApp != NULL); 
	PutProperty(lpszPropertyName, COleVariant(lpszValue, VT_BSTR)); 
}

inline void CHtmlViewEx::PutProperty(LPCTSTR lpszPropertyName, double dValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	ASSERT(m_pBrowserApp != NULL); 
	PutProperty(lpszPropertyName, COleVariant(dValue)); 
}

inline void CHtmlViewEx::SetTop(long nNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->put_Top(nNewValue); 
}

inline void CHtmlViewEx::SetLeft(long nNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->put_Left(nNewValue); 
}

inline void CHtmlViewEx::SetStatusBar(BOOL bNewValue)
{ 
	ASSERT(m_pBrowserApp != NULL); 
	m_pBrowserApp->put_StatusBar((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); 
}

inline CString CHtmlViewEx::GetUserAgent() const
{
	return m_strUserAgent;
}

inline void CHtmlViewEx::SetUserAgent(LPCTSTR strNewValue)
{
	ASSERT(strNewValue != NULL);
	m_strUserAgent = strNewValue;
}

inline void CHtmlViewEx::OnViewSource()
{
	ExecCmdTarget(HTMLID_VIEWSOURCE);
}

inline void CHtmlViewEx::OnToolsInternetOptions()
{
	ExecCmdTarget(HTMLID_OPTIONS);
}

inline void CHtmlViewEx::OnEditFindOnThisPage()
{
	ExecCmdTarget(HTMLID_FIND);	
}

inline void CHtmlViewEx::OnEditCut()
{ 
  ExecWB(OLECMDID_CUT, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL); 
} 

inline void CHtmlViewEx::OnEditCopy() 
{ 
  ExecWB(OLECMDID_COPY, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL); 
} 

inline void CHtmlViewEx::OnEditPaste() 
{ 
	ExecWB(OLECMDID_PASTE, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL); 
} 

inline void CHtmlViewEx::OnEditSelectall() 
{ 
	ExecWB(OLECMDID_SELECTALL, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL); 
} 

// #endif // __HTMLVIEWEX_INLINES__