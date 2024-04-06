// ImageStatic.cpp : implementation file
//

#include "stdafx.h"
#include "VideoCap.h"
#include "ImageStatic.h"


// CImageStatic

IMPLEMENT_DYNAMIC(CImageStatic, CStatic)
CImageStatic::CImageStatic()
{
}

CImageStatic::~CImageStatic()
{
}


BEGIN_MESSAGE_MAP(CImageStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CImageStatic message handlers

bool	CImageStatic::SetImage( const BITMAPINFOHEADER* bih, const BYTE* pData, CString sDesc )
{
	if( !bih || !pData) 
	{
		m_Image.clear();
		return true;
	}
	m_bi.bmiHeader = *bih;
	m_Image.assign( pData, pData + bih->biSizeImage );
	m_sDesc = sDesc;
	return true;
}

void	CImageStatic::OnPaint()
{
	CPaintDC dc(this);

	CRect rc;
	GetClientRect( &rc );

	if( !m_Image.empty() )
	{
	::StretchDIBits(dc.GetSafeHdc(), 
					0, 0, rc.Width(), rc.Height(), 
					0, 0, m_bi.bmiHeader.biWidth, m_bi.bmiHeader.biHeight, 
					&m_Image[0], &m_bi, DIB_RGB_COLORS, SRCCOPY );
		dc.SetBkMode( TRANSPARENT );
		dc.SetTextColor( RGB(255, 255, 255) );
		dc.DrawText( m_sDesc, rc, DT_SINGLELINE|DT_LEFT|DT_TOP );
	}
	else
	{
		dc.SetBkMode( TRANSPARENT );
		dc.DrawText( CString(_T("No image")), rc, DT_SINGLELINE|DT_VCENTER|DT_CENTER );
	}
}