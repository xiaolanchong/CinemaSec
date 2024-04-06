// AviIOGrabber.cpp: implementation of the AviIOGrabber class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "csclient.h"
#include "AviIOGrabber.h"

#include <dshow.h>
//#include <amvideo.h>
///#include <dvdmedia.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int c_MilliSecInSec		= 1000;
const int c_100NanoSecInSec		= 10 * 1000;

AviIOGrabber::AviIOGrabber():
	m_ok(false),
	m_frameIndex(0)
{

}

AviIOGrabber::~AviIOGrabber()
{

}

const BITMAPINFOHEADER *	AviIOGrabber::GetDIB() const
{
	if( !m_ok ) return 0;
	if( m_Decompressor.get() )
	{
		return &m_Decompressor->GetDstFormat();
	}
	else
	{
		return &m_bihCache;
	}
}

bool AviIOGrabber::OpenRead( LPCTSTR fileName )
{
	m_frameIndex = 0;
	m_ok = false;
	USES_CONVERSION;
	LPCWSTR szWideName = T2CW( fileName );

	m_pGrabber = GetCameraManager().CreateFileGrabber( szWideName, 24, false );
	if( m_pGrabber.get() )
	{
		HRESULT hr;
		hr = m_pGrabber->GetFPS( m_fFPS );
		if( FAILED(hr) )
		{
			m_ok = false; return false;
		}
		hr = m_pGrabber->GetTotalFrames( m_nFrameNumber );
		if( FAILED(hr) )
		{
			m_ok = false; return false;
		}
		m_ok = true;
		bool res = GetFrame(0);
		if( !res ) m_ok = false;
	}
	return m_ok;
}

void AviIOGrabber::Close()
{
	m_Decompressor = std::auto_ptr<ImageDecompressor>();
	m_pGrabber.reset();
}

bool AviIOGrabber::GetFrame( int index )
{
	HRESULT hr;
	if(!m_ok || !m_pGrabber) return false;
	if( index != m_frameIndex )
	{
		hr = m_pGrabber->Seek(index);
		if( FAILED(hr) ) return false;
		m_frameIndex = index;
	}

	const BITMAPINFO* pbi;
	const BYTE*	pBytes;
	int nSize;
	hr = m_pGrabber->LockFrame( pbi, pBytes, nSize );
	if(FAILED(hr)) return false;
	size_t nOff = sizeof( BITMAPINFOHEADER );
	size_t nHdrOff = sizeof( BITMAPINFOHEADER );
	if( pbi->bmiHeader.biCompression != BI_RGB)
	{
		if( !m_Decompressor.get() )
		{
			try
			{
			m_Decompressor = std::auto_ptr<ImageDecompressor>(
				new ImageDecompressor( pbi->bmiHeader, pbi->bmiHeader.biWidth, pbi->bmiHeader.biHeight, 24 ) );
			}
			catch( ImageDecompressorException )
			{
				m_pGrabber->UnlockFrame();
				return false;
			}
		}
		m_Decompressor->Decompress( pBytes, m_ByteCache );
	}
	else
	{
		if( pbi->bmiHeader.biBitCount == 8 )
		{
			nOff = sizeof( BITMAPINFOHEADER ) + sizeof(RGBQUAD) * 256;
			nHdrOff = pbi->bmiHeader.biClrUsed ? ( pbi->bmiHeader.biClrUsed * sizeof(RGBQUAD) ) : sizeof(RGBQUAD) * 256;
		}
		memcpy( &m_bihCache, &pbi->bmiHeader, sizeof( BITMAPINFOHEADER ) );
		m_ByteCache.assign( pBytes,  pBytes + pbi->bmiHeader.biSizeImage);
#if 0
		m_ByteCache.resize( pbi->bmiHeader.biSizeImage + nOff );
		m_ByteCache.assign( (const BYTE*)pbi, (const BYTE*)pbi + nHdrOff );
		m_ByteCache.insert( m_ByteCache.begin() + nHdrOff, pBytes,  pBytes + pbi->bmiHeader.biSizeImage);
#endif
	}
	m_pGrabber->UnlockFrame();
	return true;
}

bool AviIOGrabber::GetNextFrame()
{
  if (!m_ok /*|| (m_pDib == 0)*/)
    return false;

  if ((m_frameIndex+1) < GetAllFrameNumber())
    return GetFrame( m_frameIndex+1 );

  return false;
}