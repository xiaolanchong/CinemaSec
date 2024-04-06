// VIDEO_MODE\VideoStatView.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "VideoDocument.h"
#include "VideoStatView.h"
#include "../DBFacet/DBSerializer.h"
#include "../DBFacet/CfgDB.h"
#include "../../CSEngine/DBFacet/CinemaOleDB.h"
#include "../CommonView/wv_message.h"
#include <AFXPRIV.H>

// CVideoStatView

IMPLEMENT_DYNCREATE(CVideoStatView, CArrangeStatView)

CVideoStatView::CVideoStatView():
	CArrangeStatView( IDD_VIDEO_STAT )
{
//	GetStatisticsDB().Register( this );
}

CVideoStatView::~CVideoStatView()
{
//	GetStatisticsDB().Unregister( this );
}

UINT	Period_Database = 1000;
UINT	Timer_Database	= 0x1e43;


BEGIN_MESSAGE_MAP(CVideoStatView, CArrangeStatView)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_COMMAND( ID_BUTTON_PLAY, OnButtonPlay )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_PLAY, OnUpdateButtonPlay  )
	ON_MESSAGE( WM_IDLEUPDATECMDUI, OnUpdateUI )
END_MESSAGE_MAP()

// CVideoStatView drawing

CVideoDocument* CVideoStatView::GetDocument()
{
	return dynamic_cast<CVideoDocument*>( m_pDocument );
}

void CVideoStatView::OnInitialUpdate()
{
	static bNotInit = true;
	CArrangeStatView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 1;
	SetScrollSizes(MM_TEXT, sizeTotal);

	if( bNotInit )
	{
		bNotInit = false;

		//m_wndStatistics.ModifyStyle( 0, TVS_HASBUTTONS | TVS_HASLINES|TVS_LINESATROOT);
		m_wndStatistics.ModifyExStyle( 0, TVOXS_ROWSEL/*|TVOXS_NOSORTHEADER|TVOXS_HGRID*/ );

		CString sNumber, sTime;
		sNumber.LoadString( IDS_TITLE_NUMBER );
		sTime.LoadString( IDS_TITLE_TIME );
		LV_COLUMN lvc;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx	= 75;
		lvc.pszText = (LPTSTR)(LPCTSTR)sTime;
		lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		m_wndStatistics.SetColumn( Col_Time, &lvc );

		m_wndStatistics.InsertColumn( Col_Number, sNumber, LVCFMT_LEFT, 20 );
		lvc.cx	= 50;
		lvc.mask = LVCF_WIDTH;
		m_wndStatistics.SetColumn( Col_Number, &lvc );

		m_LayoutManager.Attach(this);

		m_LayoutManager.SetDefaultConstraint(IDC_TAB_CINEMA);
		m_LayoutManager.SetDefaultConstraint(IDC_TAB_ROOM);
		m_LayoutManager.SetDefaultConstraint(IDC_LIST_STAT);
		m_LayoutManager.SetDefaultConstraint(IDC_EDIT_CURRENT_FILM);

		m_LayoutManager.SetConstraint(IDC_TAB_CINEMA, OX_LMS_RIGHT, OX_LMT_SAME, -3);
		m_LayoutManager.SetConstraint(IDC_TAB_ROOM, OX_LMS_RIGHT, OX_LMT_SAME, -3);

		m_LayoutManager.SetConstraint(IDC_LIST_STAT, OX_LMS_RIGHT, OX_LMT_SAME, -3);
		m_LayoutManager.SetConstraint(IDC_LIST_STAT, OX_LMS_BOTTOM, OX_LMT_SAME, -3);

		CSize sizeToolbar;
		CRect mrect;
		mrect.SetRectEmpty();
		// attach command routing to dialog window
		m_wndVideoControlBar.Create(this, AFX_IDW_TOOLBAR); 
		m_wndVideoControlBar.LoadToolBar(IDR_TOOLBAR2, AILS_NEW);
		m_wndVideoControlBar.SetBarStyle(CBRS_ALIGN_TOP | CBRS_TOOLTIPS);
		m_wndVideoControlBar.ShowWindow(SW_SHOW);
		// Calculate size of toolbar and adjust size of static control to fit size
		sizeToolbar = m_wndVideoControlBar.CalcFixedLayout(false,true);
		CWnd & m_Stc_ToolbarFrame = *GetDlgItem( IDC_STATIC_TB );

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

		m_Stc_ToolbarFrame.ShowWindow(SW_HIDE);
		m_Stc_ToolbarFrame.DestroyWindow();

		m_wndVideoControlBar.SetDlgCtrlID( IDC_STATIC_TB );

		m_LayoutManager.SetConstraint(IDC_EDIT_CURRENT_FILM, OX_LMS_RIGHT, OX_LMT_OPPOSITE, -2, IDC_STATIC_TB);
		m_LayoutManager.SetConstraint(IDC_STATIC_TB, OX_LMS_RIGHT, OX_LMT_SAME, -2);
		m_LayoutManager.RedrawLayout();

		SetTimer( Timer_Database, Period_Database, 0 );
		m_edCurrentFilm.SubclassDlgItem( IDC_EDIT_CURRENT_FILM, this );

		Init(IDC_TAB_CINEMA, IDC_TAB_ROOM, this, this);

	}
}

void CVideoStatView::OnDraw(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
	//CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CVideoStatView diagnostics

#ifdef _DEBUG
void CVideoStatView::AssertValid() const
{
	CArrangeStatView::AssertValid();
}

void CVideoStatView::Dump(CDumpContext& dc) const
{
	CArrangeStatView::Dump(dc);
}
#endif //_DEBUG


// CVideoStatView message handlers

int CVideoStatView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CArrangeStatView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	return 0;
}

BOOL CVideoStatView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
//	cs.style &=  ~(WS_BORDER);
//	cs.dwExStyle &=  ~(WS_EX_CLIENTEDGE);
	return CArrangeStatView::PreCreateWindow(cs);
}

void	CVideoStatView::OnRoomChange( int nRoomID )
{
	CArrangeStatView::OnRoomChange( nRoomID );
	UpdateFilm() ;
	m_bVideoMode = true;
}

void	CVideoStatView::OnStatisticsUpdate()
{
}

void CVideoStatView::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == Timer_Database )
	{

		UpdateFilm();
	}
	CArrangeStatView::OnTimer( nIDEvent );
}

void CVideoStatView::UpdateFilm() 
try
{
	int nRoomID = GetCurRoom();
	CCinemaOleDB&	db = dynamic_cast< CCinemaOleDB& >( GetDB() );
	FilmTime ft; 
	std::wstring Name;
	bool res = db.GetTableTimetableFacet().GetCurrentFilmTime( nRoomID, ft, Name );
	if( res )
	{
		if( m_edCurrentFilm.GetSafeHwnd() )
		{
		m_edCurrentFilm.SetBackColor( GetSysColor( COLOR_WINDOW ) );
		CString sFilmName;
		m_edCurrentFilm.GetWindowText(sFilmName);
		if(sFilmName != Name.c_str() )
			m_edCurrentFilm.SetWindowText( Name.c_str() );
		}
	}
	else
	{
		if( m_edCurrentFilm.GetSafeHwnd() )
		{
		m_edCurrentFilm.SetBackColor( GetSysColor( COLOR_BTNFACE ) );
		m_edCurrentFilm.SetWindowText( _T("") );
		}
	}
	if(m_wndStatistics.GetSafeHwnd())
		UpdateTree(ft.m_timeBegin, ft.m_timeEnd);
}
catch( std::bad_cast )
{

}
catch( DataBaseException )
{

}

void		CVideoStatView::UpdateTree(CTime timeBegin, CTime timeEnd)
try
{

		CCinemaOleDB&	db = dynamic_cast< CCinemaOleDB& >( GetDB() );

		StatRowSet_t Stat;
		int nRoomID = GetCurRoom();
		db.GetTableStatFacet().GetStatForPeriod( timeBegin, timeEnd, nRoomID, Stat );

		StatRowSet_t::const_iterator it = Stat.begin();
#if 0
		std::vector<std::wstring >	FilmNames;
		TimeTable_t	FilmTimeTable;
		db.GetTableTimetableFacet().GetTimetable(
			nRoomID, 
			FilmTime(-1, timeBegin, timeEnd), 
			FilmTimeTable, FilmNames ) ;
#endif
		// what items we have to delete
		// if an item is not deleted, insert it in the map
		HTREEITEM hti = m_wndStatistics.GetChildItem( m_wndStatistics.GetRootItem() );
		std::set<int> HaveIndices;
		while( hti  )
		{
			int nID = (int)m_wndStatistics.GetItemData( hti );
			if( it->m_nRoomID != GetCurRoom() || 
				Stat.find( StatRow(nID) ) == Stat.end() )
			{
				HTREEITEM htiDel = hti;
				hti = m_wndStatistics.GetNextSiblingItem( htiDel );
				m_wndStatistics.DeleteItem(htiDel);
			}
			else
			{
				HaveIndices.insert( nID );
				hti = m_wndStatistics.GetNextSiblingItem( hti );
			}
		}

		// create map time->item
		std::map< CTime, HTREEITEM >	TimeTreeItemMap;
		hti = m_wndStatistics.GetChildItem( m_wndStatistics.GetRootItem());
		for( ; hti ; hti = m_wndStatistics.GetNextSiblingItem(hti) )
		{
			int nID = (int)m_wndStatistics.GetItemData( hti );
			StatRowSet_t::const_iterator it = Stat.find( StatRow(nID) );
			ASSERT( it != Stat.end() );
			TimeTreeItemMap.insert( std::make_pair(  it->m_Time , hti ));
		}

		// define where we have to insert an item
		for(; it != Stat.end(); ++it)
		{
			if(  HaveIndices.find(it->m_nID) == HaveIndices.end()  && it->m_nRoomID == GetCurRoom() )
			{
			//	CString sDate = it->m_Time.Format( _T("%d %b %y") );
				CString sTime = it->m_Time.Format( _T("%H:%M") );
				CString sSpect;
				sSpect.Format(_T("%d"), it->m_nSpectators);

				std::map< CTime, HTREEITEM >::const_iterator itMap = 
					TimeTreeItemMap.lower_bound( it->m_Time );
				HTREEITEM hti = 0;
				if( itMap == TimeTreeItemMap.end() )
				{
					hti = m_wndStatistics.InsertItem ( sTime);
				}
				else if( itMap == TimeTreeItemMap.begin() )
				{
					hti = m_wndStatistics.InsertItem ( sTime , 0, TVI_FIRST );
				}
				else
				{	
					--itMap;
					hti = m_wndStatistics.InsertItem ( sTime, 0, itMap->second ); 
				}
				TimeTreeItemMap.insert( std::make_pair(  it->m_Time , hti ));
				m_wndStatistics.SetItemData( hti, it->m_nID );
				//m_wndStatistics.SetSubItem( hti, Col_Date, OX_SUBITEM_TEXT, sTime );
				m_wndStatistics.SetSubItem( hti, Col_Number, OX_SUBITEM_TEXT, sSpect );
			}
		}

}
catch(std::bad_cast){};

#if 0
void CVideoStatView::OnDblClkNotify( NMHDR* pNMHDR, LRESULT* pResult  )
{
	if( pNMHDR->hwndFrom == m_wndStatistics.GetSafeHwnd() &&
		pNMHDR->idFrom	== IDC_LIST_STAT &&
		pNMHDR->code	== NM_DBLCLK)
	{
		NMITEMACTIVATE * pHdr = (NMITEMACTIVATE*)pNMHDR;
		UINT flags;
		int nIndex = m_wndStatistics.HitTest( pHdr->ptAction, &flags );
		HTREEITEM hti = m_wndStatistics.GetItemFromIndex( nIndex );
		if(hti && (flags & TVHT_ONITEMLABEL ))
		{

		}
		*pResult = TRUE;
	}
	*pResult = FALSE;
}
#endif
void CVideoStatView::OnDblClickItem(HTREEITEM hti, CPoint pt)
{
	UNREFERENCED_PARAMETER(pt);
	if( m_wndStatistics.GetParentItem(hti) !=
		m_wndStatistics.GetRootItem() )
	{

	}
	else
	{
		int nStatID = m_wndStatistics.GetItemData(hti);
		CVideoDocument* pDoc = GetDocument();
		pDoc->LoadImageRoomState (nStatID);
		m_bVideoMode = false;
	}
}

void CVideoStatView::OnRClickItem(HTREEITEM hti)
{
	UNREFERENCED_PARAMETER(hti);
}

void CVideoStatView::OnButtonPlay()
{
/*	if( !(m_btnPlay.GetState() & 0x0004) )
	{
		CArrangeStatView::OnRoomChange( GetCurRoom() )	;
	}*/
	if( !m_bVideoMode )
	{
		CArrangeStatView::OnRoomChange( GetCurRoom() )	;
	}
	m_bVideoMode = true;
}

LRESULT CVideoStatView::OnUpdateUI( WPARAM wParam, LPARAM)
{
//	m_btnPlay.SetState( m_bVideoMode ? TRUE : FALSE );
	CFrameWnd* pTarget = (CFrameWnd*)GetOwner();
	if (pTarget == NULL || !pTarget->IsFrameWnd())
		pTarget = GetParentFrame();
	if (pTarget != NULL)
		OnUpdateCmdUI(pTarget, (BOOL)wParam);
	return 0L;
}

void CVideoStatView::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	//UpdateDialogControls(pTarget, bDisableIfNoHndler);
	pTarget = (CFrameWnd*)GetParent();
	m_wndVideoControlBar.OnUpdateCmdUI( pTarget, bDisableIfNoHndler );
}

void CVideoStatView::OnUpdateButtonPlay(CCmdUI* pCmdUI)
{
//	m_btnPlay.SetState( m_bVideoMode ? TRUE : FALSE );
	pCmdUI->SetCheck( m_bVideoMode ? 1 : 0 );
#if 0
	m_btnPlay.SetCheck( m_bVideoMode ? 1 : 0  );
#endif
}