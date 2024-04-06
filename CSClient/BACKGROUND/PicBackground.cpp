// PicBackground.cpp: implementation of the CPicBackground class.
//
//////////////////////////////////////////////////////////////////////

#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "PicBackground.h"
#include <atlconv.h>
#include "..\res\resource.h"
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

// due to biImageSize=0
inline DWORD TrueImageSize( const BITMAPINFO& bi )
{
	return max(bi.bmiHeader.biSizeImage, DIBSIZE(bi.bmiHeader));
}

CBitmapBackground::CBitmapBackground(const BITMAPINFO* pbi, const void * pbits):
	m_bi(*pbi),
	m_bits( new BYTE[ TrueImageSize(m_bi) ] ),
	m_copy( m_bits.get(), (BYTE*)pbits, TrueImageSize(m_bi) ),
	m_Image( &m_bi, m_bits.get() )
{
	if (m_Image.GetLastStatus() != Gdiplus::Ok) 
	{
		USES_CONVERSION;
		CString str ;
		str.LoadString( IDS_EXC_INVALID_IMAGE_FILE );
		throw ImageFileException( T2CA( str ) );
	}
}

CBitmapBackground::CBitmapBackground(const std::wstring& FileName):
	m_Image( FileName.c_str(), FALSE )
{
	if (m_Image.GetLastStatus() != Gdiplus::Ok) 
	{
		USES_CONVERSION;
		CString str ;
		str.LoadString( IDS_EXC_INVALID_IMAGE_FILE );
		throw ImageFileException( T2CA( str ) );
	}
}



void CBitmapBackground::Draw(Graphics& gr, const CRectF& rc) const
{
	Bitmap* img = const_cast<Bitmap*>(&m_Image);
	gr.DrawImage( img, rc );
}

CSize	CBitmapBackground::GetSize() const
{
	Image* img = const_cast<Bitmap*>(&m_Image);
	return CSize(img->GetWidth(), img->GetHeight());
}


CBitmapBackground:: ~CBitmapBackground()
{
//	m_bits.reset();
//	OutputDebugString( "~CBitmapBackground\n" );
}