//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Отладочная заглушка для ICameraGrabber, создает тестовое изображение
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 05.07.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "StubCameraGrabber.h"
#include "../ChairError.h"
#include "../interfaceEx.h"

#include <dshow.h>

#include "..\public\memleak.h"

BaseStubCameraGrabber::BaseStubCameraGrabber( int nID, DWORD dwWidth, DWORD dwHeight ):m_nID(nID)
{
	BITMAPINFOHEADER& bih = m_cacheHdr.bmiHeader;
	memset( &bih, 0, sizeof (BITMAPINFOHEADER)  );
	bih.biBitCount	= 24;
	bih.biPlanes	= 1;
	bih.biWidth		= dwWidth;
	bih.biHeight	= dwHeight;
	bih.biSize		= sizeof(BITMAPINFOHEADER);
	bih.biCompression = BI_RGB;
	bih.biSizeImage	= DIBSIZE(bih);
	
	//m_cacheImage.resize( bih.biSizeImage, 128 );

	HWND hWnd = GetDesktopWindow();
	CWnd wnd; wnd.Attach(hWnd);
	CWnd* pWnd = &wnd;
	CDC* pDC = pWnd->GetDC();
	m_cacheBmp.CreateCompatibleBitmap( pDC, dwWidth, dwHeight );
	m_cacheDC.CreateCompatibleDC( pDC );
	pWnd->ReleaseDC(pDC);
	m_cacheDC.SelectObject( &m_cacheBmp );
	wnd.Detach();
}

void		BaseStubCameraGrabber::GenerateImage( )
{
	CBrush br( RGB( 255, 255, 255 ) );
	CGdiObject* pOldPen		= m_cacheDC.SelectObject( &br );

	// Not localized since it is a test video image
	CRect rcDraw( 0, 0, m_cacheHdr.bmiHeader.biWidth, m_cacheHdr.bmiHeader.biHeight );
	CString sText = GenerateHeader();
	m_cacheDC.FillSolidRect( rcDraw, RGB( 0xF0, 0xE6, 0x8C ) ); // haki
	m_cacheDC.DrawText(sText, (LPRECT)&rcDraw, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	int res;
	res = GetDIBits(m_cacheDC.GetSafeHdc(), (HBITMAP) m_cacheBmp.GetSafeHandle(),
		0, rcDraw.Height(), NULL, &m_cacheHdr, DIB_RGB_COLORS);
	ASSERT(res);
	m_cacheImage.resize( m_cacheHdr.bmiHeader.biSizeImage );
	res = GetDIBits(m_cacheDC.GetSafeHdc(), (HBITMAP) m_cacheBmp.GetSafeHandle(),
		0, rcDraw.Height(), &m_cacheImage[0], &m_cacheHdr, DIB_RGB_COLORS);
	ASSERT(res);
	m_cacheDC.SelectObject( pOldPen );	
}

HRESULT BaseStubCameraGrabber::LockFrame( const BITMAPINFO * &ppHeader, const BYTE * &ppImage, int   &nImgSize )
{
//	if(!ppHeader || !ppImage) return E_INVALIDARG;
	GenerateImage();
	ppHeader	= &m_cacheHdr;
	ppImage		= &m_cacheImage[0];
	nImgSize	= (int)m_cacheImage.size();
	return S_OK;
}

HRESULT BaseStubCameraGrabber::UnlockFrame()	
{ 
	return S_OK;
};	

CString	RealtimeStubCameraGrabber::GenerateHeader()
{
	CTime time( CTime::GetCurrentTime() );
	CString sFmtText = time.Format( _T("Realtime %H:%M:%S %d %b %y ID=%%d") );
	CString sText;
	sText.Format( sFmtText, m_nID );
	return sText;
}

CString		ArchiveStubCameraGrabber::GenerateHeader(  )
{
	INT64 t;
	GetTime(t);
	CTime time( /*m_nTime*/ t );
	CString sFmtText = time.Format( _T("Archive %H:%M:%S %d %b %y ID=%%d") );
	CString sText;
	sText.Format( sFmtText, m_nID );
	return sText;
}

//////////////////////////

BaseServerStubCameraGrabber::BaseServerStubCameraGrabber( int nID, DWORD dwWidth, DWORD dwHeight ):
	m_nID(nID)
#ifdef TEST_EXCEPTION
	,m_dwFrameNum(0)
#endif
{
	BITMAPINFOHEADER& bih = m_cacheHdr.bmiHeader;
	memset( &bih, 0, sizeof (BITMAPINFOHEADER)  );
	bih.biBitCount	= 16;
	bih.biPlanes	= 1;
	bih.biWidth		= dwWidth;
	bih.biHeight	= dwHeight;
	bih.biSize		= sizeof(BITMAPINFOHEADER);
	bih.biCompression = mmioFOURCC( 'Y', 'U', 'Y', '2') ;
	bih.biSizeImage	= DIBSIZE(bih);

	m_cacheImage.resize( bih.biSizeImage, 0x80 );

	for( size_t i =0 ; i < dwHeight/4; ++i )
		memset( &m_cacheImage[2*dwWidth * i + 0], 0xFF, 2*dwWidth/4 );
}

HRESULT BaseServerStubCameraGrabber::LockFrame( const BITMAPINFO * &ppHeader, const BYTE * &ppImage, int   &nImgSize )
{
#ifdef TEST_EXCEPTION
	++m_dwFrameNum;
	if( m_dwFrameNum == 100 )
	{
		ppHeader = NULL;
		((BITMAPINFO *)ppHeader)->bmiHeader.biXPelsPerMeter = 0;
	}
#endif
	ppHeader	= &m_cacheHdr;
	ppImage		= &m_cacheImage[0];
	nImgSize	= (int)m_cacheImage.size();
	return S_OK;
}

//======================================================================================//
//                               class StubGrabberFactory                                //
//======================================================================================//
StubGrabberFactory::StubGrabberFactory()
{
}

StubGrabberFactory::~StubGrabberFactory()
{
}

HRESULT		StubGrabberFactory::CreateCameraGrabber( ICameraGrabber** ppFrameGrabber, int nCameraID )
{
	m_Debug.PrintW( IDebugOutput::mt_info, L"Create RealtimeStubCameraGrabber for id=%d", nCameraID );
	*ppFrameGrabber = new RealtimeStubCameraGrabber (nCameraID, 720, 576) ;
	return S_OK;
}

HRESULT		StubGrabberFactory::CreateArchiveGrabber( IArchiveGrabber** ppFrameGrabber, int nCameraID, INT64 nStartTime )
{
	m_Debug.PrintW( IDebugOutput::mt_info, L"Create ArchiveStubCameraGrabber for id=%d", nCameraID );
	*ppFrameGrabber = new ArchiveStubCameraGrabber (nCameraID, 720, 576, nStartTime) ;
	return S_OK;	
}

EXPOSE_INTERFACE( StubGrabberFactory, IGrabberFactory, STUB_GRABBER_FACTORY );
EXPOSE_INTERFACE( StubServerGrabberFactory, IGrabberFactory, STUB_SERVER_GRABBER_FACTORY );