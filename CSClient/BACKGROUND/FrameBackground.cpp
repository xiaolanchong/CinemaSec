// FrameBackground.cpp: implementation of the CFrameBackground class.
//
//////////////////////////////////////////////////////////////////////
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "csclient.h"
#include "FrameBackground.h"
#include <strmif.h>
#include <amvideo.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFrameBackground::CFrameBackground( boost::shared_ptr<IBaseGrabber> pGrabber ) : 
	m_pGrabber(pGrabber),
	m_sizeCachedLastSize(0,0)
{

}

bool	CFrameBackground::InitCompressor( const BITMAPINFOHEADER& bihIn ) const
{
	try
	{
		m_Decomressor  = boost::shared_ptr<ImageDecompressor>
				( new ImageDecompressor( bihIn, bihIn.biWidth, bihIn.biHeight, 32 ));
		return true;
	}
	catch(ImageDecompressorException)
	{
		return false;
	}
}

void	CFrameBackground::Draw(Graphics& gr, const CRectF& rc) const 
{
	// FIXME
	UINT	szNoGrabber			= IDS_ERROR_VIDEO;//L"Video is not available";
	UINT	szInvalidFormat		= IDS_ERROR_VIDEO;//L"Invalid image";
	UINT	szLockFailed		= IDS_ERROR_VIDEO;//L"Frame not available";

	LPCBITMAPINFO	pbi;
	LPCUBYTE		pBits;
	int				nSize;
	if( !m_pGrabber )
	{
		CString s;s.LoadString( szNoGrabber );
		DrawError(gr, rc, s);
		return;
	}
	BITMAPINFO	biCache;
	HRESULT hr = m_pGrabber->LockFrame( pbi, pBits, nSize );
	if( hr == S_OK )
	{
		const BITMAPINFO*	pbiEnd;
		const void		*	pEndBytes;
		if( pbi->bmiHeader.biCompression != BI_RGB )
		{	
			bool bRes = true;
			if( !m_Decomressor.get() ) 
			{
				bRes = InitCompressor( pbi->bmiHeader );
			}
			if( !bRes )
			{
				CString s;s.LoadString( szInvalidFormat );
				DrawError(gr, rc, s ); 
				m_pGrabber->UnlockFrame();
				return;
			}
			bRes = m_Decomressor->Decompress( pBits, m_DecompressedBuffer );
			if( !bRes )
			{
				CString s;s.LoadString( szInvalidFormat );
				DrawError(gr, rc, s ); 
				m_pGrabber->UnlockFrame();
				return;
			}
			else
			{
				m_sizeCachedLastSize.cx = pbi->bmiHeader.biWidth;
				m_sizeCachedLastSize.cy = pbi->bmiHeader.biHeight;
			}
			memcpy( &biCache.bmiHeader, &m_Decomressor->GetDstFormat(), sizeof(BITMAPINFOHEADER) );
			pEndBytes	= (const BYTE*)&m_DecompressedBuffer[0];
			pbiEnd		= &biCache;
		}
		else
		{
			m_sizeCachedLastSize.cx = pbi->bmiHeader.biWidth;
			m_sizeCachedLastSize.cy = pbi->bmiHeader.biHeight;
			pEndBytes = (const BYTE*)pBits;
			pbiEnd = pbi;
		}
		Gdiplus::Bitmap bmp( (BITMAPINFO*) pbiEnd, ( void*)pEndBytes );
		Gdiplus::Status Stat = gr.DrawImage( &bmp, rc );
		if( Stat != Gdiplus::Ok )
		{
			CString s;s.LoadString( szLockFailed );
			DrawError( gr, rc, s );
		}
		m_pGrabber->UnlockFrame();
	}
	else
	{
		CString s;s.LoadString( szLockFailed );
		DrawError(gr, rc, s );
	}
}

CSize	CFrameBackground::GetSize()	const 
{
	return m_sizeCachedLastSize;
}

void	CFrameBackground::DrawError( Graphics& gr, const CRectF& rc, LPCWSTR szDesc ) const
{
	Gdiplus::FontFamily  fontFamily(L"Arial");
	Gdiplus::Font        font(&fontFamily, 24, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

	Gdiplus::StringFormat stringFormat;
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);

	std::wstring str = szDesc;		
	SolidBrush brString( static_cast<DWORD>(Color::DarkRed) );
	gr.DrawString( str.c_str(), -1, &font, rc, &stringFormat, &brString);
}


