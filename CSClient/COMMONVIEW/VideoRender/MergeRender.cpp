//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	CVMCad component render
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 20.06.2005
//                                                                                      //
//======================================================================================//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "MergeRender.h"
#include "../../../CSChair/debug_int.h"
#include "../../DBFacet/DBSerializer.h"
#include "../../Video/CameraManager.h"
#include <strmif.h>
#include <amvideo.h>
//======================================================================================//
//                                  class MergeRender                                   //
//======================================================================================//

boost::shared_ptr< IRoomRender >		MergeRender::m_pRender;
LONG									MergeRender::m_nRenderRef;
//CStatic									MergeRender::m_wndRenderTarget;

MergeRender::MergeRender( CWnd* pCreateWnd, CWnd* pWnd) :
	IVideoRender(  pWnd  ) 
{
	if( !m_pRender ) 
	{
		InterlockedExchange( &m_nRenderRef, 1 ); 
		IRoomRender* pRender;
		HRESULT hr = ::CreateRender( pCreateWnd->GetSafeHwnd(), &pRender );
		if( FAILED(hr) )
		{
			throw RenderCreateException( "Failed to create IRoomRender interface" );
		}
		if( pRender->TestComponent() != S_OK )
		{
			pRender->Release();
			throw RenderInvalidDeviceException( "Invalid device" );
		}
		m_pRender = boost::shared_ptr< IRoomRender >( pRender, ReleaseInterface<IRoomRender>() );
	}
	else
	{
		InterlockedIncrement( &m_nRenderRef );
	}
}

MergeRender::~MergeRender()
{
	if( m_pRender ) m_pRender.reset();
}

std::pair<int, int>	MergeRender::GetPosFromID( int nID )
{
	PosMap_t::const_iterator it = m_IdPositionMap.find( nID );
	ASSERT( it != m_IdPositionMap.end() );

	return std::make_pair( it->second.first, it->second.second );
}

void	MergeRender::LoadRoom( int nRoomID, const WindowArray_t& wa )	
{
//	m_wndRenderTarget.SetParent( m_pWnd );
	std::vector<BYTE> ModelData;
	UnloadRoom();
	try
	{
		GetDB().GetTableRoomFacet().GetMergeModel( nRoomID, ModelData );
	}
	catch( CommonException )
	{
//		ASSERT( FALSE );
		throw RenderLoadModelException("Failed to load model");
	}
#if 0 // for test failure
	throw RenderLoadModelException("Failed to load model");
#endif
	HRESULT hr;
	hr = m_pRender->Load( &ModelData[0], ModelData.size() );
	ASSERT( hr == S_OK );
		
	m_nActiveVideo = INVALID_ACTIVE_VIDEO;
	for(size_t i = 0; i <wa.size(); ++i )
	{
		m_IdPositionMap.insert( std::make_pair( wa[i].m_nID, std::make_pair( wa[i].m_X, wa[i].m_Y ) ) );
	}

	CRect rc;
	m_pWnd->GetClientRect( &rc );
	m_pRender->Resize( m_pWnd->GetSafeHwnd(), rc.Width(), rc.Height() );
}

static IStream* CreateStream( const void* pBytes, size_t nSize)
{
	HGLOBAL hMem = GlobalAlloc( GMEM_FIXED, nSize );
	void* pLock = GlobalLock( hMem );
	memcpy( pLock, pBytes, nSize );
	GlobalUnlock( hMem );
	IStream* pStream;
	CreateStreamOnHGlobal( hMem, TRUE, &pStream );
	return pStream;
}	

void	MergeRender::SetImage( int nID, const std::vector<BYTE>& ImageArr)
{
	std::pair<int, int>	p = GetPosFromID( nID );

	int x = p.first, y = p.second;

	IStream * pStream = CreateStream( &ImageArr[0], ImageArr.size() );
	ASSERT(pStream);
	Gdiplus::Bitmap img(pStream, FALSE );

	Gdiplus::BitmapData BmpData;
	Gdiplus::Rect rcImage( 0, 0, img.GetWidth(), img.GetHeight() );
	Gdiplus::Status st = img.LockBits(  &rcImage, 
										Gdiplus::ImageLockModeRead, 
										PixelFormat24bppRGB,  
										&BmpData );

	ASSERT( st == Gdiplus::Ok );
	ASSERT( BmpData.PixelFormat == PixelFormat24bppRGB );

	int nLineSize = 3 * BmpData.Width;

	BITMAPINFO			bi;
	BITMAPINFOHEADER&	bmh = bi.bmiHeader;
	memset( &bmh, 0, sizeof(BITMAPINFOHEADER) );
	bmh.biBitCount			= 24;
	bmh.biWidth				= BmpData.Width;
	bmh.biHeight			= BmpData.Height;
	bmh.biSize				= sizeof(BITMAPINFOHEADER);
	bmh.biSizeImage			= DIBSIZE(bmh);


	HRESULT hr;

	std::vector<BYTE> TmpImg; 
	TmpImg.resize(bmh.biSizeImage );

	BYTE		* pDst = &TmpImg[0] + nLineSize * (BmpData.Height - 1);
	const BYTE	* pSrc = (const BYTE*)BmpData.Scan0;
	for( size_t i = 0; i < BmpData.Height ; pSrc += BmpData.Stride, pDst -= nLineSize, ++i  )
	{
		memcpy( pDst, pSrc, nLineSize );
	}
	hr = m_pRender->SetImage( x, y, &bi, &TmpImg[0] );

	ASSERT(hr == S_OK);
	img.UnlockBits( &BmpData );
	pStream->Release();
}

void	MergeRender::SetImage( int nID, const BITMAPINFO* pBmpInfo, const void* pBits)
{
	std::pair<int, int>	p = GetPosFromID( nID );
	m_pRender->SetImage( p.first, p.second, pBmpInfo, (const BYTE*)pBits );
}

void	MergeRender::SetCameraSource( int nID, int nCameraID)
{
	std::pair<int, int>	p = GetPosFromID( nID );
	try
	{
		IBaseGrabber* pGrb = GetCameraManager().CreateGrabber( nCameraID );
		if( pGrb )
		{
			boost::shared_ptr<IBaseGrabber> Tmp( pGrb, ReleaseInterface<IBaseGrabber>() );	
			m_GrabberMap.push_back( std::make_pair( nID, Tmp ) );
		}
		else
		{
			m_pRender->SetImage( p.first, p.second, NULL, NULL );
		}
	}
	catch( CommonException& )
	{	
		m_pRender->SetImage( p.first, p.second, NULL, NULL );
	}
}

void	MergeRender::SetVideoSource( int nID, const std::wstring& sSource )
{
	std::pair<int, int>	p = GetPosFromID( nID );
	try
	{
		IBaseGrabber* pGrb = GetCameraManager().CreateFileGrabber( sSource.c_str() );
		if( pGrb )
		{
			boost::shared_ptr<IBaseGrabber> Tmp( pGrb, ReleaseInterface<IBaseGrabber>() );	
			m_GrabberMap.push_back( std::make_pair( nID, Tmp ) );
		}
		else
		{
			m_pRender->SetImage( p.first, p.second, NULL, NULL );
		}
	}
	catch( CommonException& )
	{	
		m_pRender->SetImage( p.first, p.second, NULL, NULL );
	}
}

void	MergeRender::SetWindowHeader( const LOGFONTW& lf, CString sHeader  )
{
	m_lf = lf;
	HRESULT hr;
	hr = m_pRender->SetText( &lf, (LPCWSTR)sHeader, RGB(255, 255, 255) );
	ASSERT(hr == S_OK);
}

void	MergeRender::ClearImage()
{
	MergeRender::SetDefaultState();
}

void	MergeRender::UnloadRoom()
{
	ClearImage();
	m_GrabberMap.clear();
	m_IdPositionMap.clear();
}

void	MergeRender::Render( CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
	HRESULT hr;
	std::vector< std::pair< int, boost::shared_ptr<IBaseGrabber> > >::iterator it = m_GrabberMap.begin();
	
	const BITMAPINFO*	pBi;
	const BYTE*			pByte;
	int					nImgSize;
	for( ; it != m_GrabberMap.end(); ++it )
	{
		std::pair<int, int>	p = GetPosFromID( it->first );
		IBaseGrabber* pGrabber = it->second.get();
		HRESULT hr = pGrabber->LockFrame(pBi, pByte, nImgSize);
		if( hr == S_OK )
		{
			hr = m_pRender->SetImage( p.first, p.second, pBi, pByte );
			pGrabber->UnlockFrame();
		}
		else
		{
			hr = m_pRender->SetImage( p.first, p.second, NULL, NULL );
		}
	}

	CRect rc;
	m_pWnd->GetClientRect(rc);
#if 0
	hr = m_pRender->Resize( m_pWnd->GetSafeHwnd(), rc.Width(), rc.Height() );
#endif
	hr = m_pRender->Render( m_pWnd->GetSafeHwnd(), GetSysColor( COLOR_BTNFACE ) );
	ASSERT(hr == S_OK);

}

void	MergeRender::Resize( int x, int y )
{
	if( !m_pWnd->IsWindowVisible() ) 
	{
		return;
	}
	if(! m_pRender ) return;
	HRESULT hr;
//	m_wndRenderTarget.SetWindowPos( 0, 0, 0, x, y, SWP_NOMOVE|SWP_NOZORDER );
	hr = m_pRender->Resize( m_pWnd->GetSafeHwnd(), DWORD(x), DWORD(y) );
	ASSERT(hr == S_OK);
}

bool	MergeRender::CanTakeScreenShot()
{
	return true;
}

boost::shared_ptr<Gdiplus::Image>	MergeRender::TakeScreenShot( )
{
	const BITMAPINFO*	pbmi;
	const BYTE*			pBytes;
	int					nImgSize;

#if 0
	CRect rc;
	m_pWnd->GetClientRect( &rc );
	m_pRender->Resize( m_pWnd->GetSafeHwnd(), rc.Width(), rc.Height() );
#endif
	HRESULT hr = m_pRender->LockFrame( pbmi, pBytes, nImgSize);
	if( hr == S_OK )
	{
//		((BITMAPINFO*)pbmi)->bmiHeader.biSizeImage = nImgSize;
		m_cacheArr.assign( pBytes, pBytes + nImgSize );
		memcpy( &m_cacheBi, pbmi, sizeof(BITMAPINFO) );
		m_pRender->UnlockFrame();
		boost::shared_ptr< Gdiplus::Image > Img( new Gdiplus::Bitmap( (BITMAPINFO*)&m_cacheBi, (VOID*)&m_cacheArr[0] ) );
		return Img;
	}
	else
		return boost::shared_ptr< Gdiplus::Image >();
}

void	MergeRender::OnLButtonDblClk( CPoint point )
{
	UNREFERENCED_PARAMETER (point); 
}

void	MergeRender::Destroy()
{
#if 0
	InterlockedDecrement( &m_nRenderRef );
	if( m_nRenderRef == 0 )
	{
		m_pRender.reset();
	}
#else
	OutputDebugString(_T("[Render]destroying\n"));
	if( m_pRender.get() )	m_pRender.reset();
	OutputDebugString(_T("[Render]succeeded\n"));
#endif
}

void	MergeRender::SetDefaultState()
{
	PosMap_t::const_iterator it = m_IdPositionMap.begin();
	for( ; it != m_IdPositionMap.end(); ++it)
	{
		int x = it->second.first;
		int y = it->second.second;
		m_pRender->SetImage( x, y, NULL, NULL );
	}
}