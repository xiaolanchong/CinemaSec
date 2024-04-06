// AlgoSettingView.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"

#include "settings_mode_doc.h"
#include "settings_mode_view.h"

#include "../GUI/Balloon/BalloonHelp.h"

#include "../DBFacet/DBSerializer.h"
#include "../DBFacet/CfgDB.h"
#include "../../CSEngine/DBFacet/CinemaOleDB.h"

// using seconds in "Room" table instead of minutes
#define USE_SEC
// CSettingsView

IMPLEMENT_DYNCREATE(CSettingsView, CFormView)

CSettingsView::CSettingsView()
	: CFormView(CSettingsView::IDD),
	m_bInitialized(false)
{
}

CSettingsView::~CSettingsView()
{
}

void CSettingsView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TIMEAFTER,	m_edTimeAfter);
	DDX_Control(pDX, IDC_EDIT_TIMEBEFORE,	m_edTimeBefore);
	DDX_Control(pDX, IDC_EDIT_PERIOD,		m_edTimePeriod);
	DDX_Control(pDX, IDC_SPIN_TIMEAFTER,	m_spinTimeAfter);
	DDX_Control(pDX, IDC_SPIN_TIMEBEFORE,	m_spinTimeBefore);
	DDX_Control(pDX, IDC_SPIN_PERIOD,		m_spinTimePeriod);
	DDX_Control(pDX, IDC_SPIN_BACKUP_END,	m_spinBackupEnd);
	DDX_Control(pDX, IDC_EDIT_BACKUP_DATE,	m_edBackupDate);
	DDX_Control(pDX, IDC_SPIN_BACKUP_DATE,	m_spinBackupDate);
	DDX_Control(pDX, IDC_SPIN_BACKUP_START, m_spinBackupStart);
	DDX_Control(pDX, IDC_EDIT_BACKUP_START, m_edBackupStart);
	DDX_Control(pDX, IDC_EDIT_BACKUP_END,	m_edBackupEnd);

	DDX_Control(pDX, IDC_EDIT_EXC_DELAY,		m_edExcDelay);
	DDX_Control(pDX, IDC_EDIT_EXC_WAITING,		m_edExcWaiting);
	DDX_Control(pDX, IDC_EDIT_EXC_CONNECTTIME,	m_edExcConnectTime);
	DDX_Control(pDX, IDC_EDIT_EXC_CONNECTNUMBER,m_edExcConnectNumber);

	DDX_Control(pDX, IDC_SPIN_EXC_DELAY,			m_spinExcDelay);
	DDX_Control(pDX, IDC_SPIN_EXC_WAITING,			m_spinExcWaiting);
	DDX_Control(pDX, IDC_SPIN_EXC_CONNECTTIME,	m_spinExcConnectTime);
	DDX_Control(pDX, IDC_SPIN_EXC_CONNECTNUMBER,	m_spinExcConnectNumber);


}

BEGIN_MESSAGE_MAP(CSettingsView, CFormView)
	ON_NOTIFY( UDN_DELTAPOS, IDC_SPIN_BACKUP_START, OnBackupStartSpin )
	ON_NOTIFY( UDN_DELTAPOS, IDC_SPIN_BACKUP_END, OnBackupEndSpin )
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnBnClickedButtonApply)
END_MESSAGE_MAP()


// CSettingsView diagnostics

#ifdef _DEBUG
void CSettingsView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSettingsView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CSettingsDoc * CSettingsView::GetDocument() // debug version
{
	ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CSettingsDoc ) ) );
	return (CSettingsDoc*)m_pDocument;
}

#endif //_DEBUG


// CSettingsView message handlers

void CSettingsView::OnInitialUpdate( )
{
	CFormView::OnInitialUpdate();
	if(!m_bInitialized )
	{
		m_bInitialized = true;

		Init( IDC_TAB_CINEMA, IDC_TAB_ROOM, this, this );

		m_LayoutManager.Attach(this);
		m_LayoutManager.AddAllChildren();
		m_LayoutManager.SetConstraint(IDC_TAB_CINEMA,		OX_LMS_RIGHT, OX_LMT_SAME, -2);
		m_LayoutManager.SetConstraint(IDC_TAB_ROOM,			OX_LMS_RIGHT, OX_LMT_SAME, -2);
		m_LayoutManager.RedrawLayout();

		m_spinTimeAfter.SetBuddy( &m_edTimeAfter );
		m_spinTimeBefore.SetBuddy( &m_edTimeBefore );
		m_spinTimePeriod.SetBuddy( &m_edTimePeriod );

		m_spinTimeAfter.SetRange32( 0, 30 );
		m_spinTimeBefore.SetRange32( 0, 30 );
		m_spinTimePeriod.SetRange32( 0, 30 );

		m_spinBackupEnd.SetBuddy( &m_edBackupEnd );
		m_spinBackupStart.SetBuddy( &m_edBackupStart );

		m_edBackupStart.SetDecimalDigitCount(2);
		m_edBackupStart.SetFractionalDigitCount(2);
		m_edBackupStart.SetNegativeFormat( _T("1.1") );
		m_edBackupStart.SetMask( _T("#%")  );

		m_edBackupEnd.SetDecimalDigitCount(2);
		m_edBackupEnd.SetFractionalDigitCount(2);
		m_edBackupEnd.SetNegativeFormat( _T("1.1") );
		m_edBackupEnd.SetMask( _T("#%")  );

		//m_spinBackupEnd.ModifyStyle( 0, UDS_SETBUDDYINT  );
		m_spinBackupStart.SetRange32( 0, 99);
		m_spinBackupEnd.SetRange32( 0, 99);

		m_spinBackupDate.SetRange32( 1, 365); // year is enough

		m_spinExcConnectNumber.SetBuddy	( &m_edExcConnectNumber );
		m_spinExcConnectTime.SetBuddy	( &m_edExcConnectTime );
		m_spinExcDelay.SetBuddy			( &m_edExcDelay );
		m_spinExcWaiting.SetBuddy		( &m_edExcWaiting );

		m_spinExcConnectNumber.SetRange32( 0, 1000 );
		m_spinExcConnectTime.SetRange32( 0, 1000 );
		m_spinExcDelay.SetRange32( 0, 1000 );
		m_spinExcWaiting.SetRange32( 0, 1000 );

		CWnd* pWnd = GetDlgItem( IDC_BUTTON_APPLY );
		ASSERT(pWnd);
		if(pWnd) pWnd->EnableWindow( GetCfgDB().IsUserAnAdmin() ? TRUE : FALSE  );

		ChangeRoomInControl();
	}
	SetScrollSizes( MM_TEXT, CSize(1, 1) );

	ModifyStyleEx(  WS_EX_CLIENTEDGE, 0 );
	ModifyStyle(  WS_BORDER, 0 );
}

BOOL	CSettingsView::Create( CWnd* pParent, const CRect & rc, CCreateContext& cc, int i  )
{
	return CFormView::Create( NULL,NULL,WS_CHILD | WS_VISIBLE | WS_BORDER, rc, pParent, AFX_IDW_PANE_FIRST+1 + i, &cc );
}

void CSettingsView::OnRoomChange(int nRoomID)
{
	UNREFERENCED_PARAMETER(nRoomID);
	ChangeRoomInControl( );
}

void CSettingsView::ChangeRoomInControl( )
try
{
	int nAfter, nBefore, nPeriod;
	int nRoomID = GetCurRoom();
	GetDB().GetTableRoomFacet(). GetWorkParam( nRoomID, nAfter, nBefore, nPeriod );

	CCinemaOleDB& db = dynamic_cast< CCinemaOleDB& >( GetDB() );
	int nConnectTime, nConnectNumber, nDelayTime, nWaitingTime;
	db.GetExchangeFacet().GetExchangeParam( nRoomID,  nConnectTime, nConnectNumber, 
											nWaitingTime, nDelayTime);
#ifdef USE_SEC
	m_spinTimeAfter.SetPos32(nAfter/60);
	m_spinTimeBefore.SetPos32(nBefore/60);
	m_spinTimePeriod.SetPos32(nPeriod/60);
#else
	m_spinTimeAfter.SetPos32(nAfter);
	m_spinTimeBefore.SetPos32(nBefore);
	m_spinTimePeriod.SetPos32(nPeriod);
#endif

	m_spinExcConnectTime.SetPos32( nConnectTime );
	m_spinExcConnectNumber.SetPos32( nConnectNumber);
	m_spinExcDelay.SetPos32( nDelayTime );
	m_spinExcWaiting.SetPos32( nWaitingTime );

	double fStart, fEnd;
	int		nDaysKeep;
	GetDB().GetTableRoomFacet().GetArchiveParam( nRoomID, fStart, fEnd, nDaysKeep );

	m_edBackupStart.SetValue( fStart );
	m_edBackupEnd.SetValue(fEnd);

	m_spinBackupDate.SetPos32(nDaysKeep);

	m_spinBackupStart.SetPos32( int(fStart) );
	m_spinBackupEnd.SetPos32( int(fEnd) );
}
catch( DataBaseException )
{
}	
catch( std::bad_cast )
{
}

void ProcessNumEdit(NMUPDOWN* pUDHdr, COXNumericEdit& ed, CSpinButtonCtrl& spin)
{
	double fStart = ed.GetValue();
	fStart = fabs( fStart );
	int nD = pUDHdr->iDelta;
	fStart += nD;
	int low, up;
	spin.GetRange32(low, up);
	if		( fStart >= up ) 	ed.SetValue( up );
	else if	( fStart <= low  )	ed.SetValue( low );
	else						ed.SetValue( fStart );
}

void CSettingsView::OnBackupStartSpin( NMHDR* pHdr, LRESULT * pResult )
{
	NMUPDOWN* pUDHdr = (NMUPDOWN*)pHdr;
	ProcessNumEdit( pUDHdr, m_edBackupStart, m_spinBackupStart );
	*pResult = TRUE;
}

void CSettingsView::OnBackupEndSpin( NMHDR* pHdr, LRESULT * pResult )
{
	NMUPDOWN* pUDHdr = (NMUPDOWN*)pHdr;
	ProcessNumEdit( pUDHdr, m_edBackupEnd, m_spinBackupEnd );
	*pResult = TRUE;
}
void CSettingsView::OnBnClickedButtonApply()
{
	// TODO: Add your control notification handler code here
#if 0
	UINT nStyle = 	
	(	CBalloonHelp::unCLOSE_ON_ANYTHING & ~  CBalloonHelp::unCLOSE_ON_MOUSE_MOVE)|
		CBalloonHelp::unCLOSE_ON_KEYPRESS |
	//	CBalloonHelp::unSHOW_INNER_SHADOW |
		CBalloonHelp::unSHOW_CLOSE_BUTTON
		;
#endif
	double fStart	= fabs ( m_edBackupStart.GetValue() );
	double fEnd		= fabs ( m_edBackupEnd.GetValue() );	

	// FIXME : user warning
	if( fStart > fEnd )
	{
		m_edBackupEnd.SetValue( fStart );
	}
	
	int nAfter, nBefore, nPeriod;
	int nDaysKeep;
	nAfter		= m_spinTimeAfter.GetPos32();
	nBefore		= m_spinTimeBefore.GetPos32();
	nPeriod		= m_spinTimePeriod.GetPos32();
	nDaysKeep	 = m_spinBackupDate.GetPos32();

	m_spinTimeAfter.SetPos32(nAfter);
	m_spinTimeBefore.SetPos32(nBefore);
	m_spinTimePeriod.SetPos32(nPeriod);
	m_spinBackupDate.SetPos32(nDaysKeep);

	int nConnectTime, nConnectNumber, nDelayTime, nWaitingTime;

	nConnectTime	= m_spinExcConnectTime.GetPos32();
	nConnectNumber	= m_spinExcConnectNumber.GetPos32();
	nDelayTime		= m_spinExcDelay.GetPos32();
	nWaitingTime	= m_spinExcWaiting.GetPos32();

	m_spinExcConnectTime.SetPos32( nConnectTime );
	m_spinExcConnectNumber.SetPos32( nConnectNumber);
	m_spinExcDelay.SetPos32( nDelayTime );
	m_spinExcWaiting.SetPos32( nWaitingTime );

#ifdef USE_SEC
	nAfter	*= 60;
	nBefore	*= 60;
	nPeriod	*= 60;
#endif

	int nRoomID = GetCurRoom();
	try
	{
		CCinemaOleDB*	db = dynamic_cast< CCinemaOleDB* >( &GetDB() );
		if( db && !db->CheckConnection())
		try
		{
				db->Open();
		}
		catch(DataBaseException)
		{
			AfxMessageBox( IDS_ERROR_CONNECTION, MB_OK|MB_ICONERROR );
			return;
		}
		GetDB().GetTableRoomFacet().SetWorkParam( nRoomID, nAfter, nBefore, nPeriod );
		GetDB().GetTableRoomFacet().SetArchiveParam( nRoomID, fStart, fEnd, nDaysKeep );
		db->GetExchangeFacet().SetExchangeParam( nRoomID, 
												 nConnectTime, nConnectNumber,
												 nWaitingTime, nDelayTime);
	}
	catch(DataBaseException)
	{
		CRect rc;
		GetDlgItem(IDC_BUTTON_APPLY)->GetWindowRect(&rc);
		AfxMessageBox( IDS_ERROR_DB_SAVING, MB_OK | MB_ICONERROR );
	}
}
