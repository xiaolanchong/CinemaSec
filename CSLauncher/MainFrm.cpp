// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "CSLauncher.h"

#include <mmsystem.h>
//using std::max;
//using std::min;

#include "MainFrm.h"
//#include "LauncherView.h"
//#include "AnalyzerView.h"
#include "LauncherDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const UINT WM_PING_MESSAGE	= ::RegisterWindowMessage( _T("CSLauncherPing") );
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_NOTIFY( TCN_SELCHANGE, IDC_TAB_ROOM, OnSelectTab )
	ON_REGISTERED_MESSAGE( WM_PING_MESSAGE, OnPingMsg )
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| /*CBRS_GRIPPER |*/ CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	if ( !m_wndMainReBar.Create( this, RBS_AUTOSIZE  ) )
	{
		TRACE0("Failed to create main rebar\n");
		return -1;      // fail to create
	}
	if( !m_wndSelectBar.Create( this, IDD_SELECT_MODE, CBRS_TOP , 1) ||
		!m_wndMainReBar.AddBar( &m_wndSelectBar, (LPCTSTR)0, 0, 0 ) )
	{
		TRACE0("Failed to create select dialog bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
#if 0
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
#endif
	m_LayoutManager.Attach( &m_wndSelectBar );
	m_LayoutManager.AddChild( IDC_TAB_ROOM );
	m_LayoutManager.SetConstraint(IDC_TAB_ROOM, OX_LMS_RIGHT, OX_LMT_SAME, 0);

	m_LayoutManager.RedrawLayout();

	// window propery for CSGuard
	SetProp( GetSafeHwnd(), _T("WindowID"), (HANDLE)459 );
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~FWS_ADDTOTITLE;
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

// CMainFrame message handlers

void CMainFrame::ChangeView(CView* pNewView)
{
	CView *pOldView = GetActiveView();
	if( pOldView == pNewView ) return; 
	SetActiveView( pNewView, TRUE);
	pNewView->SetDlgCtrlID( AFX_IDW_PANE_FIRST );
	pOldView->SetDlgCtrlID( AFX_IDW_PANE_FIRST + 0x7F);
	pNewView->ShowWindow(SW_SHOW);
	pOldView->ShowWindow(SW_HIDE);
	RecalcLayout();
}

CView*		CMainFrame::GetTagView(boost::shared_ptr< SelectionTag > Tag)
{
	CDocument *pDoc = GetActiveDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	int nNumber = 0;
	while(pos)
	{
		CView* pView = pDoc->GetNextView(pos);
		TabSelection* pSelect = dynamic_cast<TabSelection*> (pView);
		if( pSelect )
		{
			const SelectionTag* pTag = pSelect->GetTag();
			if( pTag && (*Tag.get()) == *pTag ) return pView;
		}
	}
	return NULL;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	return CFrameWnd::OnCreateClient( lpcs, pContext );
}

BOOL	CMainFrame::AddTab( boost::shared_ptr< SelectionTag >  TagForTab )
{
	ASSERT ( TagForTab.get() ); 
	CWnd* pWnd = m_wndSelectBar.GetDlgItem( IDC_TAB_ROOM );
	CTabCtrl wndRoomSelector;
	wndRoomSelector.Attach( pWnd->GetSafeHwnd() );

#if 0
	CString sName;
	sName.Format( _T("Room %d"), it->first );
	int nWhereInsert = wndRoomSelector.GetItemCount();
	LONG nIndex = wndRoomSelector.InsertItem( nWhereInsert, sName );
	TCITEM tci;
	tci.mask	= TCIF_PARAM;
	tci.lParam	= it->first;
	wndRoomSelector.SetItem( nIndex, &tci );
#endif
	CString sName = TagForTab->GetTabName();
	int nWhereInsert = wndRoomSelector.GetItemCount();
	LONG nIndex = wndRoomSelector.InsertItem( nWhereInsert, sName );
	m_TagArr.push_back( TagForTab );

	wndRoomSelector.Detach();
	return TRUE;
}

void CMainFrame::OnSelectTab( NMHDR * pHdr, LRESULT * pResult  )
{
	CWnd* pWnd = m_wndSelectBar.GetDlgItem( IDC_TAB_ROOM );
	CTabCtrl wndRoomSelector;
	wndRoomSelector.Attach( pWnd->GetSafeHwnd() );
	int nIndex = wndRoomSelector.GetCurSel();

	ASSERT(nIndex >= 0 && size_t(nIndex) < m_TagArr.size() );
	boost::shared_ptr<SelectionTag> Tag =  m_TagArr [ nIndex ];
	CView * pView = GetTagView( Tag );
	ASSERT(pView);
	ChangeView( pView );	

	wndRoomSelector.Detach();
	*pResult = TRUE;
}

LRESULT	CMainFrame::OnPingMsg(  WPARAM wp, LPARAM )
{
//	Sleep( 10 * 1000 );
	return wp;
}