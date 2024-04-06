// ArrangeView.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "ArrangeView.h"
#include "ArrangeDocument.h"
//#include "DBFacet/DBSerializer.h"
#include "DBFacet/CfgDB.h"
#include "../Background/EmptyBackground.h"
#include "../Layout/LayoutDialog.h"
#include "../Layout/LayoutValidate.h"
#include "VideoRender/ArrayRender.h"
#include "VideoRender/MergeRender.h"
#include "VideoRender/MixedRender.h"

#ifdef DEBUG_TRANSFER
std::auto_ptr<CFileTransferDialog>	CArrangeView::m_pdlgTransfer;
#endif	

// CArrangeView

IMPLEMENT_DYNCREATE(CArrangeView, CView)

CArrangeView::CArrangeView():
	m_bAutoUpdate(false),
	m_bInCreate(false),
	m_bUseMergeRender(true),
	m_bDisableAction(true),
	m_bShowCameraID(false)
{
}

CArrangeView::~CArrangeView()
{
}

BEGIN_MESSAGE_MAP(CArrangeView, CView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CArrangeView drawing

CArrangeDocument* CArrangeView::GetDocument()
{
	return dynamic_cast<CArrangeDocument*>( m_pDocument );
}

// CArrangeView diagnostics

#ifdef _DEBUG
void CArrangeView::AssertValid() const
{
	CView::AssertValid();
}

void CArrangeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

// CArrangeView message handlers

//static boost::shared_ptr< IVideoRender > m_pRender;
static bool bSignalled = false;

void CArrangeView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	ModifyStyleEx(  WS_EX_CLIENTEDGE, 0 );
	ModifyStyle(  WS_BORDER, 0 );

	bool bOldSignalled = bSignalled;

	if( !m_pRender && !m_bInCreate)
	{
		m_bInCreate = true;

		NONCLIENTMETRICS ncm;
		ncm.cbSize =  sizeof(NONCLIENTMETRICS);
		SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( NONCLIENTMETRICS), &ncm, 0  );
		ncm.lfCaptionFont.lfUnderline = TRUE;
		m_fntRender.CreateFontIndirect( &ncm.lfCaptionFont );
		SetFont( &m_fntRender );

		CDC* pDC = GetDC();
		pDC->SelectObject( &m_fntRender );
		ReleaseDC( pDC ); 

		m_pArrayRender = boost::shared_ptr< IVideoRender >( new ArrayRender(this) );
		m_pRender = m_pArrayRender;
		if( GetCfgDB().UseDB() )
			try
			{
	#if 0
				m_pMergeRender = boost::shared_ptr< IVideoRender >( new MergeRender( this , this) );
	#else
				m_pMergeRender = boost::shared_ptr< IVideoRender >( new MixedRender( this ) );
	#endif
				m_pRender = m_pMergeRender;
			}
			catch( RenderCreateException )
			{
				bSignalled = true;
				if( !bOldSignalled )
					AfxMessageBox( IDS_ERROR_CVMCADCOMPONENT, MB_OK|MB_ICONERROR );
			}
			catch( RenderInvalidDeviceException )
			{
				bSignalled = true;
				m_pRender = boost::shared_ptr< IVideoRender >( new ArrayRender(this) );
				if( !bOldSignalled )
					AfxMessageBox( IDS_ERROR_D3DDEVICE, MB_OK|MB_ICONERROR );
			}
	}
}

BOOL CArrangeView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	UNREFERENCED_PARAMETER(pDC);
	return TRUE;
}

void CArrangeView::OnDraw(CDC* pDC)
{
	// TODO: add draw code here
	SetWindowHeader();
	if( !m_bDisableAction )
		m_pRender->Render( pDC );
}

void CArrangeView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	if( m_pRender.get() && !m_bDisableAction )
		m_pRender->Resize( cx, cy );
}



void CArrangeView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch( nIDEvent )
	{
	case IDT_FrameBackground:
		{
			if( m_bAutoUpdate )
				Invalidate();
		}
		break;
	}
	CView::OnTimer(nIDEvent);
}

int CArrangeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	ModifyStyle( 0, WS_CLIPCHILDREN );
	SetTimer(  IDT_FrameBackground, Period_FrameBackground, 0);
	return 0;
}

bool	CArrangeView::CanTakeScreenShot()
{
	return m_pRender->CanTakeScreenShot();
}

static bool GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return false;  // Failure

	BYTE * RawArray = new BYTE[size];
	if(RawArray == NULL)
		return false;  // Failure
	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)RawArray;

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			delete [] RawArray;
			return true;  // Success
		}    
	}
	delete [] RawArray;
	return false;  // Failure
}

MACRO_EXCEPTION(ImageException, CommonException);

boost::shared_ptr<Gdiplus::Image>	CArrangeView::TakeScreenShot( )
{
	return m_pRender->TakeScreenShot();
}

void	CArrangeView::SaveScreenShot( LPCWSTR szFileName, Gdiplus::Image* pBmp )
{
	Gdiplus::EncoderParameters m_encoderParameters;
	LONG	m_lQuality;
	CLSID	m_clsidEncoder;

	bool res = GetEncoderClsid(L"image/jpeg", &m_clsidEncoder);
	if( !res )
	{
		throw ImageException("Failed to get a suitable encoder");
	}

	m_lQuality = 85; // % from possible quality
	m_encoderParameters.Count = 1;
	m_encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
	m_encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
	m_encoderParameters.Parameter[0].NumberOfValues = 1;
	m_encoderParameters.Parameter[0].Value = &m_lQuality;

	Gdiplus::Status Stat = pBmp->Save( szFileName, &m_clsidEncoder, &m_encoderParameters );
	if( Stat != Gdiplus::Ok )
	{
		throw ImageException("Failed to save");
	}
}

LRESULT	CArrangeView::OnRVideoClick(WPARAM , LPARAM )
{
	const UINT_PTR Item_ScreenShot	= 1;
	const UINT_PTR Item_Array		= 2;
	const UINT_PTR Item_Merge		= 3;
	const UINT_PTR Item_ID			= 4;


	CMenu menuShot;
	menuShot.CreatePopupMenu();
	CArrangeDocument* pDoc = GetDocument();
	
	UINT nStyle = CanTakeScreenShot() ? MF_ENABLED :  MF_GRAYED;

	CString sMenuItem; 
	sMenuItem.LoadString( IDS_SCREENSHOT );
	menuShot.AppendMenu( nStyle |MF_STRING, Item_ScreenShot,	sMenuItem ) ;
	sMenuItem.LoadString( IDS_RENDER_ARRAY );
	menuShot.AppendMenu( (m_bUseMergeRender ? MF_UNCHECKED : MF_CHECKED)|MF_STRING, Item_Array, sMenuItem );
	sMenuItem.LoadString( IDS_RENDER_MERGE );
	menuShot.AppendMenu( (m_bUseMergeRender ? MF_CHECKED : MF_UNCHECKED)|MF_STRING, Item_Merge, sMenuItem );

	sMenuItem.LoadString( IDS_RENDER_ID );
	menuShot.AppendMenu( (m_bShowCameraID ? MF_CHECKED : MF_UNCHECKED)|MF_STRING, Item_ID, sMenuItem );

	AddContextMenuItem( menuShot );

	CPoint pt;
	GetCursorPos( &pt );
	UINT nID = menuShot.TrackPopupMenuEx( TPM_RETURNCMD|TPM_NONOTIFY, pt.x, pt.y, this , 0 );
	switch( nID )
	{
	case Item_ScreenShot:
	{
		boost::shared_ptr<Gdiplus::Image> Bmp = TakeScreenShot();

		if( !Bmp )
		{
			AfxMessageBox( IDS_ERROR_SCREENSHOT, MB_OK|MB_ICONERROR );
		}
		else
		{
			CString sFilter;
			sFilter.LoadString( IDS_FILTER_IMAGE );
			UINT nStyle = OFN_ENABLESIZING | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_EXPLORER;
			CFileDialog dlg( FALSE, _T("jpg"), 0,  nStyle, sFilter );
			if(dlg.DoModal() == IDOK)
			{
				try
				{
				
				SaveScreenShot(  dlg.GetPathName(), Bmp.get() );
				}
				catch( ImageException )
				{
					AfxMessageBox( IDS_ERROR_SCREENSHOT, MB_OK|MB_ICONERROR );
				}
			}
		}
		break;
	}
	case Item_Array:
		m_bUseMergeRender = false;
		pDoc->UpdateRoom();
		break;
	case Item_Merge:
		m_bUseMergeRender = true;
		pDoc->UpdateRoom();
		break;
	case Item_ID:
		m_bShowCameraID = !m_bShowCameraID;
		m_pRender->ShowID( m_bShowCameraID );
		Invalidate();
		break;

	default:
		ProcessContextMenuItem( nID );
	}
	return TRUE;
}

void CArrangeView::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( !m_bDisableAction )
	m_pRender->OnLButtonDblClk( point );
	CView::OnLButtonDblClk( nFlags, point );
}

void CArrangeView::OnRButtonDown( UINT nFlags, CPoint point )
{
	OnRVideoClick(0, 0);
	CView::OnRButtonDown( nFlags, point );
}

////////////// delegate to render ///////////////

void	CArrangeView::LoadRoom( int nRoomID, const WindowArray_t& wa )
{
	if( m_bDisableAction ) return;
	try
	{
		if( m_pMergeRender.get() && m_bUseMergeRender )
		{
			m_pMergeRender->LoadRoom( nRoomID, wa );
			m_pRender = m_pMergeRender;
		}
		else
		{
			m_pRender = m_pArrayRender;
			m_pRender->LoadRoom( nRoomID, wa );
		}
	}
	catch(RenderLoadModelException)
	{
		m_bUseMergeRender = false;
		m_pRender = m_pArrayRender;
		m_pRender->LoadRoom( nRoomID, wa );
	}

#ifdef DEBUG_TRANSFER
	m_nCachedRoomID = nRoomID;
#endif

	if( m_pMergeRender.get() ) m_pMergeRender->ShowID( m_bShowCameraID );
	if( m_pArrayRender.get() ) m_pArrayRender->ShowID( m_bShowCameraID );
	PostLoadRoom();

	CRect rc;
	GetClientRect( &rc );
	OnSize( SIZE_RESTORED, rc.Width(), rc.Height() );
}


void	CArrangeView::UnloadRoom()
{
	m_pRender->UnloadRoom();
}

void	CArrangeView::ClearImage()
{
	m_pRender->ClearImage();
	Invalidate();
}

void	CArrangeView::SetImage( int nID, const std::vector<BYTE>& ImageArr)
{
	m_pRender->SetImage( nID, ImageArr );
	Invalidate();
}

void	CArrangeView::SetImage( int nID, const BITMAPINFO* pBmpInfo, const void* pBits)
{
	m_pRender->SetImage( nID, pBmpInfo, pBits );
	Invalidate();
}

void	CArrangeView::SetCameraSource( int nID, int nCameraID)
{
	m_pRender->SetCameraSource( nID, nCameraID );
	Invalidate();
}

void	CArrangeView::SetVideoSource( int nID, const std::wstring& sSource )
{
	m_pRender->SetVideoSource( nID, sSource );
	Invalidate();
}

void	CArrangeView::SetWindowHeader(  )
{
	CArrangeDocument* pDoc = GetDocument();
	CString sHeader = pDoc->GetHeaderText( );

	NONCLIENTMETRICSW ncm;
	ncm.cbSize =  sizeof(NONCLIENTMETRICSW);
	SystemParametersInfoW( SPI_GETNONCLIENTMETRICS, sizeof( NONCLIENTMETRICSW), &ncm, 0  );
	const LOGFONTW& lf = ncm.lfCaptionFont;

	m_pRender->SetWindowHeader( lf, sHeader );
//	Invalidate();
}

void CArrangeView::OnDestroy()
{
	if( m_pArrayRender.get() )
		m_pArrayRender->Destroy();
	if( m_pMergeRender.get() )
		m_pMergeRender->Destroy();
	CView::OnDestroy();
}