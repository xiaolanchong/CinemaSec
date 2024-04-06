//#include "..\cslauncher\mainfrm.h"
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "CSClient.h"

#include "MainFrm.h"
#include ".\mainfrm.h"
#include "DBFacet/CfgDB.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

const DWORD Mode_ViewVideo		= 0;
const DWORD Mode_ArchiveMode	= 1;
const DWORD Mode_DiagnosticMode	= 2;
const DWORD Mode_TimeTableMode	= 3;
const DWORD Mode_ReportMode		= 4;
const DWORD Mode_Settings		= 5;
const DWORD Mode_GaugeHall		= 6;
const DWORD Mode_BoxOffice		= 7;
const DWORD Mode_TrainMode		= 8;
const DWORD Mode_LogMode		= 9;
const DWORD Mode_AlwaysLastMode		= Mode_LogMode;

const UINT  ID_VIEW_ALWAYS_LAST_MODE = ID_VIEW_VIDEO + Mode_AlwaysLastMode;

IMPLEMENT_DYNCREATE(CMainFrame, CGuiMDIFrame)

BEGIN_MESSAGE_MAP(CMainFrame, CGuiMDIFrame)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_INITMENUPOPUP()
	ON_WM_SIZE()
	ON_WM_CANCELMODE()
	ON_MESSAGE(WM_ADDCHILD, OnAddChild)
	ON_MESSAGE(WM_MDISETMENU, OnMDISetMenu)

	ON_COMMAND_RANGE( ID_VIEW_VIDEO, ID_VIEW_ALWAYS_LAST_MODE, OnViewMode )
	ON_UPDATE_COMMAND_UI_RANGE( ID_VIEW_VIDEO, ID_VIEW_ALWAYS_LAST_MODE, OnViewModeUpdate )

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

UINT Mode2String[][2] = 
{
	{ ID_VIEW_VIDEO,		IDS_VIEW_VIDEO		},
	{ ID_VIEW_ARCHIVE,		IDS_VIEW_ARCHIVE	},
	{ ID_VIEW_ROOM,			IDS_VIEW_ALGORITHM	},
	{ ID_VIEW_TRAINDATA,	IDS_VIEW_TRAINDATA	},
	{ ID_VIEW_BOXOFFICE,	IDS_VIEW_BOXOFFICE	},
	{ ID_VIEW_REPORT,		IDS_VIEW_REPORT		},
	{ ID_VIEW_DIAGNOSTIC,	IDS_VIEW_DIAGNOSTIC	},
	{ ID_VIEW_TIMETABLE,	IDS_VIEW_TIMETABLE	},
	{ ID_VIEW_SETTINGS,		IDS_VIEW_SETTINGS	},
	{ ID_VIEW_LOG,			IDS_VIEW_LOG	}
};

UINT FindStringForMode( UINT nModeID )
{
	for( size_t i = 0; i < sizeof( Mode2String )/sizeof(Mode2String[0]); ++i )
	{
		if( nModeID == Mode2String[i][0] ) return Mode2String[i][1]; 
	}
	// error probably
	return UINT(~0);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

//******************************************************************
BOOL CMDIClientHook::Install( HWND hWndToHook)
{
	return HookWindow(hWndToHook);
}
//******************************************************************
LRESULT CMDIClientHook::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{


	switch (nMsg) {
	case WM_MDISETMENU:			// only sent to MDI client window
		// Setting new frame/window menu: bypass MDI. wParam is new menu.
//		ASSERT(FALSE);
		return 0;

	case WM_MDIREFRESHMENU:		// only sent to MDI client window
		// Normally, would call DrawMenuBar, but I have the menu, so eat it.
		//LTRACE(_T("CMenuBar::WM_MDIREFRESHMENU\n"));
		return 0;

	}

	return CSubclassWnd::WindowProc(nMsg, wParam, lParam);
}

///////////////////
struct RightsChecker
{
	virtual void	Check(  CAlphaToolBar& wndToolBar ) = 0;
};

struct TrainChecker
{
	virtual void	Check(  CAlphaToolBar& wndToolBar )
	{
		if( !GetCfgDB().IsDeveloper() )
		{
			int nTrainIndex = wndToolBar.CommandToIndex( ID_VIEW_TRAINDATA );
			// must be!!!
			ASSERT( nTrainIndex >= 0 );
			wndToolBar.GetToolBarCtrl().DeleteButton( nTrainIndex );
		}
	}
};

struct DeveloperChecker
{
	CMainFrame* m_pFrame;

	DeveloperChecker( CMainFrame* pFrame ) : m_pFrame( pFrame ){}
	virtual void	Check(  CAlphaToolBar& wndToolBar )
	{
		if( GetCfgDB().IsInstall() || GetCfgDB().IsDeveloper() )
		{
			m_pFrame->ChangeActiveView( Mode_GaugeHall );
		}
		else
		{
			int nRoomIndex = wndToolBar.CommandToIndex( ID_VIEW_ROOM );
			// must be!!!
			ASSERT( nRoomIndex >= 0 );
			wndToolBar.GetToolBarCtrl().DeleteButton( nRoomIndex );

			m_pFrame->ChangeActiveView( Mode_ViewVideo );
		}
	}
};

struct SettingsChecker
{
	virtual void	Check(  CAlphaToolBar& wndToolBar )
	{
		if( !GetCfgDB().IsUserAnAdmin() && !GetCfgDB().IsDeveloper() )
		{
			int nSettingsIndex = wndToolBar.CommandToIndex( ID_VIEW_SETTINGS );
			// must be!!!
			ASSERT( nSettingsIndex >= 0 );
			wndToolBar.GetToolBarCtrl().DeleteButton( nSettingsIndex );
		}
	}
};

struct ExchangeChecker
{
	virtual void	Check( CAlphaToolBar& wndToolBar )
	{
		if( !GetCfgDB().UseDB() )
		{
			int nExchangeIndex = wndToolBar.CommandToIndex( ID_VIEW_BOXOFFICE );
			ASSERT( nExchangeIndex >= 0 );
			wndToolBar.GetToolBarCtrl().DeleteButton( nExchangeIndex );
		}
	}
};

struct LogChecker
{
	virtual void	Check( CAlphaToolBar& wndToolBar )
	{
		//// temporaly delete Exchange
		if( !GetCfgDB().UseLogFile() )
		{
			int nLogIndex = wndToolBar.CommandToIndex( ID_VIEW_LOG );
			ASSERT( nLogIndex >= 0 );
			wndToolBar.GetToolBarCtrl().DeleteButton( nLogIndex );
		}
	}
};
///////////////////

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitToolBar( IDR_TOOLBAR1 );
	InitStatusBar( indicators, sizeof(indicators)/sizeof(UINT) );

	OpenNewMode( _T("Video Viewer") );
	
	for ( int i = 0; i <= Mode_AlwaysLastMode ; i++ )
	{
		UINT nID, nStyle;
		int nImage;
		m_wndToolBar.GetButtonInfo( i, nID, nStyle, nImage );
		if ( !( nStyle & TBSTYLE_SEP ) )
		{
			nStyle |= BTNS_AUTOSIZE;
			m_wndToolBar.SetButtonInfo( i, nID, nStyle, nImage);
			CString sText;
			sText.LoadString( FindStringForMode(nID) );
			m_wndToolBar.SetButtonText( i, sText );
		}
	}
/*
	if( !GetCfgDB().IsDeveloper() )
	{
		int nTrainIndex = m_wndToolBar.CommandToIndex( ID_VIEW_TRAINDATA );
		// must be!!!
		ASSERT( nTrainIndex >= 0 );
		m_wndToolBar.GetToolBarCtrl().DeleteButton( nTrainIndex );
	}

	if( GetCfgDB().IsInstall() || GetCfgDB().IsDeveloper() )
	{
		ChangeActiveView( Mode_GaugeHall );
	}
	else
	{
		int nRoomIndex = m_wndToolBar.CommandToIndex( ID_VIEW_ROOM );
		// must be!!!
		ASSERT( nRoomIndex >= 0 );
		m_wndToolBar.GetToolBarCtrl().DeleteButton( nRoomIndex );

		ChangeActiveView( Mode_ViewVideo );
	}

	if( !GetCfgDB().IsUserAnAdmin() && !GetCfgDB().IsDeveloper() )
	{
		int nSettingsIndex = m_wndToolBar.CommandToIndex( ID_VIEW_SETTINGS );
		// must be!!!
		ASSERT( nSettingsIndex >= 0 );
		m_wndToolBar.GetToolBarCtrl().DeleteButton( nSettingsIndex );
	}
	if( !GetCfgDB() )

//// temporaly delete Exchange
	{
	int nExchangeIndex = m_wndToolBar.CommandToIndex( ID_VIEW_BOXOFFICE );
	ASSERT( nExchangeIndex >= 0 );
	m_wndToolBar.GetToolBarCtrl().DeleteButton( nExchangeIndex );
	}
///
*/
	TrainChecker().Check	( m_wndToolBar);
	DeveloperChecker(this).Check( m_wndToolBar);
	SettingsChecker().Check	( m_wndToolBar);
	ExchangeChecker().Check	( m_wndToolBar);
	LogChecker().Check		( m_wndToolBar);

	RecalcLayout();
	SetMenu(NULL);
	m_MDIClientHook.Install( m_hWndMDIClient );
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	if(cs.hMenu!=NULL)
	{
		::DestroyMenu(cs.hMenu);      // delete menu if loaded
		cs.hMenu = NULL;              // no menu for this window
	}

	cs.dwExStyle &=  ~( WS_EX_CLIENTEDGE);
	cs.style	 &=  ~( FWS_ADDTOTITLE );
	return CMDIFrameWnd::PreCreateWindow(cs);;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu )
{
#ifdef USE_GUILIB
	UpdateWindow();
#ifdef USE_MENUBAR
	m_wndHookMenu.UpdateWindow();
#endif
	CNewMenu::OnInitMenuPopup(m_hWnd,pPopupMenu, nIndex, bSysMenu);
#endif
    CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
    
}
/*
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	CGuiMDIFrame::OnCreateClient(lpcs, pContext);

	return TRUE;
}*/

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CGuiMDIFrame::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
}

void CMainFrame::OnViewMode( UINT nID )
{
	int nIndex = nID - ID_VIEW_VIDEO;

	for( int i= ID_VIEW_VIDEO; i <= ID_VIEW_VIDEO + Mode_AlwaysLastMode ; ++i )
	{
		m_wndToolBar.GetToolBarCtrl().CheckButton( i, FALSE );
	}
	if( m_ChildFrames.GetSize() > nIndex )
	{
		ChangeActiveView( nIndex );
		m_wndToolBar.GetToolBarCtrl().CheckButton( nID, TRUE );
	}
}

void CMainFrame::OnViewModeUpdate( CCmdUI* pCmdUI )
{
	int nIndex = pCmdUI->m_nID - ID_VIEW_VIDEO;
	CMDIChildWnd* pWnd = MDIGetActive(  );

	if( Mode_AlwaysLastMode < nIndex )
		pCmdUI->Enable(FALSE);
	else
	{
		if( m_ChildFrames.GetSize() > nIndex )
			pCmdUI->SetCheck( (pWnd->GetSafeHwnd() == m_ChildFrames[nIndex]) ? 1 : 0);
	}
}

void	CMainFrame::ChangeActiveView( UINT nViewID )
{

	CWnd* pWnd = CWnd::FromHandle( m_ChildFrames[nViewID] );
	MDIActivate(pWnd);
	FireChangeTab( pWnd->GetSafeHwnd() );
}

//-------------------------------------------------------------------------------------------------
/** \brief Function opens and initializes a new working mode.

  \param  modeTemplateName  the name of the document template of desired mode. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::OpenNewMode( LPCTSTR modeTemplateName )
{
	UNREFERENCED_PARAMETER(modeTemplateName);
  for (POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition(); pos != 0;)
  {
    CDocTemplate * pTempl = AfxGetApp()->GetNextDocTemplate( pos );

    if ((pTempl != 0) )
    {
        pTempl->OpenDocumentFile( 0 );
    }
  }
}

LRESULT CMainFrame::OnAddChild( WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam);
	m_ChildFrames.Add( (HWND)wParam );
	return TRUE;
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave it alone!

	CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle && pDocument != NULL)
		UpdateFrameTitleForDocument(pDocument->GetTitle());
	else
		UpdateFrameTitleForDocument(NULL);
}

void CMainFrame::OnCancelMode()
{
	CWnd* pWnd = GetActiveFrame();
	// NOTE : can return this pointer - no active child wnds
	if(pWnd && pWnd != this) pWnd->SendMessage( WM_CANCELMODE );
}

BOOL CMainFrame::InitToolBar(UINT nResID)
{
//	m_wndToolBar.Create( this, AFX_IDW_TOOLBAR );
	m_wndToolBar.CreateEx( this, TBSTYLE_FLAT|TBSTYLE_LIST, WS_CHILD | WS_VISIBLE | CBRS_TOP |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED);
	return m_wndToolBar.LoadToolBar( nResID, AILS_NEW );
}

BOOL CMainFrame::InitStatusBar( UINT * pIndicators, int nSize )
{
	m_wndStatusBar.Create( this );
	return m_wndStatusBar.SetIndicators( pIndicators, nSize );
}

// Overridden method declarations for CMainFrame

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
						   CWnd* pParentWnd, CCreateContext* pContext)
{
	return CFrameWnd::LoadFrame(nIDResource,dwDefaultStyle,
		pParentWnd,pContext);
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs,
								CCreateContext* /*pContext*/)
{
	return CreateClient(lpcs,NULL);
}

void	CMainFrame::RegisterChangeTab( IChangeTabCallback* pInt )
{
	std::vector<IChangeTabCallback*>::iterator it = std::find( m_CallbackArr.begin(), m_CallbackArr.end(), pInt );
	if( it == m_CallbackArr.end() )
	{
		m_CallbackArr.push_back( pInt );
	}
}

void	CMainFrame::UnregisterChangeTab( IChangeTabCallback* pInt )
{
	std::vector<IChangeTabCallback*>::iterator it = std::find( m_CallbackArr.begin(), m_CallbackArr.end(), pInt );
	if( it != m_CallbackArr.end() )
	{
		m_CallbackArr.erase( it );
	}
}

void	CMainFrame::FireChangeTab( HWND hWnd )
{
	for( size_t i = 0; i < m_CallbackArr.size(); ++i )
	{
		m_CallbackArr[i]->OnActivateTab( hWnd );
	}
}