#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "boxoffice_mode_doc.h"
#include "boxoffice_mode_view.h"
#include "..\GUI\Balloon\BalloonHelp.h"

#include <AFXPRIV.H>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CBoxOfficeView, CFormView)

BEGIN_MESSAGE_MAP(CBoxOfficeView, CFormView)
  //{{AFX_MSG_MAP(CBoxOfficeView)
  ON_COMMAND( IDC_BUTTON_GETDATA, OnGetData )
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CBoxOfficeView::CBoxOfficeView():
	CFormView(CBoxOfficeView::IDD),
	m_bInitialized(false)
{
}

//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CBoxOfficeView::~CBoxOfficeView()
{
}

//-------------------------------------------------------------------------------------------------
// CBoxOfficeView diagnostics.
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CBoxOfficeView::AssertValid() const
{
  CFormView::AssertValid();
}
void CBoxOfficeView::Dump( CDumpContext & dc ) const
{
  CFormView::Dump( dc );
}
CBoxOfficeDoc * CBoxOfficeView::GetDocument() // debug version
{
  ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CBoxOfficeDoc ) ) );
  return (CBoxOfficeDoc*)m_pDocument;
}
#endif //_DEBUG

void CBoxOfficeView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBoxOfficeView)
	DDX_Control( pDX, IDC_DATETIME_FROM	, m_dtFrom );
	DDX_Control( pDX, IDC_DATETIME_TO	, m_dtTo );
	//}}AFX_DATA_MAP
}

void CBoxOfficeView::OnInitialUpdate( )
{
	CFormView::OnInitialUpdate();
	if(!m_bInitialized )
	{
		m_bInitialized = true;
	
		Init( IDC_TAB_CINEMA, IDC_TAB_ROOM, this, this );

		m_wndReport.CreateFromStatic( IDC_STATIC_HTML, this );
		m_wndReport.SetHideContextMenu(TRUE);
		m_wndReport.ModifyStyle( 0, WS_BORDER );

		m_LayoutManager.Attach(this);
		m_LayoutManager.AddAllChildren();

		m_LayoutManager.SetConstraint(IDC_TAB_CINEMA,		OX_LMS_RIGHT, OX_LMT_SAME, -2);
		m_LayoutManager.SetConstraint(IDC_TAB_ROOM,			OX_LMS_RIGHT, OX_LMT_SAME, -2);

		m_LayoutManager.SetConstraint(IDC_STATIC_HTML,		OX_LMS_RIGHT,	OX_LMT_SAME, -2);
		m_LayoutManager.SetConstraint(IDC_STATIC_HTML,		OX_LMS_BOTTOM,	OX_LMT_SAME, -2);

		m_LayoutManager.RedrawLayout();

		m_dtFrom.SetFormat( _T("HH:mm dd.MM.yy"));
		m_dtTo.SetFormat( _T("HH:mm dd.MM.yy"));
	}
	// i prefer rubber windows, so switch off default CFormView layout (w/ scroll bar)
	SetScrollSizes( MM_TEXT, CSize(1, 1) );
}

 void	CBoxOfficeView::OnRoomChange(int nRoomID)
 {
	UNREFERENCED_PARAMETER( nRoomID );
 }

 void CBoxOfficeView::OnGetData()
 {
	 CTime timeFrom, timeTo;
	 CTime timeNow = CTime::GetCurrentTime();
	 m_dtFrom.GetTime(timeFrom);
	 m_dtTo.GetTime(timeTo);

	 CBoxOfficeDoc* pDoc = GetDocument();
	 CString sFileName = pDoc->CreateReport(  GetCurCinema(), timeFrom, timeTo );

	 if( timeFrom > timeNow )
	 {
		 UINT nStyle = 	( CBalloonHelp::unCLOSE_ON_ANYTHING & ~  CBalloonHelp::unCLOSE_ON_MOUSE_MOVE)|
			 CBalloonHelp::unSHOW_CLOSE_BUTTON ;


		 CRect rc;
		// CWnd* pWnd = GetDlgItem( IDC_BUTTON_GETDATA );
		 m_dtFrom.GetWindowRect(&rc);
		 CString sTitle, sContent;
		 sTitle.LoadString( IDS_ERROR_INVALIDTIME );
		 sContent.LoadString( IDS_ERROR_DESC_1 );
		 CBalloonHelp::LaunchBalloon( sTitle, sContent, rc.CenterPoint(), IDI_ERROR, nStyle );	
		 
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
		 m_wndReport.Navigate2( _T("about:blank") );
	 }
	 else
	 m_wndReport.Navigate2( CString(_T("file://")) + sFileName );
 }