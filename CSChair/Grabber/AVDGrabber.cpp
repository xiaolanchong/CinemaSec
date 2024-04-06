//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Description is not available
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 15.08.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "AVDGrabber.h"

#include <dshow.h>
//======================================================================================//
//                                   class AVDGrabber                                   //
//======================================================================================//
AVDGrabber::AVDGrabber(LPCWSTR szFileName, int bpp, bool bLoop, IDebugOutput* pDbgInt ):
	m_bpp(bpp),
	m_nCurFrame(0),
	m_pGrabber( avideolib::CreateAVideoReader( szFileName, pDbgInt, false ) )
{
	if( m_bpp != 24 )  throw AVDGrabberException("");
	if( !m_pGrabber.get() ) 
	{
		throw AVDGrabberException("");
	}
//	INT64 nTotal;
//	HRESULT hr = GetTotalFrames( nTotal ); 
}

AVDGrabber::~AVDGrabber()
{
}

HRESULT AVDGrabber::GetSize( SIZE& sizeFrame )	
{
	if (m_pGrabber.get() == 0) return E_FAIL;

	alib::TStateFlag flag;
	flag.data = avideolib::IBaseVideo::WIDTH;
	if (m_pGrabber->GetData( &flag ))
	{
		sizeFrame.cx = flag.data;
		flag.data = avideolib::IBaseVideo::HEIGHT;
		if (m_pGrabber->GetData( &flag ))
		{
			sizeFrame.cy = flag.data;
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT AVDGrabber::GetCurFrame( INT64 &nCurPos )
{
	if (m_pGrabber.get() == 0) return E_FAIL;
	m_nCurFrame = (int)nCurPos;
	return S_OK;
}

HRESULT AVDGrabber::GetTotalFrames( INT64 &nTotal )	
{
	if (m_pGrabber.get() == 0) return E_FAIL;
	alib::TStateFlag flag;
	flag.data = avideolib::IBaseVideo::FRAME_NUMBER;
	if (m_pGrabber->GetData( &flag ))
	{
		nTotal = flag.data;
		return S_OK;
	}
	else return E_FAIL;
}

HRESULT AVDGrabber::Seek( INT64 nCurPos )	
{
	if (m_pGrabber.get() == 0) return E_FAIL;
	m_nCurFrame = (int)nCurPos;
	return S_OK;
}

HRESULT AVDGrabber::GetFPS(double& fFPS)
{
	if (m_pGrabber.get() == 0) return E_FAIL;
	fFPS = 10.0;
	return S_OK;
}

HRESULT AVDGrabber::LockFrame( const BITMAPINFO * &ppHeader, const BYTE * &ppImage, int   &nImgSize )
{
	if (m_pGrabber.get() == 0) return E_FAIL;

	int nTime;
	avideolib::IVideoReader::ReturnCode ret = m_pGrabber->ReadFrame( &m_ByteCache, -1, &m_nCurFrame, &nTime);
	if( ret != avideolib::IVideoReader::NORMAL ) return E_FAIL;
	
	BITMAPINFOHEADER& bih = m_cacheHdr.bmiHeader;
	memset( &bih, 0, sizeof (BITMAPINFOHEADER)  );
	bih.biBitCount	= m_bpp;
	bih.biPlanes	= 1;
	bih.biWidth		= m_ByteCache.width();
	bih.biHeight	= m_ByteCache.height();
	bih.biSize		= sizeof(BITMAPINFOHEADER);
	bih.biCompression = BI_RGB;
	bih.biSizeImage	= DIBSIZE(bih);

	m_LockCache.resize( bih.biSizeImage );

	for( int i = 0; i < m_ByteCache.size(); ++i )
	{
		BYTE clr = m_ByteCache[i];
		m_LockCache[ 3* i + 0 ] = 
		m_LockCache[ 3* i + 1 ] = 
		m_LockCache[ 3* i + 2 ] = clr;
	}

	ppHeader = &m_cacheHdr;
	ppImage	 = &m_LockCache[0];
	nImgSize = (int)m_LockCache.size();
	return S_OK;
}