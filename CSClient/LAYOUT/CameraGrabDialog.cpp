// ameraGrabDialog.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "CameraGrabDialog.h"
//#include "../CSAlgo/CSAlgo.h"

const UINT Timer_Update		= 10;
const UINT Period_Update	= 200;

// CameraGrabDialog dialog

//IMPLEMENT_DYNAMIC(CameraGrabDialog, CDialog)
CameraGrabDialog::CameraGrabDialog(
								   SparceRectImageLayout& layout,   
								   CameraGrabDialog::SourceArr_t&	CamSrc,
								   CWnd* pParent /*=NULL*/)
	: CDialog(CameraGrabDialog::IDD, pParent),
		m_layout( layout ),
		m_CamSrc(CamSrc),
		m_fInit(false),
		m_lFrames(0),
		m_wndLayout(this, false)
{
	ASSERT( m_layout.GetCells().size() == m_CamSrc.size() );
	for( size_t i = 0; i < m_CamSrc.size() ; ++i )
	{
		m_Background.push_back( CFrameBackground( m_CamSrc[i] ) ) ;
	}
}

CameraGrabDialog::~CameraGrabDialog()
{
}

void CameraGrabDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CameraGrabDialog, CDialog)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_WM_SIZE()
	ON_WM_TIMER()
//	ON_WM_PAINT()
END_MESSAGE_MAP()


// CameraGrabDialog message handlers

void CameraGrabDialog::OnStop()
{
	// TODO: Add your control notification handler code here
	SetEvent( m_Sync.m_hEvent );

	CWnd* pWnd = GetDlgItem(IDC_LAYOUT);
	if(!pWnd) return;
	pWnd = GetDlgItem(IDC_STOP);
	if(pWnd) pWnd->EnableWindow( FALSE );
	pWnd = GetDlgItem(IDOK );
	if(pWnd) pWnd->EnableWindow( TRUE );
	//OnOK();
}

void CameraGrabDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

void CameraGrabDialog::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	m_bUpdate	= false;
	m_lFrames	= 0;
	m_Images.clear();
	m_Images.resize( m_CamSrc.size() );
	m_timeStart = CTime::GetCurrentTime();
	CWnd* pWnd = GetDlgItem(IDC_STOP);
	if(pWnd) pWnd->EnableWindow( TRUE );
	pWnd = GetDlgItem(IDOK );
	if(pWnd) pWnd->EnableWindow( FALSE );
	HANDLE h = (HANDLE)_beginthreadex( 0, 0, ThreadProc, this, 0, NULL  );
	m_Sync.Start( h );
	//CDialog::OnOK();
}

void CameraGrabDialog::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	m_Sync.Stop( 1000 );
	CDialog::OnCancel();
}

void CameraGrabDialog::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if( nIDEvent == Timer_Update )
	{
		if(m_bUpdate) m_wndLayout.Invalidate();
		else
		{
			DWORD res = WaitForSingleObject( m_Sync.m_hThread, 50 );
			if( res == WAIT_OBJECT_0 )
			{
				CDialog::OnOK();
			}
			else
			{
				SetFrameAndTime();
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CameraGrabDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if( m_fInit ) return  TRUE;
	else m_fInit = true;

	CWnd *pWnd = GetDlgItem( IDC_LAYOUT );
	ASSERT(pWnd);
	CRect rcLayout;
	pWnd->GetWindowRect(&rcLayout);
	ScreenToClient( &rcLayout );
	pWnd->DestroyWindow();
	m_wndLayout.Create( 0, 0, WS_CHILD|WS_VISIBLE, rcLayout, this, IDC_LAYOUT );

	m_LayoutManager.Attach( this );
	m_LayoutManager.AddAllChildren();

	m_LayoutManager.SetConstraint(IDC_LAYOUT, OX_LMS_RIGHT, OX_LMT_SAME, -2);
	m_LayoutManager.SetConstraint(IDC_LAYOUT, OX_LMS_BOTTOM, OX_LMT_SAME, -2);
	m_LayoutManager.RedrawLayout();

	m_bUpdate = true;
	SetTimer( Timer_Update, Period_Update, 0 );

	GetDlgItem( IDC_STOP )->EnableWindow( FALSE );
	// FIXME
	m_ImageDimension = CSize(640, 480);
	
	std::vector< CLayoutWnd::Coord > c;
	c.reserve( m_Background.size() );
	const SparceRectImageLayout::IndexArray_t IndArr = m_layout.GetCells();
	for( size_t i =0; i < IndArr.size() ; ++i )
	{
		c.push_back( CLayoutWnd::Coord( IndArr[i].m_nX, IndArr[i].m_nY, i ) );
	}
	m_wndLayout.SetResultLayout( c );

	CRect rc;
	GetClientRect(&rc);
	OnSize( SIZE_RESTORED, rc.Width(), rc.Height() ); 
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

UINT	CameraGrabDialog::ThreadProc( void * pParam )
{
	CameraGrabDialog* pThis = (CameraGrabDialog*)pParam;
	CoInitializeEx( 0, COINIT_MULTITHREADED );

	std::vector< boost::shared_ptr<IBackgroundAccumulator> > Accum;
	for(DWORD i=0; i < pThis-> m_CamSrc.size(); ++i)
	{
		IBackgroundAccumulator* pAcc;
		HRESULT hr = CreateBackgroundAccumulator( &pAcc );
		if( hr != S_OK )
		{
			//FIXME : to resources
			//AfxMessageBox(  );
				return 1;
		}
		hr = pAcc->Start();
		boost::shared_ptr<IBackgroundAccumulator> p( pAcc, ReleaseInterface<IBackgroundAccumulator> () );
		Accum.push_back( p );
	}

	const BYTE*			pImage;
	const BITMAPINFO*	pHeader	;
	int					imgSize;

	HRESULT hr;
	while( pThis->m_Sync.CanThreadWork( 100 ) )
	{
		bool bOK = true;
		for( size_t i = 0; i < pThis->m_CamSrc.size(); ++i )
		{
			hr = pThis->m_CamSrc[i]->LockFrame( pHeader, pImage, imgSize ) ;
			if(hr == S_OK)
			{
				hr	= Accum[i]->ProcessImage( pHeader, pImage );
				hr	= pThis->m_CamSrc[i]->UnlockFrame();
			}
			else
				bOK = false;
		}
		if( bOK ) InterlockedIncrement( &pThis->m_lFrames );
	}

	for(size_t i=0; i < Accum.size(); ++i)
	{
		const Arr2f* pArr;
		hr = Accum[i]->Stop(&pArr);
		pThis->m_Images[i] = *pArr;
	}
	Accum.clear();

	CoUninitialize();
	return 0;
}

void	CameraGrabDialog::SetFrameAndTime()
{
	CTimeSpan ts = CTime::GetCurrentTime() - m_timeStart;
	CString s;
	s.Format( _T("%d"), ts.GetTotalSeconds() );
	SetDlgItemText( IDC_STATIC_TIMEVALUE, s );
	s.Format( _T("%d"), m_lFrames );
	SetDlgItemText( IDC_STATIC_FRAMEVALUE, s );
}

void	CameraGrabDialog::DrawItem( Gdiplus::Graphics& gr, CRect rc, DWORD_PTR dwCookie )
{
	ASSERT( dwCookie < m_Background.size() );
	m_Background[ dwCookie ].Draw( gr, rc );
}