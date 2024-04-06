// AlgoTimetable.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "timetable_mode_view.h"
#include "timetable_mode_doc.h"

#include "../gui/Grid/GridCellDateTime.h"
#include "../DBFacet/CfgDB.h"
#include "../gui/Balloon/BalloonHelp.h"
#include "../../CSEngine/DBFacet/DBRows.h"

const UINT c_Timer_Grid		= 0x1e75;
const UINT c_Period_Grid	= 500; // ms

#define INVALID_FILMID -1

// CTimeTableView

IMPLEMENT_DYNCREATE(CTimeTableView, CFormView)

#pragma warning( disable : 4355 ) // this used in base member initializer list, I KNOW THIS!!!

CTimeTableView::CTimeTableView()
	: CFormView(CTimeTableView::IDD),
	m_bInitialized(false),
	m_wndGrid(this)
{
}

CTimeTableView::~CTimeTableView()
{
}

void CTimeTableView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATE_TIMETABLE,		m_wndDate);
	DDX_Control(pDX, IDC_TIME_START,			m_wndStartTime);
	DDX_Control(pDX, IDC_SPIN_ROW_NUMBER,		m_spinRowNumber);
	DDX_Control(pDX, IDC_DATE_SAVE_FROM,		m_wndDateSaveFrom);
	DDX_Control(pDX, IDC_DATE_SAVE_TO,			m_wndDateSaveTo);
	DDX_Control(pDX, IDC_CHECK_AFTER_MIDNIGHT,	m_btnIgnoreAfterMidnight);
}


BEGIN_MESSAGE_MAP(CTimeTableView, CFormView)
	ON_COMMAND( IDC_BUTTON_LOAD, OnLoad )
	ON_COMMAND( IDC_BUTTON_SAVE, OnSave )
	ON_NOTIFY( DTN_DATETIMECHANGE, IDC_DATE_TIMETABLE, OnNotifyDateChange )
	ON_NOTIFY( UDN_DELTAPOS, IDC_SPIN_ROW_NUMBER, OnNotifyRowNumberChange )
	 ON_WM_TIMER()
END_MESSAGE_MAP()


// CTimeTableView diagnostics

#ifdef _DEBUG
void CTimeTableView::AssertValid() const
{
	CFormView::AssertValid();
}

void CTimeTableView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CTimeTableDoc * CTimeTableView::GetDocument() // debug version
{
	ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CTimeTableDoc ) ) );
	return (CTimeTableDoc*)m_pDocument;
}
#endif //_DEBUG

BOOL	CTimeTableView::Create( CWnd* pParent, const CRect & rc, CCreateContext& cc , int i )
{
	return CFormView::Create( NULL,NULL,WS_CHILD | WS_VISIBLE /*| WS_BORDER*/, rc, pParent, AFX_IDW_PANE_FIRST+1 + i, &cc );
}

// CTimeTableView message handlers

void CTimeTableView::OnInitialUpdate( )
{
	CFormView::OnInitialUpdate();
	if(!m_bInitialized )
	{
		m_bInitialized = true;

		ModifyStyle( 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS );
		CRect rc;
		CWnd* pWnd = GetDlgItem(IDC_GRID_TIMETABLE);
		pWnd->GetWindowRect( &rc );
		ScreenToClient(&rc);
		pWnd->DestroyWindow();
		m_wndGrid.Create( rc, this, IDC_GRID_TIMETABLE );
		m_wndGrid.SetBkColor( GetSysColor( COLOR_BTNFACE ) );

		m_wndGrid. SetFixedRowCount(1);
		//		m_wndGrid. SetFixedColumnCount(1);

		Init(IDC_TAB_CINEMA, IDC_TAB_ROOM, this, this);

		m_LayoutManager.Attach(this);
		m_LayoutManager.AddAllChildren();

		m_LayoutManager.SetConstraint(IDC_GRID_TIMETABLE, OX_LMS_RIGHT, OX_LMT_SAME, -2);
		m_LayoutManager.SetConstraint(IDC_GRID_TIMETABLE, OX_LMS_BOTTOM, OX_LMT_SAME, -2);

		m_LayoutManager.SetConstraint(IDC_TAB_CINEMA, OX_LMS_RIGHT, OX_LMT_SAME, -3);
		m_LayoutManager.SetConstraint(IDC_TAB_ROOM, OX_LMS_RIGHT, OX_LMT_SAME, -3);

		m_LayoutManager.RedrawLayout();
		//FIXME: to resources
		//		m_wndGrid.GetCol  ( _T( "#" ) );

		CString sName, sStartTime, sEndTime;
		sName.LoadString(		IDS_TITLE_NAME );
		sStartTime.LoadString(	IDS_TITLE_STARTTIME );
		sEndTime.LoadString(	IDS_TITLE_ENDTIME );
		m_wndGrid.InsertColumn( sName );
		m_wndGrid.InsertColumn( sStartTime );
		m_wndGrid.InsertColumn( sEndTime );

		m_wndGrid.SetHeaderSort();
		m_wndGrid.SetSortColumn(Col_Name);
		m_wndGrid.SetSortColumn(Col_Begin);
		m_wndGrid.SetSortColumn(Col_End);

		m_wndGrid.SetCompareFunction( CTimeTableView::CompareItem );

		m_wndGrid.SetColumnWidth( Col_Name, 300);
		m_wndGrid.SetColumnWidth( Col_Begin, 100);
		m_wndGrid.SetColumnWidth( Col_End, 100);

		m_wndGrid.EnableSelection();
		m_wndGrid.SetSingleRowSelection(FALSE);

		m_spinRowNumber.SetRange32( 0, 100 );
		m_spinRowNumber.SetPos32( 10 );

		SetScrollSizes( MM_TEXT, CSize(1, 1) );

		SetRowCount( m_spinRowNumber.GetPos32() );

		SetTimer( c_Timer_Grid, c_Period_Grid, 0 );

		m_wndStartTime.SetFormat(_T("HH:mm"));
		CTime timeTrueAndEvilDarkTime(2005, 1, 1, 10, 0, 0);
		m_wndStartTime.SetTime( &timeTrueAndEvilDarkTime );
#if 1
		pWnd = GetDlgItem( IDC_BUTTON_SAVE );
		ASSERT(pWnd);
		if(pWnd) pWnd->EnableWindow( GetCfgDB().IsUserAnAdmin() ? TRUE : FALSE  );
#endif
		CheckDlgButton( IDC_CHECK_AFTER_MIDNIGHT, BST_CHECKED);
	}
	ModifyStyleEx(  WS_EX_CLIENTEDGE, 0 );
	ModifyStyle(  WS_BORDER, 0 );
}

/// grid ////////

void  CGridCtrlEx::OnEndEditCell(int nRow, int nCol, CString str)
{
	CGridCtrl::OnEndEditCell( nRow, nCol, str );
	m_pView->OnEndEditCell( nRow, nCol, str);
}

BOOL  CGridCtrlEx::ValidateEdit(int nRow, int nCol, LPCTSTR str)
{
	return m_pView->ValidateEdit( nRow, nCol, str );
}

BEGIN_MESSAGE_MAP(CGridCtrlEx, CGridCtrl)
	//{{AFX_MSG_MAP(CGridCtrlEx)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CGridCtrlEx::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if( nChar == VK_DELETE )
	{
		bool bShift = ( GetKeyState( VK_SHIFT ) < 0 );
		m_pView->OnDeleteKey( bShift );
	}
	else 
		CGridCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
}

void  CTimeTableView::OnEndEditCell(int nRow, int nCol, CString str)
{
	/*	CVVMapEditorDoc* pDoc = GetDocument();*/
	int z = m_wndGrid.GetRowCount() - 1;

	if( nRow == z && ValidateEdit( nRow,  nCol,  str) && nCol == Col_End)
	{
		/*	const AnchorPointMap_t& ap = pDoc->AnchorPoints();
		InsertRow( ap.size() + 1 );

		for( int i= COLUMN_X_RO; i <= COLUMN_Z_PSI; ++i)
		{
		CString str = m_wndGrid.GetItemText( nRow, i );
		if( str.IsEmpty() )
		{
		str.Format(_T("%.3f"), 0.0);
		m_wndGrid.SetItemText( nRow, i, str );
		}
		}
		InsertCoords(nRow);*/
		//	InsertRow();
		//	m_wndGrid.SetFocusCell( nRow, nCol );

		//	m_wndGrid.RedrawWindow();
		RefreshGrid();
	}
}


BOOL  CTimeTableView::ValidateEdit(int nRow, int nCol, LPCTSTR str)
{
	UNREFERENCED_PARAMETER(nRow);
	UNREFERENCED_PARAMETER(nCol);
	UNREFERENCED_PARAMETER(str);
	return TRUE;
}

void	CTimeTableView::OnDeleteKey( bool bShift )
{
	if(!bShift) 
	{
		CCellRange cr = m_wndGrid.GetSelectedCellRange();
		for( int i = cr.GetMinRow(); i <= cr.GetMaxRow(); ++i)
			for( int j = cr.GetMinCol(); j <= cr.GetMaxCol() ; ++j)
			{
				CGridCellBase* pCell = m_wndGrid.GetCell( i,j );
				pCell->SetText(0);
			}
	}
	else
	{
		CCellRange Range = m_wndGrid.GetSelectedCellRange();
	//	int nRowCount = m_wndGrid.GetRowCount();

		int nNumberDelRow = 0;
		for( int i = Range.GetMinRow(); i <= Range.GetMaxRow(); ++i , ++nNumberDelRow)
		{
			m_wndGrid.DeleteRow( i );
		}
		for(int i = 0; i < nNumberDelRow; ++i)
			InsertRow();
	}
	m_wndGrid.RedrawWindow();
}

void	CTimeTableView::InsertRow()
{
//	m_wndGrid.GetColumnCount();
	int nRow = m_wndGrid.InsertRow(_T(""));

	m_wndGrid.SetRowHeight( nRow ,25);
	m_wndGrid.SetCellType(nRow, Col_Begin, RUNTIME_CLASS(CGridCellDateTime));
	((CGridCellDateTime*) m_wndGrid.GetCell(nRow, Col_Begin))->Init(0   ,DTS_EX_SHOW_MINUTES|DTS_EX_NO_INIT_VALUE);

	m_wndGrid.SetCellType(nRow, Col_End, RUNTIME_CLASS(CGridCellDateTime));
	((CGridCellDateTime*) m_wndGrid.GetCell(nRow, Col_End))->Init(0  , DTS_EX_SHOW_MINUTES|DTS_EX_NO_INIT_VALUE);
}

MACRO_EXCEPTION( TimeInputValidateException, CommonException);
MACRO_EXCEPTION( EmptyFieldException,		TimeInputValidateException);
MACRO_EXCEPTION( InvalidPeriodException,	TimeInputValidateException);

void CTimeTableView::OnLoad()
{
	std::pair<CTime, CTime>	TimePeriod = GetPeriodForLoad();
	try
	{
		TimeTable_t tt;
		std::vector<std::wstring> Name;

		CTimeTableDoc* pDoc = GetDocument();

		int nRoomID = GetCurRoom();
		pDoc->Load( nRoomID, TimePeriod.first, TimePeriod.second, tt, Name );

		m_spinRowNumber.SetPos32( (int)tt.size() );
		SetRowCount( (int)tt.size() );

		for( size_t i = 0; i < tt.size() ; ++i )
		{
			int nIndex = 1 + (int)i;
			CTime TimeBegin, TimeEnd;
			CGridCellDateTime* pCell;
			m_wndGrid.GetCell(nIndex, Col_Name )->SetText( Name[i].c_str() ) ;
			pCell = (CGridCellDateTime*)m_wndGrid.GetCell( nIndex, Col_Begin ) ;
			pCell->Init( tt[i].m_timeBegin, 0, DTS_EX_SHOW_MINUTES );
			pCell = (CGridCellDateTime*)m_wndGrid.GetCell( nIndex, Col_End ) ;
			pCell->Init(tt[i].m_timeEnd, 0, DTS_EX_SHOW_MINUTES );
			m_wndGrid.GetCell( (int)i, Col_Name )->SetData( tt[i].m_nFilmID  );
		}
		m_wndGrid.RedrawWindow();
	}
	catch(EmptyFieldException)
	{
		AfxMessageBox( IDS_ERROR_TABLEITEM, MB_OK|MB_ICONERROR );
	}
	catch( TimeHelperException& ex)
	{
		CString sError;
		sError.Format( IDS_ERROR_ROW, ex.GetErrorIndex() );
		AfxMessageBox( sError, MB_OK|MB_ICONERROR  );
	}
}

void CTimeTableView::OnSave()
{
	if( !GetCfgDB().IsUserAnAdmin() || !CheckUserDate() ) return;

	std::pair<CTime, CTime>	TimePeriod = GetPeriodForSave();
	std::map<size_t, int>					Index2Record;
	try
	{
		TimeTable_t FullTimetable, DayTimetableForValidate;
		std::vector<std::wstring>				DayName, FullName;
		std::vector<std::pair<CTime, CTime> >	DayTimetable;
		for( int i = 1; i < m_wndGrid.GetRowCount(); ++i)
		{
			CTime TimeBegin, TimeEnd;
			CGridCellDateTime* pCell;
			pCell = (CGridCellDateTime*)m_wndGrid.GetCell( i, Col_Begin ) ;
			CString sBegin = pCell->GetText();
			TimeBegin = *pCell->GetTime();
			pCell = (CGridCellDateTime*) m_wndGrid.GetCell( i, Col_End ) ;
			CString sEnd = pCell->GetText();
			TimeEnd = *pCell->GetTime();

			if(( !sBegin.IsEmpty() && sEnd.IsEmpty()) || 
				(sBegin.IsEmpty() && !sEnd.IsEmpty() ) )
			{
				throw EmptyFieldException("Error");
			}
			else if( sBegin.IsEmpty() && sEnd.IsEmpty() ) continue;
			TimeBegin	= GetTrueTimeForSave( TimeBegin.GetHour(), TimeBegin.GetMinute() );
			TimeEnd		= GetTrueTimeForSave( TimeEnd.GetHour(), TimeEnd.GetMinute()  );
#if 0
			int nFilmID = (int)m_wndGrid.GetItemData( (int)i, Col_Name );
			tt.push_back( FilmTime( nFilmID, CTime(TimeBegin), CTime(TimeEnd) ) );
#else
			DayTimetableForValidate.push_back( FilmTime( FilmTime::s_nUserFilmID, CTime(TimeBegin), CTime(TimeEnd) ) );
			DayTimetable.push_back( std::make_pair( TimeBegin, TimeEnd ) );
#endif
			std::wstring sCellName ( m_wndGrid.GetCell(i, Col_Name )->GetText() );
			DayName.push_back( sCellName  );
		}
		CTimeTableDoc* pDoc = GetDocument();
		try
		{
			DumpTimetable( DayTimetableForValidate, DayName );
			pDoc->Validate( DayTimetableForValidate );
		}
		catch( TimeHelperLateBeginException& ex)
		{
			CString sError;
			sError.Format( IDS_ERROR_ROWTIME , ex.GetErrorIndex() + 1);
			AfxMessageBox( sError, MB_OK|MB_ICONERROR );
			return;
		}
		catch( TimeHelperIntersectionException& ex)
		{
			CString sError;
			sError.Format( IDS_ERROR_ROWINTERSECT, ex.GetErrorIndex() + 1 );
			AfxMessageBox( sError, MB_OK|MB_ICONERROR );
			return;
		}
		bool bEnableAfterMidnight = IsDlgButtonChecked( IDC_CHECK_AFTER_MIDNIGHT ) == BST_CHECKED ;
			
		PropagateDay(	TimePeriod.first, TimePeriod.second, 
						DayTimetable, DayName, 
						FullTimetable, FullName, Index2Record, bEnableAfterMidnight );
		int nRoomID = GetCurRoom();
#if 1
		// NOTE +1 day
		pDoc->Save( nRoomID, TimePeriod.first, TimePeriod.second, FullTimetable, FullName );
#else
		// dump me
		DumpTimetable( FullTimetable, FullName );
#endif
	}
	catch(EmptyFieldException)
	{
		AfxMessageBox( IDS_ERROR_TABLEITEM, MB_OK|MB_ICONERROR );
	}
	catch( TimeHelperLateBeginException& ex)
	{
		CString sError;
		sError.Format( IDS_ERROR_ROWTIME , ex.GetErrorIndex() + 1);
		AfxMessageBox( sError, MB_OK|MB_ICONERROR );
	}
	catch( TimeHelperIntersectionException& ex)
	{
		CString sError;
		sError.Format( IDS_ERROR_ROWINTERSECT, ex.GetErrorIndex() + 1 );
		AfxMessageBox( sError, MB_OK|MB_ICONERROR );
	}
	catch( InvalidIntervalException& ex)
	{
		//FIXME : check if the record exists
		size_t nIndex = ex.GetRecordNumber();
		CString sError;
		sError.Format( IDS_ERROR_ROWTIME, Index2Record[nIndex] );
		AfxMessageBox( sError, MB_OK|MB_ICONERROR );
	}
	catch( IntervalIntersectionException& ex)
	{
		//FIXME : check if the record exists
		size_t nIndex = ex.GetRecordNumber();
		CString sError;
		sError.Format( IDS_ERROR_ROWINTERSECT, Index2Record[nIndex] );
		AfxMessageBox( sError, MB_OK|MB_ICONERROR );
	}
	catch( DataBaseException )
	{
		AfxMessageBox( IDS_ERROR_DB_SAVING, MB_OK|MB_ICONERROR );
	}
}

void CTimeTableView::OnTimer(UINT_PTR nID)
{
	CFormView::OnTimer(nID);
	if( nID == c_Timer_Grid  )
	{
		RefreshGrid();
	}
}

void	CTimeTableView::RefreshGrid()
{
	CTime timeDate, timeTime, timeCur  = CTime::GetCurrentTime();
	m_wndDate.GetTime( timeDate);

	for( int i = 1; i < m_wndGrid.GetRowCount(); ++i)
	{
		CTime TimeBegin, TimeEnd;
		CGridCellDateTime* pCell;
		pCell = (CGridCellDateTime*)m_wndGrid.GetCell( i, Col_Begin ) ;
		CString sBegin = pCell->GetText();
		if( !pCell->IsEditing() && !sBegin.IsEmpty())
		{
#ifdef  USE_TIME_COLORIZE
			TimeBegin = *pCell->GetTime();
			timeTime = GetTrueTime(	 TimeBegin.GetHour(), TimeBegin.GetMinute()); 
			pCell->SetBackClr( timeTime < timeCur ? GetSysColor( COLOR_BTNFACE ) : RGB(255,255,255) );
#endif
		}
		else
		{
			pCell->SetBackClr(RGB(255,255,255));
		}
		if( !pCell->IsEditing() )
		{
			m_wndGrid.RedrawCell( i, Col_Begin );
		}

		pCell = (CGridCellDateTime*) m_wndGrid.GetCell( i, Col_End ) ;
		CString sEnd = pCell->GetText();
		if( !pCell->IsEditing() && !sEnd.IsEmpty() )
		{
#ifdef  USE_TIME_COLORIZE
			TimeEnd = *pCell->GetTime();
			timeTime = GetTrueTime(	 TimeEnd.GetHour(), TimeEnd.GetMinute() ); 
			pCell->SetBackClr( timeTime < timeCur ? GetSysColor( COLOR_BTNFACE )  : RGB(255,255,255) );
			//	m_wndGrid.RedrawCell( i, Col_End );
#endif
		}

		else
		{
			pCell->SetBackClr(RGB(255,255,255));
			//		m_wndGrid.RedrawCell( i, Col_End );
		}
		if( !pCell->IsEditing() )
		{
			m_wndGrid.RedrawCell( i, Col_End );
		}
	}
}

void  CTimeTableView::OnNotifyDateChange( NMHDR * pNotifyStruct, LRESULT* result )
{
	UNREFERENCED_PARAMETER(pNotifyStruct);
	RefreshGrid();
	*result = TRUE;
}

void CTimeTableView::OnRoomChange(int nRoomID)
{
	UNREFERENCED_PARAMETER(nRoomID);
	ChangeRoomInControl( );
}

void CTimeTableView::ChangeRoomInControl( )
{
	RefreshGrid();
}

std::pair<CTime, CTime>	CTimeTableView::GetPeriodForSave()
{
	CTime timeBegin, timeEnd, timeStartHour;
	m_wndStartTime.GetTime( timeStartHour );
	m_wndDateSaveFrom.GetTime( timeBegin	);
	m_wndDateSaveTo.GetTime( timeEnd		);
	timeBegin = CTime(	timeBegin.GetYear(), timeBegin.GetMonth(), timeBegin.GetDay(),
						timeStartHour.GetHour(), timeStartHour.GetMinute(), 0	);
	timeEnd = CTime(	timeEnd.GetYear(), timeEnd.GetMonth(), timeEnd.GetDay(),
						timeStartHour.GetHour(), timeStartHour.GetMinute(), 0	);
	// +1 day , same as below
	timeEnd +=  CTimeSpan( 1, 0, 0, 0);
	return std::make_pair( timeBegin, timeEnd );
}

std::pair<CTime, CTime>	CTimeTableView::GetPeriodForLoad()
{
	CTime timeBegin, timeEnd, timeStartHour;
	m_wndStartTime.GetTime( timeStartHour );
	m_wndDate.GetTime( timeBegin );
	m_wndStartTime.GetTime( timeStartHour );
	timeBegin = CTime(timeBegin.GetYear(), timeBegin.GetMonth(), timeBegin.GetDay(),
		timeStartHour.GetHour(), timeStartHour.GetMinute(), 0	);
	// +1 day
	timeEnd = timeBegin + CTimeSpan( 1, 0, 0, 0 );
	return std::make_pair( timeBegin, timeEnd );
}

CTime	CTimeTableView::GetTrueTimeInternal( CTime timeStart, int nHours, int nMinutes)
{
	CTime timeDay;
	m_wndStartTime.GetTime( timeDay );
	CTime timeBegin (timeStart.GetYear(), timeStart.GetMonth(), timeStart.GetDay(), timeDay.GetHour(), timeDay.GetMinute(), 0 );	
	CTime timeTrue ( timeStart.GetYear(), timeStart.GetMonth(), timeStart.GetDay(), nHours, nMinutes, 0);
	if( timeTrue < timeBegin )
	{
		timeTrue += CTimeSpan(1, 0, 0, 0);
	}
	return timeTrue;
}

CTime	CTimeTableView::GetTrueTimeForLoad( int nHours, int nMinutes)
{
	CTime timeStart;
	m_wndStartTime.GetTime( timeStart );
	return GetTrueTimeInternal( timeStart, nHours, nMinutes ) ;
}

CTime	CTimeTableView::GetTrueTimeForSave( int nHours, int nMinutes)
{
	CTime timeStart;
	m_wndDateSaveFrom.GetTime( timeStart );
	return GetTrueTimeInternal( timeStart, nHours, nMinutes ) ;
}

int CALLBACK	CTimeTableView::CompareItem( LPARAM _1st, LPARAM _2nd, LPARAM _3rd )
{
//	BOOL bAsc = !(BOOL)_3rd;
	CGridCellBase	*pFirst		= (CGridCellBase*)_1st,
					*pSecond	= (CGridCellBase*)_2nd;
	CString s1stText = pFirst->GetText(),
			s2ndText = pSecond->GetText();
	if( s1stText == s2ndText ) return 0;
	if( s1stText.IsEmpty() )	return 1;
	if( s2ndText.IsEmpty() )	return -1;
	return int(s1stText > s2ndText)* 2 - 1; 
}

void CTimeTableView::OnNotifyRowNumberChange( NMHDR * pNotifyStruct, LRESULT* result )
{
	NMUPDOWN* pHdr = (NMUPDOWN*)pNotifyStruct;
	int nNewRowCount = pHdr->iPos + pHdr->iDelta;
	int nUpper, nLower;
	m_spinRowNumber.GetRange32( nLower, nUpper );
	if( nLower <= nNewRowCount &&
		nUpper >= nNewRowCount )
	{
		SetRowCount( nNewRowCount );
		*result = 0;
	}
	else
		*result = 1;
}

void	CTimeTableView::SetRowCount( int nCount )
{
	int nPrev = m_wndGrid.GetRowCount() - 1;
	if( nPrev == nCount ) return;
	if( nPrev > nCount  )
	{
/*		for( int i = nPrev; i > nCount; --i  )
			m_wndGrid.DeleteRow( i );*/
		m_wndGrid.SetRowCount( nCount + 1 );
	}
	else
	{
		for( int i = nPrev; i < nCount; ++i  )
			InsertRow( );
	}
	m_wndGrid.Invalidate();
}

bool	CTimeTableView::CheckUserDate()
{
	CString sTitle, sContent;
	sTitle.LoadString( IDS_ERROR_WRONG_DATE );
	CTime timeFrom, timeTo;
	m_wndDateSaveFrom.GetTime( timeFrom );
	m_wndDateSaveTo.GetTime( timeTo );
	if( timeFrom > timeTo )
	{
		CRect rc;
		m_wndDateSaveTo.GetWindowRect(&rc);
		UINT nStyle = 	(	CBalloonHelp::unCLOSE_ON_ANYTHING & ~  CBalloonHelp::unCLOSE_ON_MOUSE_MOVE)|
							CBalloonHelp::unSHOW_CLOSE_BUTTON;
		sContent.LoadString( IDS_ERROR_DESC_4 );
		CBalloonHelp::LaunchBalloon( sTitle, sContent, rc.CenterPoint(), IDI_ERROR, nStyle );	
		return false;
	}
	else 
		return true;
}

CTime SameTimeAfterNDays( CTime SomeTime, int nDays ) 
{ 
	struct tm *ptm = SomeTime.GetLocalTm(); 
	ptm->tm_mday += nDays; 
	ptm->tm_isdst = -1; 
	return CTime( _mktime64( ptm ) ); 
} 

void	CTimeTableView::PropagateDay(	CTime timeBegin, CTime timeEnd, 
										const std::vector< std::pair<CTime, CTime> >&	DayArr,
										const std::vector<std::wstring>&				DayName,
										TimeTable_t&									FullTimeTable, 
										std::vector<std::wstring>&						FullName,
										std::map<size_t, int>&							Index2RecordMap,
										bool											bIgnoreAfterMidnight)
{
	CTime timeStartHour ;
	m_wndStartTime.GetTime( timeStartHour );
	FullTimeTable.clear();
	FullName.clear();
	Index2RecordMap.clear();
	CTimeSpan TimePeriod ( timeEnd - timeBegin );
	for( LONG i = 0; i < TimePeriod.GetDays() ; ++i )
	{
		for( size_t j = 0; j < DayArr.size(); ++j )
		{

			// BUGFIX 2006-03-22
			// нельзя просто прибавить CTimeSpan( i, 0, 0, 0) = 24ч * i
			// при переходе на летнее время промежуток состовляет 23 или 25 ч
			// CTimeSpan игнорирует DST
			//	CTime timeBeginFilm = DayArr[j].first	+ CTimeSpan( i, 0, 0, 0);
			//	CTime timeEndFilm	= DayArr[j].second	+ CTimeSpan( i, 0, 0, 0);

			CTime timeBeginFilm	= SameTimeAfterNDays( DayArr[j].first, i );
			CTime timeEndFilm	= SameTimeAfterNDays( DayArr[j].second, i );

			int nDay = timeBeginFilm.GetDayOfWeek();
			CTime timeTrueStartHour (	timeBeginFilm.GetYear(), timeBeginFilm.GetMonth(), timeBeginFilm.GetDay(), 
										timeStartHour.GetHour(), timeStartHour.GetMinute(), 0);
			CTime timeMidnight (	timeBeginFilm.GetYear(), timeBeginFilm.GetMonth(), timeBeginFilm.GetDay(), 
				0, 0, 0);
			// if start in sat, sun & mon
			if( bIgnoreAfterMidnight					&&
#if 0
				( nDay != 7 && nDay != 1 && nDay != 2 ) && 
#else
				!GetCfgDB().EnableFilmAfterMidnight( nDay ) &&
#endif
				timeBeginFilm < timeTrueStartHour		&&
				timeBeginFilm >= timeMidnight			)
			{
				// skip
				OutputDebugString( _T("SKIP\n")  );
			}
			else
			{
				FullTimeTable.push_back( FilmTime( FilmTime::s_nUserFilmID, timeBeginFilm, timeEndFilm ) );
				FullName.push_back(  DayName[j] );
				Index2RecordMap.insert( std::make_pair(FullName.size() - 1, (int)j + 1 ) );
			}
		}
	}
}

void	CTimeTableView::DumpTimetable(	const TimeTable_t& FullTimetable, 
										const std::vector<std::wstring>& FullNames ) const 
{
	ASSERT( FullTimetable.size() == FullNames.size() );

	OutputDebugString( _T("===[Timetable]===") );
	for( size_t i = 0; i < FullTimetable.size(); ++i )
	{
		CString sBegin = FullTimetable[i].m_timeBegin.Format(_T("[%d %b] %H:%M:%S"));
		CString sEnd = FullTimetable[i].m_timeEnd.Format(_T("[%d %b] %H:%M:%S"));
		CString sFinal;
		sFinal.Format( _T("%s - %s (%ws)\n"), (LPCTSTR)sBegin, (LPCTSTR)sEnd, FullNames[i].c_str() );
		OutputDebugString( sFinal );
	}
}