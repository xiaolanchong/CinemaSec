// VideoView.cpp : implementation file
//

#include "stdafx.h"
#include "ReportParamView.h"
#include "ReportDocument.h"
#include ".\reportparamview.h"
#include "..\GUI\Balloon\BalloonHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportParamView

IMPLEMENT_DYNCREATE(CReportParamView, CFormView)

CReportParamView::CReportParamView(): 
	CFormView(IDD), 
	m_bInit(false),
	m_bReportCreated(false)
{
}

CReportParamView::~CReportParamView()
{
}

BEGIN_MESSAGE_MAP(CReportParamView, CFormView)
	//{{AFX_MSG_MAP(CReportParamView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_COMMAND(IDC_BUTTON_CREATE, OnButtonCreate)
	ON_COMMAND(IDC_BUTTON_EXPORT, OnButtonExport)
	ON_COMMAND(IDC_RADIO_MEASURE_MOMENT,OnRadioMeasure)
	ON_COMMAND(IDC_RADIO_FILM,			OnRadioFilm)
	ON_NOTIFY( NM_CLICK, IDC_TREE_ROOM, OnTreeClick )
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_PRINTPREVIEW,	OnBnClickedButtonPrintpreview)
	ON_BN_CLICKED(IDC_BUTTON_PRINT,			OnBnClickedButtonPrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportParamView diagnostics

#ifdef _DEBUG
void CReportParamView::AssertValid() const
{
	CFormView::AssertValid();
}

void CReportParamView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CReportParamView message handlers

void CReportParamView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ROOM,		m_wndRoom);
	DDX_Control(pDX, IDC_BEGIN_DATE,	m_wndBeginDate);
	DDX_Control(pDX, IDC_BEGIN_TIME,	m_wndBeginTime);
	DDX_Control(pDX, IDC_END_DATE,		m_wndEndDate);
	DDX_Control(pDX, IDC_END_TIME,		m_wndEndTime);
}

CReportDocument* CReportParamView::GetDocument()
{
	return dynamic_cast<CReportDocument*>( m_pDocument );
}

void CReportParamView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	if( !m_bInit )
	{
		m_bInit = true;

		m_LayoutManager.Attach(this);
		m_LayoutManager.AddAllChildren();

		m_LayoutManager.SetConstraint(IDC_TAB_CINEMA, OX_LMS_RIGHT, OX_LMT_SAME,  -2 );
		// | 2px <- begin date-> 2px <-begin time-> 2px | width(begin date)= 50% of main window
		m_LayoutManager.SetConstraint(IDC_BEGIN_DATE, OX_LMS_LEFT, OX_LMT_SAME, 2);
		m_LayoutManager.SetConstraint(IDC_BEGIN_DATE, OX_LMS_RIGHT, OX_LMT_POSITION, 50);

		m_LayoutManager.SetConstraint(IDC_BEGIN_TIME, OX_LMS_RIGHT, OX_LMT_SAME, -2);
		m_LayoutManager.SetConstraint(IDC_BEGIN_TIME, OX_LMS_LEFT, OX_LMT_OPPOSITE, 2,IDC_BEGIN_DATE );

		// | 2px <- end date-> 2px <-end time-> 2px | width(end date)= 50% of main window
		// as above
		m_LayoutManager.SetConstraint(IDC_END_DATE, OX_LMS_LEFT, OX_LMT_SAME, 2);
		m_LayoutManager.SetConstraint(IDC_END_DATE, OX_LMS_RIGHT, OX_LMT_POSITION, 50);

		m_LayoutManager.SetConstraint(IDC_END_TIME, OX_LMS_RIGHT, OX_LMT_SAME, -2);
		m_LayoutManager.SetConstraint(IDC_END_TIME, OX_LMS_LEFT, OX_LMT_OPPOSITE, 2,IDC_END_DATE );



		UINT ButtonArr[] = 
		{
			IDC_BUTTON_CREATE,
			IDC_BUTTON_EXPORT,
			IDC_BUTTON_PRINT,
			IDC_BUTTON_PRINTPREVIEW
		};

		for( size_t i = 0 ; i < sizeof(ButtonArr)/sizeof(UINT);++i )
		{
#if 0
			m_LayoutManager.SetConstraint( ButtonArr[i], OX_LMS_BOTTOM, OX_LMT_SAME, 25+2, IDC_TREE_ROOM);
			m_LayoutManager.SetConstraint(ButtonArr[i], OX_LMS_TOP, OX_LMT_OPPOSITE, 2, IDC_TREE_ROOM);
#else
			m_LayoutManager.SetConstraint( ButtonArr[i], OX_LMS_RIGHT, OX_LMT_SAME, -2);
#endif
		}

		m_LayoutManager.SetConstraint(IDC_TREE_ROOM, OX_LMS_RIGHT,	OX_LMT_SAME, -2);
//		m_LayoutManager.SetConstraint(IDC_TREE_ROOM, OX_LMS_BOTTOM, OX_LMT_SAME, -2 - 25 * 4 );
		m_LayoutManager.RedrawLayout();

		CheckDlgButton( IDC_RADIO_MEASURE_MOMENT, 1 );
		SetRadioState(true);

		CFont* pFont = m_wndRoom.GetFont();
		ASSERT(pFont);
		LOGFONT lf;
		pFont->GetLogFont( &lf );
		m_fnTree.CreatePointFont( 150, lf.lfFaceName ); //15pt
		m_wndRoom.SetFont( &m_fnTree );

		m_wndRoom.ModifyStyle( TVS_CHECKBOXES, 0 );
		m_wndRoom.ModifyStyle( 0, TVS_CHECKBOXES|TVS_TRACKSELECT );

		// in Init tree will be filled
		Init( IDC_TAB_CINEMA, IDC_TAB_ROOM, this ,this );

		m_wndBeginTime.SetFormat(_T("HH:mm"));
		m_wndEndTime.SetFormat(_T("HH:mm"));

		EnableAdditionalButtons( m_bReportCreated );
	}
	SetScrollSizes(MM_TEXT, CSize(1, 1));
}

int CReportParamView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
		
	return 0;
}

void CReportParamView::OnButtonCreate()
{
	CReportDocument* pDoc = GetDocument();

	CTime
	timeBeginDate	,
	timeBeginTime	, 
	timeEndDate		, 
	timeEndTime		; 

	m_wndBeginDate.GetTime(timeBeginDate);
	m_wndBeginTime.GetTime(timeBeginTime);
	m_wndEndDate.GetTime(timeEndDate);
	m_wndEndTime.GetTime(timeEndTime);

	CTime timeBegin(timeBeginDate.GetYear(), timeBeginDate.GetMonth(), timeBeginDate.GetDay(),
					timeBeginTime.GetHour(), timeBeginTime.GetMinute(), 0	);


	CTime timeEnd(timeEndDate.GetYear(), timeEndDate.GetMonth(), timeEndDate.GetDay(),
					timeEndTime.GetHour(), timeEndTime.GetMinute(), 0	);

	// также можно сравнить с текущим временем и выдать ошибку для отчетов будущего
	
	if( timeBegin > timeEnd )
	{
		UINT nStyle = 	( CBalloonHelp::unCLOSE_ON_ANYTHING & ~  CBalloonHelp::unCLOSE_ON_MOUSE_MOVE)|
						CBalloonHelp::unSHOW_CLOSE_BUTTON ;

		m_wndEndDate.SetTime(&timeBeginDate);
		m_wndEndTime.SetTime(&timeBeginTime);

		CRect rc;
		m_wndEndTime.GetWindowRect(&rc);
		CString sTitle, sContent;
		sTitle.LoadString( IDS_ERROR_INVALIDTIME );
		sContent.LoadString( IDS_ERROR_DESC_1 );
		CBalloonHelp::LaunchBalloon( sTitle, sContent, rc.CenterPoint(), IDI_ERROR, nStyle );	
		return;
/*		else if( timeBegin > timeNow )
		{
			sContent.LoadString( IDS_ERROR_DESC_2 );
			CBalloonHelp::LaunchBalloon( sTitle, sContent, rc.CenterPoint(), IDI_ERROR, nStyle );	
			return;
		}
		else if( timeEnd > timeNow )
		{
			sContent.LoadString( IDS_ERROR_DESC_3 );
			CBalloonHelp::LaunchBalloon( sTitle, sContent, rc.CenterPoint(), IDI_WARNING, nStyle );	
			timeEnd = timeNow;
			m_wndEndTime.SetTime(&timeEnd);	
		}
		return;*/
	}

	HTREEITEM hti = m_wndRoom.GetNextItem( m_wndRoom.GetRootItem(), TVGN_ROOT );

	// probably no rooms or no db connections
	if( !hti) return;
	std::vector<int> Rooms;
	while (hti) 
	{
		int nRoomID		= (int)m_wndRoom.GetItemData( hti );
		BOOL bChecked	= m_wndRoom.GetCheck( hti );
		if( bChecked ) Rooms.push_back( nRoomID );
		hti = m_wndRoom.GetNextItem( hti, TVGN_NEXT );
	}
	
	m_bReportCreated = pDoc->CreateFirstReport( timeBegin, timeEnd, Rooms );
	EnableAdditionalButtons( m_bReportCreated );
	//ASSERT(res);
}

void CReportParamView::OnButtonExport()
{
	CReportDocument* pDoc = GetDocument();
	pDoc->SaveReport();
}

void CReportParamView::OnRadioMeasure()
{
	SetRadioState(true);
}

void CReportParamView::OnRadioFilm()
{
	SetRadioState(false);
}

void	CReportParamView::SetRadioState(bool bMeasure)
{
	if(bMeasure)
	{
		CWnd* pWnd = GetDlgItem(IDC_CHECK_DETAIL);
		ASSERT(pWnd);
		pWnd->EnableWindow(FALSE);
	}
	else
	{
		CWnd* pWnd = GetDlgItem(IDC_CHECK_DETAIL);
		ASSERT(pWnd);
		pWnd->EnableWindow(TRUE);
	}
}

void	CReportParamView::OnRoomChange(int nRoomID)
{
	// i suppose when room changed cinema too
	UNREFERENCED_PARAMETER(nRoomID);
	CReportDocument* pDoc = GetDocument();
	pDoc->FillRoomTree( &m_wndRoom, GetCurCinema() );
}

void CReportParamView::OnTreeClick( NMHDR* pNmHdr, LRESULT* pResult  )
{
	UNREFERENCED_PARAMETER(pNmHdr);
	CPoint pt;
	::GetCursorPos( &pt );
	m_wndRoom.ScreenToClient( &pt);
	UINT nFlags;
	HTREEITEM hti = m_wndRoom.HitTest( pt, &nFlags );
	if( hti /*&& ( nFlags & (TVHT_ONITEM|TVHT_NOWHERE) ) */ )
	{
		BOOL bCheck = m_wndRoom.GetCheck( hti );
		m_wndRoom.SetCheck( hti, !bCheck );
	}
	*pResult = TRUE;
}
void CReportParamView::OnBnClickedButtonPrintpreview()
{
	// TODO: Add your control notification handler code here
	CReportDocument* pDoc = GetDocument();
	pDoc->PreviewReport();
}

void CReportParamView::OnBnClickedButtonPrint()
{
	// TODO: Add your control notification handler code here
	CReportDocument* pDoc = GetDocument();
	pDoc->PrintReport();
}

void	CReportParamView::EnableAdditionalButtons( bool bEnable )
{
	UINT ButtonArr[] = 
	{
		IDC_BUTTON_EXPORT,
		IDC_BUTTON_PRINT,
		IDC_BUTTON_PRINTPREVIEW
	};
	for( size_t i = 0; i < sizeof(ButtonArr)/sizeof( ButtonArr[0] ); ++i )
	{
		CWnd* pWnd = GetDlgItem( ButtonArr[i] );
		if( pWnd ) pWnd->EnableWindow( bEnable ? TRUE : FALSE );
	}
}