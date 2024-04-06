// Archive_Mode\VideoControlView.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "Archive_Mode\VideoControlView.h"
#include "archive_mode_doc.h"
#include <afxpriv.h>

// CVideoControlView

const UINT Timer_VideoControl		= 0x2e;
const UINT Period_VideoControl		= 200;

IMPLEMENT_DYNCREATE(CVideoControlView, CFormView)

CVideoControlView::CVideoControlView()
	: CFormView(CVideoControlView::IDD),
	m_bInitialized(false),
	m_State		( CVideoControlView::st_disable		),
	m_RangeState( CVideoControlView::rst_disable	)
{
}

CVideoControlView::~CVideoControlView()
{
}

void CVideoControlView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVideoControlView, CFormView)
	ON_WM_TIMER()
	ON_MESSAGE( wm_slider, OnProcessSlider )
	ON_COMMAND			( ID_VIDEO_PLAY, OnPlay		)
	ON_UPDATE_COMMAND_UI( ID_VIDEO_PLAY, OnPlayUpdate	)
	ON_COMMAND			( ID_VIDEO_PAUSE, OnPause		)
	ON_UPDATE_COMMAND_UI( ID_VIDEO_PAUSE, OnPauseUpdate	)
	ON_COMMAND			( ID_VIDEO_STOP, OnStop		)
	ON_UPDATE_COMMAND_UI( ID_VIDEO_STOP, OnStopUpdate	)

	ON_COMMAND			( ID_VIDEO_SAVE, OnSaveRange		)
	ON_UPDATE_COMMAND_UI( ID_VIDEO_SAVE, OnSaveRangeUpdate	)
	ON_COMMAND			( ID_VIDEO_START, OnStartRange		)
	ON_UPDATE_COMMAND_UI( ID_VIDEO_START, OnStartRangeUpdate	)
	ON_COMMAND			( ID_VIDEO_END, OnEndRange		)
	ON_UPDATE_COMMAND_UI( ID_VIDEO_END, OnEndRangeUpdate	)
END_MESSAGE_MAP()


// CVideoControlView diagnostics

#ifdef _DEBUG
void CVideoControlView::AssertValid() const
{
	CFormView::AssertValid();
}

void CVideoControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CArchiveDoc * CVideoControlView::GetDocument() // debug version
{
	ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CArchiveDoc ) ) );
	return (CArchiveDoc*)m_pDocument;
}
#endif //_DEBUG

BOOL	CVideoControlView::Create( CWnd* pParent, const CRect & rc, UINT nID , CCreateContext& cc )
{
	return CFormView::Create( NULL,NULL,WS_CHILD | WS_VISIBLE | WS_BORDER, rc, pParent, nID, &cc );
}

void CVideoControlView::OnInitialUpdate( )
{
	CFormView::OnInitialUpdate();
	if(!m_bInitialized )
	{
		ModifyStyleEx( WS_EX_CLIENTEDGE, 0);
		ModifyStyle( 0, WS_CLIPCHILDREN );
		
		m_bInitialized = true;

		CRect rcClient;
		CRect rcRange;
//		GetClientRect(&rcClient);

		CWnd* pWnd;
	
		m_wndRangeSelect.SubclassDlgItem( IDC_STATIC_RANGE, this );
		//m_wndRangeSelect.ModifyStyleEx( 0, WS_EX_CLIENTEDGE );
		m_wndRangeSelect.ModifyStyle( 0, WS_BORDER );
		pWnd = GetDlgItem( IDC_STATIC_SLIDER );
		ASSERT(pWnd);
		pWnd->GetWindowRect(&rcClient);
		ScreenToClient(&rcClient);
		pWnd->DestroyWindow();
		
		HCURSOR h = ::LoadCursor( 0, IDC_HAND );
		CString s = ::AfxRegisterWndClass(0, h);
		m_wndSlider.Create( s, 0, WS_CHILD|WS_VISIBLE|TBS_NOTICKS, rcClient, this, IDC_STATIC_SLIDER );
		m_wndSlider.SetReceiver( this );
		//m_wndRange.EnableWindow(FALSE);
		// Positioning of toolbar
		CSize sizeToolbar;
		CRect mrect;
		mrect.SetRectEmpty();
		// attach command routing to dialog window
		m_wndVideoControlBar.Create(this, AFX_IDW_TOOLBAR); 
		m_wndVideoControlBar.LoadToolBar(IDR_SEEK_BAR, AILS_NEW);
		m_wndVideoControlBar.SetBarStyle(CBRS_ALIGN_TOP | CBRS_TOOLTIPS);
		m_wndVideoControlBar.ShowWindow(SW_SHOW);
		// Calculate size of toolbar and adjust size of static control to fit size
		sizeToolbar = m_wndVideoControlBar.CalcFixedLayout(false,true);

		CWnd & m_Stc_ToolbarFrame = *GetDlgItem( IDC_STATIC_CONTROL );

		WINDOWPLACEMENT wpl;
		m_Stc_ToolbarFrame.GetWindowPlacement(&wpl);
		wpl.rcNormalPosition.bottom = wpl.rcNormalPosition.top  + 
			sizeToolbar.cy + 4;
		wpl.rcNormalPosition.right  = wpl.rcNormalPosition.left + 
			sizeToolbar.cx + 4;
		// Position static control and toolbar
		m_Stc_ToolbarFrame.SetWindowPlacement(&wpl);
		m_wndVideoControlBar.SetWindowPlacement(&wpl);
		// Adjust buttons into static control
		m_Stc_ToolbarFrame.RepositionBars(AFX_IDW_CONTROLBAR_FIRST, 
			AFX_IDW_CONTROLBAR_LAST, 0);
		m_Stc_ToolbarFrame.ShowWindow(SW_HIDE);
		m_Stc_ToolbarFrame.DestroyWindow();

		m_wndVideoControlBar.SetDlgCtrlID( IDC_STATIC_CONTROL );
///////	
	
			// attach command routing to dialog window
		m_wndVideoRangeBar.Create(this, AFX_IDW_TOOLBAR); 
		m_wndVideoRangeBar.LoadToolBar(IDR_VIDEO_RANGE, AILS_NEW);
		m_wndVideoRangeBar.SetBarStyle(CBRS_ALIGN_TOP | CBRS_TOOLTIPS);
		m_wndVideoRangeBar.ShowWindow(SW_SHOW);
		// Calculate size of toolbar and adjust size of static control to fit size
		sizeToolbar = m_wndVideoRangeBar.CalcFixedLayout(false,true);

		CWnd & m_Stc_ToolbarFrame2 = *GetDlgItem( IDC_STATIC_VRANGE );

		//WINDOWPLACEMENT wpl;
		m_Stc_ToolbarFrame2.GetWindowPlacement(&wpl);
		wpl.rcNormalPosition.bottom = wpl.rcNormalPosition.top  + 
			sizeToolbar.cy + 4;
		wpl.rcNormalPosition.right  = wpl.rcNormalPosition.left + 
			sizeToolbar.cx + 4;
		// Position static control and toolbar
		m_Stc_ToolbarFrame2.SetWindowPlacement(&wpl);
		m_wndVideoRangeBar.SetWindowPlacement(&wpl);
		// Adjust buttons into static control
		m_Stc_ToolbarFrame2.RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
		m_Stc_ToolbarFrame2.ShowWindow(SW_HIDE);
		m_Stc_ToolbarFrame2.DestroyWindow();

		m_wndVideoRangeBar.SetDlgCtrlID( IDC_STATIC_VRANGE );

////////
		m_LayoutManager.Attach(this);
		m_LayoutManager.AddAllChildren();
	
#if 0 
		int nGap = rcRange.Height() * 3/4;
		m_LayoutManager.SetConstraint(&m_wndRange,	OX_LMS_LEFT, OX_LMT_OPPOSITE, 0, &m_wndVideoControlBar);
		m_LayoutManager.SetConstraint(&m_wndRange,	OX_LMS_RIGHT, OX_LMT_SAME, 0);
		m_LayoutManager.SetConstraint(&m_wndSlider,	OX_LMS_RIGHT, OX_LMT_SAME, - nGap, &m_wndRange);
		m_LayoutManager.SetConstraint(&m_wndSlider,	OX_LMS_LEFT, OX_LMT_SAME, nGap, &m_wndRange);
#else
		m_LayoutManager.SetConstraint(&m_wndSlider,	OX_LMS_RIGHT, OX_LMT_SAME, - 5 );
		m_LayoutManager.SetConstraint(&m_wndSlider,	OX_LMS_LEFT, OX_LMT_OPPOSITE, 5, &m_wndVideoControlBar);
		m_LayoutManager.SetConstraint(&m_wndRangeSelect,	OX_LMS_RIGHT, OX_LMT_SAME, 0, &m_wndSlider );
		m_LayoutManager.SetConstraint(&m_wndRangeSelect,	OX_LMS_LEFT, OX_LMT_SAME, 0, &m_wndSlider);
#endif
		m_LayoutManager.RedrawLayout();

		m_CtrlHelper.Attach( this, GetParentFrame() );
	}

//	SetScrollSizes( MM_TEXT, CSize(1, 1) );
}
// CVideoControlView message handlers


void	CVideoControlView::OnPlay()
{
	m_State = st_play;
	CArchiveDoc* pDoc = GetDocument();
	pDoc->PlayAfterPause();
	m_wndSlider.EnableWindow(TRUE);
}

void	CVideoControlView::OnPlayUpdate(CCmdUI* pCmdUI)
{
	switch( m_State )
	{
		case st_disable:		
			pCmdUI->Enable(FALSE);
			pCmdUI->SetCheck(0);
			break;
		case st_stop:
		case st_pause:
			pCmdUI->Enable(TRUE);
			pCmdUI->SetCheck(0);
			break;
		case st_play:
			pCmdUI->Enable(TRUE);
			pCmdUI->SetCheck(1);
			break;
	}
}

void	CVideoControlView::OnPause()
{
	m_State = st_pause;
	CArchiveDoc* pDoc = GetDocument();
	pDoc->PauseVideo();
}

void	CVideoControlView::OnPauseUpdate(CCmdUI* pCmdUI)
{
	switch( m_State )
	{
	case st_disable:
	case st_stop:
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(0);
		break;
	case st_play:
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(0);
		break;
	case st_pause:
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(1);
		break;
	}
}

void	CVideoControlView::OnStop()
{
	m_nCurrentMS	= 0;
	m_State			= st_stop;
	CArchiveDoc* pDoc = GetDocument();
	pDoc->StopVideo();
	m_wndSlider.EnableWindow(FALSE);
}

void	CVideoControlView::OnStopUpdate(CCmdUI* pCmdUI)
{
	switch( m_State )
	{
	case st_disable:		
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(0);
		break;
	case st_stop:
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(1);
		break;
	case st_pause:
	case st_play:
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(0);
		break;
	}
}

void	CVideoControlView::EnableVideo( CTime timeBegin, CTime timeEnd )
{
	ASSERT( timeBegin < timeEnd );
	CTimeSpan span = timeEnd - timeBegin;
	m_nCurrentMS = 0;
	__int64 nSecNumber = span.GetTotalSeconds() * 1000; //ms
	m_wndSlider.SetCurrent(0);
	m_wndSlider.SetMax( nSecNumber );
	EnableWindow(TRUE);
	m_wndSlider.EnableWindow(FALSE);

	SetTimer( Timer_VideoControl, Period_VideoControl, 0 );
	m_dwLastTick = GetTickCount();
	m_timeBegin = timeBegin;

	m_State = st_stop;
	m_RangeState = rst_idle;
	m_wndRangeSelect.SetMinMax( timeBegin, timeEnd );
}

void	CVideoControlView::DisableVideo( )
{
	m_State = st_disable;
	EnableWindow(FALSE);
	m_wndSlider.EnableWindow(FALSE);

	m_wndSlider.SetCurrent(0);
	KillTimer(Timer_VideoControl);

	m_RangeState = rst_disable;
	m_wndRangeSelect.Reset();
}

void	CVideoControlView::SeekVideoInternal( CTime timeSeek, bool bCallDoc )
{
	CArchiveDoc* pDoc = GetDocument();
	CTimeSpan span = timeSeek - m_timeBegin;
	__int64 nSecNumber = span.GetTotalSeconds() * 1000; //ms
	m_wndSlider.SetCurrent(nSecNumber);

	if( bCallDoc) pDoc->SeekVideo( timeSeek );

	m_nCurrentMS = nSecNumber;
	m_dwLastTick = GetTickCount();

	if( m_RangeState == rst_first )
		m_wndRangeSelect.SetCurrent( GetCurTime() );
}

void	CVideoControlView::SeekVideo( CTime timeSeek )
{
	SeekVideoInternal( timeSeek, true );
}

void	CVideoControlView::SetExternalTime( CTime timeFrame)
{
	SeekVideoInternal( timeFrame, false );
}

void	CVideoControlView::SeekForPhoto( CTime timeSeek )
{
	SeekVideoInternal( timeSeek, false );
	m_State = st_pause;
}

void	CVideoControlView::OnTimer( UINT_PTR nIDEvent )
{
	if( nIDEvent == Timer_VideoControl && m_State == st_play)
	{
		CArchiveDoc* pDoc = GetDocument();
#if 0
		DWORD dwCur = GetTickCount();
		m_nCurrentMS += dwCur - m_dwLastTick;
		m_dwLastTick = dwCur;
		m_wndSlider.SetCurrent( m_nCurrentMS );
		CTimeSpan span( m_nCurrentMS/1000 ) ;
		pDoc->StepVideo( m_timeBegin + span );
#else
		pDoc->StepVideo();
#endif
	}
	CFormView::OnTimer( nIDEvent );
}

LRESULT	CVideoControlView::OnProcessSlider( WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );

	CArchiveDoc* pDoc = GetDocument();
	if( wParam == sm_end_drag )
	{
		pDoc->StopDrag();
		m_nCurrentMS = m_wndSlider.GetCurrent();
		m_dwLastTick = GetTickCount();

		if( m_State == st_pause || 
			m_State == st_play )
		{
			
			pDoc->SeekVideo( GetCurTime() );
		}

		if( m_RangeState == rst_first )
			m_wndRangeSelect.SetCurrent( GetCurTime() );
	}
	else if( wParam == sm_start_drag )
	{
		pDoc->StartDrag();
	}
	return 0;
}

void	CVideoControlView::OnSaveRange()
{

}

void	CVideoControlView::OnSaveRangeUpdate(CCmdUI* pCmdUI)
{
	switch( m_RangeState )
	{
	case rst_disable:	
	case rst_first:
	case rst_idle:
		pCmdUI->Enable(FALSE);
		break;
	case rst_selected:
		pCmdUI->Enable(TRUE);
		break;
	}
}

void	CVideoControlView::OnStartRange()
{
	
	m_RangeState = rst_first;
	m_wndRangeSelect.SetStart( GetCurTime() );
}

void	CVideoControlView::OnStartRangeUpdate(CCmdUI* pCmdUI)
{
	switch( m_RangeState )
	{
	case rst_disable:		
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(0);
		break;
	case rst_first:
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(1);
		break;
	case rst_idle:
	case rst_selected:
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(0);
		break;
	}
}

void	CVideoControlView::OnEndRange()
{
	m_RangeState = rst_selected;
	m_wndRangeSelect.SetEnd( GetCurTime() );
}

void	CVideoControlView::OnEndRangeUpdate(CCmdUI* pCmdUI)
{
	switch( m_RangeState )
	{
	case rst_disable:
	case rst_idle:
	case rst_selected:
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(0);
		break;
	case rst_first:
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(0);
		break;
	}
}