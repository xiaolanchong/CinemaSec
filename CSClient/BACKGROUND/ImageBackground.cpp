#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include ".\imagebackground.h"

CImageBackground::CImageBackground(const std::wstring& FileName):
	m_Image( FileName.c_str(), FALSE )
{
	if (m_Image.GetLastStatus() != Gdiplus::Ok) 
	{
		throw ImageBgException( "Image file error" );
	}
}

CImageBackground::CImageBackground(const void* pBytes, size_t nSize):
	m_pStream( CreateStream( pBytes, nSize ) ),
	m_Image( m_pStream, FALSE )
{
	if( m_pStream ) 
	{
		m_pStream->Release();
		m_pStream = 0;
	}
	if (m_Image.GetLastStatus() != Gdiplus::Ok) 
	{
		throw ImageBgException( "Image stream error" );
	}
}

void	CImageBackground::Draw(Graphics& gr, const CRectF& rc) const
{
	gr.DrawImage( (Image*)&m_Image, static_cast<const RectF&>(rc) );
}

CSize	CImageBackground::GetSize() const
{
	Image& img = const_cast<Gdiplus::Image&>(m_Image);
	return CSize( img.GetWidth(), img.GetHeight() );
}