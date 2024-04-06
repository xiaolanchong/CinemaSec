//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Simple video window array render class
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 17.06.2005
//                                                                                      //
//======================================================================================//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "ArrayRender.h"
#include "../../background/EmptyBackground.h"

// FIXME - in cfg
//const int c_nXRes	= 320;
//const int c_nYRes	= 240;
const int c_nGap	= 0;

//======================================================================================//
//                                  class ArrayRender                                   //
//======================================================================================//
ArrayRender::ArrayRender( CWnd* pWnd ) : 
	IVideoRender(pWnd),
	m_SingleLayout( 1 ,1, RectImageLayout::ftKeepAspectRatio, 1 ),
	m_dwSizeX(720), 
	m_dwSizeY(576),
	m_bShowID(false)
{
}

ArrayRender::~ArrayRender()
{
}

void ArrayRender::DrawHeader( Gdiplus::Font& font, Gdiplus::Graphics& gr, CRectF rc,CString sText)
{
	Gdiplus::StringFormat stringFormat;
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
	stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);

	std::wstring str = sText;			

	SolidBrush brString( static_cast<DWORD>(Color::White) );
	Gdiplus::SmoothingMode SmoothMd =  gr.GetSmoothingMode();
	gr.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	gr.DrawString( str.c_str(), -1, &font, rc, &stringFormat, &brString);
	gr.SetSmoothingMode(SmoothMd);
}

void ArrayRender::DrawID( Gdiplus::Font& font, Gdiplus::Graphics& gr, CRectF rc, int nID)
{
	Gdiplus::StringFormat stringFormat;
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
	stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);

	CStringW sID;
	sID.Format(L"%d", nID );

	SolidBrush brString( static_cast<DWORD>(Color::White) );
	Gdiplus::SmoothingMode SmoothMd =  gr.GetSmoothingMode();
	gr.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	gr.DrawString( sID, -1, &font, rc, &stringFormat, &brString);
	gr.SetSmoothingMode(SmoothMd);
}

std::auto_ptr<Gdiplus::Font> ArrayRender::InitFont()
{
	CWnd* pWnd = AfxGetMainWnd();
	CDC* pDC = pWnd->GetDC();
	Gdiplus::Font* 	pfont = new Font( pDC->GetSafeHdc(), &m_lf );
	pWnd->ReleaseDC(pDC);
	return std::auto_ptr<Gdiplus::Font>(pfont);
}

void ArrayRender::DrawVideoImage(Gdiplus::Graphics& gr )
{
	std::auto_ptr<Gdiplus::Font> pfont = InitFont();

	CRect rcHeader(0,0,0,0);
	if( IsSingleMode() )
	{
		m_SingleLayout.recalculate( m_dwSizeX, m_dwSizeY, m_pWnd );
		CRect rc = m_SingleLayout.rectangle();
		m_VideoWnds[m_nActiveVideo].Draw( gr, rc );
		if( m_bShowID )  DrawID( *pfont.get(),  gr, rc, m_WindowArrayID[m_nActiveVideo] );
		rcHeader = rc;
	}
	else
	{
		std::pair<DWORD, DWORD> p = m_Layout.GetMaxNonEmptyRect();
		for( size_t i = 0; i < m_VideoWnds.size(); ++i)
		{
			CRect rc = m_Layout.rectangle( i );
			m_VideoWnds[i].Draw( gr, rc );
			if( m_bShowID )  DrawID( *pfont.get(),  gr, rc, m_WindowArrayID[i] );
			if( rcHeader.IsRectEmpty() ) rcHeader = rc;
			else 
			{
				CRect rcTemp(rcHeader);
				rcHeader.UnionRect( rcTemp, rc);
			}	
		}
	}

	DrawHeader(*pfont.get(), gr, CRectF(rcHeader) , m_sHeader );
}

void ArrayRender::Render( CDC* pDC )
{
	CRect rc;
	m_pWnd->GetClientRect(&rc);

	Gdiplus::Bitmap		BackBitmap( rc.Width(), rc.Height() );
	Gdiplus::Graphics	BackGr( &BackBitmap );
	COLORREF rgbBack = GetSysColor( COLOR_BTNFACE );
	Gdiplus::Color clBack = Color::MakeARGB( 0xFF, GetRValue(rgbBack), GetGValue(rgbBack), GetBValue(rgbBack) );
	BackGr.Clear( clBack );
	// draw

	DrawVideoImage( BackGr );

	Gdiplus::Graphics gr(pDC->GetSafeHdc());
	gr.DrawImage( &BackBitmap, 0, 0 );
}

void	ArrayRender::Resize( int , int ) 
{
	m_SingleLayout.recalculate( m_dwSizeX, m_dwSizeY, m_pWnd );
	std::pair<DWORD, DWORD> p = m_Layout.GetMaxNonEmptyRect();
	m_Layout.recalculate( p .first * m_dwSizeX, p.second * m_dwSizeY, m_pWnd );
	m_pWnd->Invalidate(FALSE);
}

void	ArrayRender::EnterToSingleMode(int nIndex)
{
	UNREFERENCED_PARAMETER(nIndex);
#if 0
	CRect rc;
	GetClientRect( &rc );
	OnSize( SIZE_RESTORED, rc.Width(), rc.Height() );
#else
	m_pWnd->SetWindowPos( 0, 0, 0, 0, 0, SWP_NOZORDER| SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE );
#endif
	m_pWnd->Invalidate(FALSE);
}

void	ArrayRender::EnterToCommonMode()
{
#if 0
	CRect rc;
	GetClientRect( &rc );
#endif
	m_pWnd->Invalidate(FALSE);
}

bool	ArrayRender::CanTakeScreenShot()
{
	std::pair<DWORD, DWORD> p = m_Layout.GetMaxNonEmptyRect();
	CSize sizeImage(0,0);
	for(size_t i = 0; i < m_VideoWnds.size(); ++i )
	{
		CBackground* pBg = m_VideoWnds[i].GetBackground();
		if( pBg && !dynamic_cast<CEmptyBackground*>( pBg ) )
		{
			sizeImage = pBg->GetSize();
		}
	}
	return sizeImage.cx != 0 && sizeImage.cy != 0;
}

boost::shared_ptr<Gdiplus::Image>	ArrayRender::TakeScreenShot( )
{
	ASSERT( CanTakeScreenShot() );
	
	std::auto_ptr<Gdiplus::Font> pfont = InitFont();

	std::pair<DWORD, DWORD> p = m_Layout.GetMaxNonEmptyRect();
	CSize sizeImage(0,0);
	for(size_t i = 0; i < m_VideoWnds.size(); ++i )
	{
		CBackground* pBg = m_VideoWnds[i].GetBackground();
		if( pBg && !dynamic_cast<CEmptyBackground*>( pBg ) )
		{
			//assume all image have the equal size
			sizeImage = pBg->GetSize();
			break;
		}
	}
	CSize sizeTotal( sizeImage.cx * p.first, sizeImage.cy * p.second );
	Gdiplus::Bitmap BackBmp( sizeTotal.cx, sizeTotal.cy );
	Gdiplus::Graphics gr( &BackBmp );
	Gdiplus::Color clBack = Gdiplus::ARGB(Color::Black) ;
	SolidBrush brBack( clBack );
	gr.FillRectangle( &brBack, 0, 0, sizeTotal.cx, sizeTotal.cy );

	const SparceRectImageLayout::IndexArray_t& IndexArr = m_Layout.GetCells();
	ASSERT( m_VideoWnds.size() ==  IndexArr.size() );
	CRect rcHeader(0,0,0,0);
	for(size_t i = 0; i < IndexArr.size(); ++i )
	{
		const SparceRectImageLayout::Cell& c = IndexArr[i];
		int nXOff = c.m_nX * sizeImage.cx;
		int nYOff = c.m_nY * sizeImage.cy;
		CBackground* pBg = m_VideoWnds[i].GetBackground();
		CRect rc( CPoint(nXOff, nYOff), sizeImage );
		if( !dynamic_cast<CEmptyBackground*>( pBg ) )
		{
			pBg->Draw( gr, rc );
		}
		if( rcHeader.IsRectEmpty() ) rcHeader = rc;
		else 
		{
			CRect rcTemp(rcHeader);
			rcHeader.UnionRect( rcTemp, rc);
		}	
	}

	DrawHeader( *pfont.get(), gr, CRectF(rcHeader), m_sHeader );
	return boost::shared_ptr<Gdiplus::Image>( BackBmp.Image::Clone(  ) );
}

void	ArrayRender::LButton( int nIndex )
{
	if( m_nActiveVideo == INVALID_ACTIVE_VIDEO )
	{
		m_nActiveVideo = nIndex;
		EnterToSingleMode(nIndex);
	}
	else
	{
		m_nActiveVideo = INVALID_ACTIVE_VIDEO;
		EnterToCommonMode();
	}
}


void	ArrayRender::OnLButtonDblClk( CPoint point )
{
	if( m_VideoWnds.size() != 1 )
	{

		if( IsSingleMode() )
		{

			m_SingleLayout.recalculate( m_dwSizeX, m_dwSizeY, m_pWnd );
			CRect rc = m_SingleLayout.rectangle();
			if( rc.PtInRect( point ) )
				LButton( m_nActiveVideo );
		}
		else
		{

			std::pair<DWORD, DWORD> p = m_Layout.GetMaxNonEmptyRect();
			for( size_t i = 0; i < m_VideoWnds.size(); ++i)
			{
				CRect rc = m_Layout.rectangle( i );
				// assume have no scroll bars;
				if( rc.PtInRect( point )  )
				{
					//	m_VideoWnds[i].Draw( gr, rc );
					LButton( (int)i );
					break;
				}
			}
		}
	}
}

void	ArrayRender::LoadRoom( int nRoomID, const WindowArray_t& wa )
{
	UNREFERENCED_PARAMETER( nRoomID );
	m_nActiveVideo = INVALID_ACTIVE_VIDEO;

	m_VideoWnds.resize( wa.size() );
	m_WindowArrayID.clear();

	SparceRectImageLayout::IndexArray_t wndPos;
	DWORD nMaxX = 0, nMaxY = 0;

	m_WindowID.clear();

	for(size_t i = 0; i <wa.size(); ++i )
	{
		wndPos.push_back( SparceRectImageLayout::Cell( wa[i].m_X, wa[i].m_Y )  );
		m_VideoWnds[ i ].ClearBackground( );

		nMaxX = std::max<DWORD>( nMaxX,  wa[i].m_X ) ;
		nMaxY = std::max<DWORD>( nMaxY,  wa[i].m_Y ) ;
		m_WindowID.insert( std::make_pair( wa[i].m_nID, &m_VideoWnds[i] ) );
		m_WindowArrayID.push_back( wa[i].m_nID );
	}
	m_Layout.SetArray( nMaxX+1, nMaxY+1);
	m_Layout.SetMode( SparceRectImageLayout::ftKeepAspectRatio );
	m_Layout.SetCells( wndPos );
}

void	ArrayRender::Clear()
{
	for(size_t i = 0; i < m_VideoWnds.size(); ++i )
	{
		m_VideoWnds[i].ClearBackground();
	}
}

void	ArrayRender::SetImage( int nID, const std::vector<BYTE>& ImageArr)
{
	std::map<int, VideoImage*>::const_iterator it = m_WindowID.find( nID );
	if( it != m_WindowID.end() )
	{
		const BYTE* pBmp = (const BYTE* )&ImageArr[0];
		size_t nSize	=	ImageArr.size();
		it->second->SetBackground( pBmp, nSize );
	}
	else
	{
		// error probably
	}
}

void	ArrayRender::SetImage( int nID, const BITMAPINFO* pBmpInfo, const void* pBits)
{
	std::map<int, VideoImage*>::const_iterator it = m_WindowID.find( nID );
	if( it != m_WindowID.end() )
	{
		if( pBmpInfo )
		{
			m_dwSizeX = pBmpInfo->bmiHeader.biWidth;
			m_dwSizeY = pBmpInfo->bmiHeader.biHeight;
		}
		it->second->SetBackground( pBmpInfo, pBits );
	}
	else
	{
		// error probably
	}
}

void	ArrayRender::SetCameraSource( int nID, int nCameraID)
{
	std::map<int, VideoImage*>::const_iterator it = m_WindowID.find( nID );
	if( it != m_WindowID.end() )
	{
		it->second->SetCameraSource (/* sSource.c_str() */ nCameraID);
	}
	else
	{
		// error probably
		//	ASSERT(FALSE);
	}
}

void	ArrayRender::SetVideoSource( int nID, const std::wstring& sSource )
{
	std::map<int, VideoImage*>::const_iterator it = m_WindowID.find( nID );
	if( it != m_WindowID.end() )
	{
		it->second->SetFileSource ( sSource.c_str() );
	}
	else
	{
		//	ASSERT(FALSE);
		// error probably
	}
}

void	ArrayRender::SetWindowHeader( const LOGFONTW& lf, CString sHeader)
{
	m_lf = lf;
	m_sHeader = sHeader;
}


bool	ArrayRender::ForceToSingleMode( int x, int y)
{
	const SparceRectImageLayout::IndexArray_t& PosArr = m_Layout.GetCells();
	for( size_t i = 0; i < PosArr.size(); ++i )
	{
		if( PosArr[i].m_nX == x && PosArr[i].m_nY == y)
		{
			m_nActiveVideo = int(i);
			return true;
		}
	}
	return false;
}

void	ArrayRender::UnloadRoom()
{
	ArrayRender::Clear();
}

void	ArrayRender::ClearImage()
{
	ArrayRender::Clear();
}