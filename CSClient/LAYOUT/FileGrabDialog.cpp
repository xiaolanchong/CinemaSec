// Layout\FileGrabDialog.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "Layout\FileGrabDialog.h"
#include "..\VIDEO\CameraManager.h"

const UINT Timer_Update		= 10;
const UINT Period_Update	= 200;
// CFileGrabDialog dialog

//IMPLEMENT_DYNAMIC(CFileGrabDialog, CDialog)

CFileGrabDialog::CFileGrabDialog(
	 SparceRectImageLayout& layout, const std::vector<int>& IdArr,
		CWnd* pParent /*=NULL*/)
	:	CLayerHelperDialog(CFileGrabDialog::IDD, true, pParent),
		m_layout( layout ),
		m_IdArr( IdArr ),
		m_nCurrentInd(~0)
{
	ASSERT( m_layout.GetCells().size() == m_IdArr.size() );
}

CFileGrabDialog::~CFileGrabDialog()
{
}

void CFileGrabDialog::DoDataExchange(CDataExchange* pDX)
{
	CLayerHelperDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_GRAB, m_wndProgress);
}


BEGIN_MESSAGE_MAP(CFileGrabDialog, CLayerHelperDialog)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CFileGrabDialog message handlers

void CFileGrabDialog::OnStop()
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

void CFileGrabDialog::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	bool bAllIsSelected = std::find ( m_Background.begin(), m_Background.end(), boost::shared_ptr<CBackground>() ) == m_Background.end();
	if( !bAllIsSelected )
	{
		AfxMessageBox( IDS_ERROR_ITEM, MB_OK|MB_ICONERROR );
		// quite until a user'll select all cells;
		return;
	}

	m_lFrames	= 0;
	m_Images.clear();
	m_Images.resize( m_IdArr.size() );
	m_wndProgress.SetRange( 0, 100 );
	CWnd* pWnd = GetDlgItem(IDC_STOP);
	if(pWnd) pWnd->EnableWindow( TRUE );
	pWnd = GetDlgItem(IDOK );
	if(pWnd) pWnd->EnableWindow( FALSE );
	SetTimer( Timer_Update, Period_Update, 0 );
	HANDLE h = (HANDLE)_beginthreadex( 0, 0, ThreadProc, this, 0, NULL  );
	m_Sync.Start( h );
	//CDialog::OnOK();
}

void CFileGrabDialog::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	m_Sync.Stop( 1000 );
	CLayerHelperDialog::OnCancel();
}

void CFileGrabDialog::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if( nIDEvent == Timer_Update )
	{
		DWORD res = WaitForSingleObject( m_Sync.m_hThread, 50 );
		if( res == WAIT_OBJECT_0 )
		{
			CDialog::OnOK();
		}
		else
		{
			SetFrame();
		}
	}

	CLayerHelperDialog::OnTimer(nIDEvent);
}

void CFileGrabDialog::OnInitDialogOnce()
{
	CLayerHelperDialog::OnInitDialogOnce();

	// TODO:  Add extra initialization here


	GetDlgItem( IDC_STOP )->EnableWindow( FALSE );
	// FIXME
	m_ImageDimension = CSize(640, 480);

	std::vector< CLayoutWnd::Coord > c;
	m_Background.resize( m_IdArr.size() );
	c.reserve(/* m_Background.size()*/m_IdArr.size() );
//	m_FileSrc.resize( m_IdArr.size() );
	m_FileNames.resize( m_IdArr.size() );
	const SparceRectImageLayout::IndexArray_t IndArr = m_layout.GetCells();
	for( size_t i =0; i < IndArr.size() ; ++i )
	{
		c.push_back( CLayoutWnd::Coord( IndArr[i].m_nX, IndArr[i].m_nY, i ) );
	}
	m_wndLayout.SetResultLayout( c );

	CRect rc;
	GetClientRect(&rc);
	OnSize( SIZE_RESTORED, rc.Width(), rc.Height() ); 
}

void	CFileGrabDialog::SetFrame()
{
	m_wndProgress.SetPos( m_lFrames );
}

void		CFileGrabDialog::DrawItem( Gdiplus::Graphics& gr, CRect rc, DWORD_PTR dwCookie )
{
	ASSERT( m_IdArr.size() > dwCookie );
//	m_IdArr[ dwCookie ].Draw( gr, CRectF(rc) );
	CString s;
	int nCameraID = m_IdArr[ dwCookie ];
	if( m_Background [dwCookie].get() )
	{
		m_Background [dwCookie]->Draw( gr, CRectF(rc) );
	}
	//FIXME : resources
	s.Format( _T("Camera %d"), nCameraID );
	DrawHeader( gr, CRectF(rc), s );
}

bool	CFileGrabDialog::GetItemMenuContent( std::vector<CString> &  MenuItems, WORD x, WORD y, bool& bCanDeleted   )
{
	//FIXME : resources
	MenuItems.push_back( CString(_T("Video file...")) );

	INT_PTR nOffset = -1;
	bCanDeleted = false;
	const SparceRectImageLayout::IndexArray_t& IndArr = m_layout.GetCells();
	for( size_t i =0; i <  IndArr.size(); ++i )
	{
		if( IndArr[i].m_nX == x  &&
			IndArr[i].m_nY == y )
		{
			nOffset = i;
			break;
		}
	}
	if( nOffset == -1 ) return false;
	else
	{
		m_nCurrentInd = nOffset;
		return true;
	}
}

std::pair<bool, DWORD_PTR>	CFileGrabDialog::SelectMenuItem( size_t nIndex  )
{
//FIXME : resources
	CString sVideo; sVideo.LoadString( IDS_FILTER_AVI );
	CFileDialog dlg( TRUE, _T("*.avi"), 0, 0, sVideo, 0 );
	ASSERT( m_nCurrentInd >= 0 && m_nCurrentInd < m_IdArr.size() );
	int nCameraID = m_IdArr[ m_nCurrentInd ];
	if( dlg.DoModal() == IDOK )
	{
		CString sPath = dlg.GetPathName();
		boost::shared_ptr<IBaseGrabber> pBase = GetCameraManager().CreateGrabber( nCameraID, sPath, 24, false );
		IFileGrabber* pFileInt = dynamic_cast<IFileGrabber*> ( pBase.get() );
		if( !pFileInt ) 
		{
			AfxMessageBox( IDS_ERROR_VIDEOIMAGE, MB_OK|MB_ICONERROR );
			return std::make_pair( false ,0);
		}
//		m_FileSrc[m_nCurrentInd] = pBase;
		m_FileNames[ m_nCurrentInd ] = dlg.GetPathName();
		const BITMAPINFO*	bi		= 0;
		const BYTE*			pBytes	= 0;
		int					nImgSize;
		int nLockNumber  = 10;
		while ( nLockNumber )
		{
			HRESULT hr = pFileInt->LockFrame( bi, pBytes, nImgSize );
			if( hr == S_OK )
				break;
			Sleep(300);
			--nLockNumber;
		}
		if( !nLockNumber )
		{
			AfxMessageBox( IDS_ERROR_VIDEOIMAGE, MB_OK|MB_ICONERROR );
			return std::make_pair( false ,0);
		}
		boost::shared_ptr<CBitmapBackground> BmpBg ( new CBitmapBackground( bi, pBytes ) );
		pFileInt->UnlockFrame( );
		m_Background[ m_nCurrentInd ] = BmpBg;
		//m_wndLayout.Invalidate();
		return std::make_pair( true, m_nCurrentInd );
	}
	else
		return std::make_pair( false ,0);
}

void	CFileGrabDialog::DeleteItem( DWORD_PTR dwCookie )
{

}

UINT	CFileGrabDialog::ThreadProc( void * pParam )
{
	CFileGrabDialog* pThis = (CFileGrabDialog*)pParam;
	CoInitializeEx( 0, COINIT_MULTITHREADED );

	FileSrc_t	FileSrc( pThis->m_FileNames.size() );

	for( size_t i =0 ; i < pThis->m_FileNames.size(); ++i )
	{
		CString sPath = pThis->m_FileNames[i];
		boost::shared_ptr<IBaseGrabber> pBase = GetCameraManager().CreateGrabber( -1, sPath, MY_BI_YUY2, false );
		FileSrc[i] = pBase;
	}

	std::vector<IFileGrabber* > FileGrabbers ( pThis->m_FileNames.size(), 0) ;
	for( size_t i = 0; i < FileSrc.size(); ++i )
	{
		IFileGrabber* pFg = dynamic_cast< IFileGrabber* > ( FileSrc[i].get() ) ;
		ASSERT(pFg);
		FileGrabbers[i] = pFg;
	}

	std::vector< boost::shared_ptr<IBackgroundAccumulator> > Accum;
	for(DWORD i=0; i < FileGrabbers.size(); ++i)
	{
		IBackgroundAccumulator* pAcc;
		HRESULT hr = CreateBackgroundAccumulator( &pAcc );
		if( hr != S_OK )
		{
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
	LONG				nLockNumbers = 0;
	while( pThis->m_Sync.CanThreadWork( 100 ) )
	{	
		bool bOK = false;
		double fProgress = 1.0f;
		for( size_t i=0; i < FileGrabbers.size(); ++i )
		{
			float pos ;
			INT64 nCurFrame, nTotalFrame;
			hr = FileGrabbers[i]->GetCurFrame( nCurFrame ) ;
			ASSERT( hr == S_OK );
			hr = FileGrabbers[i]->GetTotalFrames( nTotalFrame ) ;
			ASSERT( hr == S_OK );
			fProgress = std::_cpp_min<double>( double( nCurFrame) /nTotalFrame , fProgress );

			if( nTotalFrame > nCurFrame )
			{
				hr = FileGrabbers[i]->LockFrame( pHeader, pImage, imgSize ) ;
				if(hr == S_OK)
				{
					hr	= Accum[i]->ProcessImage( pHeader, pImage );
					hr = FileGrabbers[i]->UnlockFrame();
					bOK = true;
				}
			}
		}
		if( bOK ) 
		{
			InterlockedExchange( &pThis->m_lFrames,   LONG(100 * fProgress) );
			++nLockNumbers;
		}
		else
			break;
	}

	TRACE1( "FileGrabbing::Number of lock=%d", nLockNumbers );
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