////////////////////////////////////////////////////////////////
// MSDN Magazine -- August 2003
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio .NET on Windows XP. Tab size=3.
//
#pragma once

//////////////////
// This struct defines one entry in the command map that maps text strings to
// command IDs for HTML controls. Don't use it directly; instead, use the
// macros provided to create your command map:
//
//		BEGIN_HTML_COMMAND_MAP(MyHtmlCmds)
//			HTML_COMMAND(_T("about"), ID_APP_ABOUT)
//			HTML_COMMAND(_T("exit"),  ID_APP_EXIT)
//		END_HTML_COMMAND_MAP()
//
// Call CHtmlCtrl::SetCmdMap to use the map, usually just after you create the
// HTML control in your main frame's OnCreate handler.
//
//		// in CMainFrame::OnCreate
//		m_wndView.SetCmdMap(MyHtmlCmds, this);
//
// Now to invoke the About command from HTML in your CHtmlCtrl, you can add
// an HTML link using the "app" pseudo-protocol like so:
//
//		...<A HREF="app:about">About</A>....
//
// Clicking the link will be the same as sending the ID_APP_ABOUT command to
// whatever window you specified in SetCmdMap--in this case, the main frame.
// In general, you can use HREF="app:cmdname" where cmdname is any string
// appearing in your command map. See MultiWin for an example.
//
struct HTMLCMDMAP {
	LPCTSTR name;		// command name used in "app:name" HREF in <A
	UINT nID;
};

// macros for creating HTML command maps
#define BEGIN_HTML_COMMAND_MAP(name) HTMLCMDMAP name[] = {
#define HTML_COMMAND(cmd, id)		{ cmd, id },
#define END_HTML_COMMAND_MAP()	{ NULL, 0 } };

//////////////////
// CHtmlCtrl makes CHtmlView usable as a child window without doc/frame. Use
// it to display HTML in any child window--for example, inside a dialog. Call
// CreateFromStatic to create a CHtmlCtrl control in the same location as a
// static control in your dialog. For example:
//
//		BOOL CAboutDialog::OnInitDialog()
//		{
//			VERIFY(CDialog::OnInitDialog());
//			VERIFY(m_wndHtml.CreateFromStatic(IDC_HTMLVIEW, this));
//			m_wndHtml.LoadFromResource(_T("about.htm"));
//			m_wndHtml.SetCmdMap(MyHtmlCmds);
//			return TRUE;
//		}
//
// Here IDC_HTMLVIEW is the control ID of a static text control and
// "about.htm" is an HTML resource embedded in the resource file like so:
//
//			// in .rc file:
//			ABOUT.HTM HTML DISCARDABLE "res\\about.htm"
//
// If your HTML file has <IMG> tags, the images must also appear as HTML
// resources:
//
//			// in .rc file:
//			PD.JPG    HTML DISCARDABLE "res\\pd.jpg"
//			OKUP.GIF  HTML DISCARDABLE "res\\okup.gif"
//
// You can invoke commands from your HTML page by creating a command map and
// adding an A element with the special pseudo-protocol "app:". See the
// description for HTMLCMDMAP.
//
class CHtmlCtrl : public CHtmlView {
public:
	CHtmlCtrl() : m_bHideMenu(FALSE), m_cmdmap(NULL) { }
	~CHtmlCtrl() { }

	// Get/set HideContextMenu property
	BOOL GetHideContextMenu()			 { return m_bHideMenu; }
	void SetHideContextMenu(BOOL val) { m_bHideMenu=val; }

	// Set doc contents from string.
	HRESULT SetHTML(LPCTSTR strHTML);

	// Set command map.
	void SetCmdMap(HTMLCMDMAP* val, CWnd* pWndCmd = NULL)
	{
		m_cmdmap = val;
		m_pWndCmd = pWndCmd ? pWndCmd : GetParent();
	}

	// Create control in same place as static control.
	BOOL CreateFromStatic(UINT nID, CWnd* pParent);

	// craete control from scratch
	BOOL Create(const RECT& rc, CWnd* pParent, UINT nID,
		DWORD dwStyle = WS_CHILD|WS_VISIBLE, CCreateContext* pContext = NULL)
	{
		return CHtmlView::Create(NULL, NULL, dwStyle, rc, pParent, nID, pContext);
	}

protected:
	HTMLCMDMAP* m_cmdmap;	// command map
	CWnd* m_pWndCmd;			// window to receive commands
	BOOL	m_bHideMenu;		// hide context menu

	// helpers
	UINT GetAppCommandID(LPCTSTR lpszCmd);

	// Override this to handle "app:" commands. Only if you don't use a command map.
	virtual void OnAppCmd(LPCTSTR lpszCmd);

	// virtual overrides

	// Override to trap "app:" pseudo protocol:
	void OnStatusTextChange(LPCTSTR lpszText);
	virtual void OnBeforeNavigate2( LPCTSTR lpszURL,
		DWORD nFlags,
		LPCTSTR lpszTargetFrameName,
		CByteArray& baPostedData,
		LPCTSTR lpszHeaders,
		BOOL* pbCancel );

	// Override to intercept child window messages to disable context menu.
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Normally, CHtmlView destroys itself in PostNcDestroy, but we don't want
	// to do that for a control since a control is usually implemented as a
	// member of another window object.
	//
	virtual void PostNcDestroy() {  }

	// Overrides to bypass MFC doc/view frame dependencies. These are
	// the only places CHtmView depends on livining inside a frame.
	afx_msg void OnDestroy();
	afx_msg int  OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT msg);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP();
	DECLARE_DYNAMIC(CHtmlCtrl)
};
