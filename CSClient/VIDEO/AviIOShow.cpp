// AviIOShow.cpp: implementation of the AviIOShow class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "csclient.h"
#include "AviIOShow.h"

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

AviIOShow::AviIOShow():
	m_ok(false),
	m_frameIndex(0)
{

}

AviIOShow::~AviIOShow()
{

}

bool AviIOShow::OpenRead( LPCTSTR fileName )
{
	USES_CONVERSION;
	CComBSTR bstrFileName( fileName );
	HRESULT hr;
	hr = m_pMedia.CoCreateInstance( CLSID_MediaDet );
	hr = m_pMedia->put_Filename( bstrFileName );
	AM_MEDIA_TYPE mt;
	hr = m_pMedia->get_StreamMediaType( &mt );

	if( mt.majortype	== MEDIATYPE_Video				&&
		(	mt.subtype		== MEDIASUBTYPE_RGB1	||
			mt.subtype		== MEDIASUBTYPE_RGB4	||
			mt.subtype		== MEDIASUBTYPE_RGB8	||
			mt.subtype		== MEDIASUBTYPE_RGB565	||
			mt.subtype		== MEDIASUBTYPE_RGB555	||
			mt.subtype		== MEDIASUBTYPE_RGB24	||
			mt.subtype		== MEDIASUBTYPE_RGB32 )		&&
		mt.formattype	== FORMAT_VideoInfo	)
	{
		const VIDEOINFO* pvi = (VIDEOINFO*) mt.pbFormat;
		memcpy( &m_header, &pvi->bmiHeader , sizeof(BITMAPINFO) );
		m_ok = true;
		CoTaskMemFree( mt.pbFormat );

		m_pMedia->get_StreamLength(&m_streamDuration);
		m_streamDuration *= c_MilliSecInSec;								// in ms
		m_streamRate = double(pvi->AvgTimePerFrame)/ c_100NanoSecInSec ;	// in ms
		m_ByteCache.reserve( pvi->bmiHeader.biSize + sizeof( BITMAPINFO ) + sizeof(BYTE) * 256 );
		GetFrame(0);
	}
	else
	{
		m_ok = false;
	}
	m_frameIndex = 0;
	return m_ok;
}

void AviIOShow::Close()
{
	m_pMedia.Release();
}

bool AviIOShow::GetFrame( int index )
{
	if(!m_ok) return false;
	HRESULT hr;
	double fTime = IndexToTime( index );
	int cx = m_header.bi.bmiHeader.biWidth;
	int cy = m_header.bi.bmiHeader.biHeight;
	long lBufferSize;
	hr = m_pMedia->GetBitmapBits( fTime / c_MilliSecInSec, &lBufferSize, NULL, cx, cy  );
	if( FAILED(hr) ) return false;
	m_ByteCache.resize( lBufferSize );
	hr = m_pMedia->GetBitmapBits( fTime / c_MilliSecInSec, 0, &m_ByteCache[0], cx, cy  );
	if( FAILED(hr) ) return false;
	m_frameIndex = index;
	return true;
}
/*
bool AviIOShow::GetNextFrame()
{
	if(!m_ok) return false;
	bool res = GetFrame( m_frameIndex );
	if(!res) return false;
//	++m_frameIndex;
	return true;
}
*/
bool AviIOShow::GetNextFrame()
{
  if (!m_ok /*|| (m_pDib == 0)*/)
    return false;

  if ((m_frameIndex+1) < GetAllFrameNumber())
    return GetFrame( m_frameIndex+1 );

  return false;
}