// LayoutWnd.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "LayoutWnd.h"
#include ".\layoutwnd.h"


// CLayoutWnd

//IMPLEMENT_DYNAMIC(CLayoutWnd, CWnd)

CLayoutWnd::CLayoutWnd( LayoutWndCB* pCB, bool bSelectable ) : 
	m_Layout( 1, 1, RectImageLayout::ftKeepAspectRatio, 0),
	m_pCB( pCB ),
	m_bSelectable(bSelectable)
{
}

CLayoutWnd::~CLayoutWnd()
{
}


BEGIN_MESSAGE_MAP(CLayoutWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CLayoutWnd message handlers


void CLayoutWnd::OnPaint()
{
	// TODO: add draw code here
	CPaintDC dc(this);
	CRect rc;
	GetClientRect(&rc);

	Gdiplus::Bitmap		BackBitmap( rc.Width(), rc.Height() );
	Gdiplus::Graphics	BackGr( &BackBitmap );
	COLORREF rgbBack = GetSysColor( COLOR_BTNFACE );
	Gdiplus::Color clBack = Color::MakeARGB( 0xFF, GetRValue(rgbBack), GetGValue(rgbBack), GetBValue(rgbBack) );
	BackGr.Clear( clBack );
	// draw here

	DrawGrid( BackGr );

	Gdiplus::Graphics gr(dc.GetSafeHdc());
	gr.DrawImage( &BackBitmap, 0, 0 );
}

bool 	CLayoutWnd::SetSize(WORD w, WORD h)
{
	if( w <= 0 || 
		h <= 0  ) return false;
//	m_nWidth	= w;
//	m_nHeight	= h;
	m_Layout.SetArray( w, h );
	m_Layout.SetDefaultCells();

	CellMap_t::iterator it = m_Cells.begin( );
	for( ; it != m_Cells.end() ;  )
	{
		std::pair<WORD, WORD> p = GetCoord( it->first );
		DWORD_PTR dwCookie = it->second;
		if( w <= p.first || h <= p.second  )
		{
			m_pCB->DeleteItem( dwCookie );
			m_Cells.erase( it++ );
		}
		else
		{
			++it;
		}
	}
	CRect rc;
	GetClientRect( &rc );
	OnSize ( SIZE_RESTORED, rc.Width(), rc.Height() );
	Invalidate();
	return true;
}

void	CLayoutWnd::DrawGrid( Gdiplus::Graphics& gr )
{
	CSize sizeCell	= m_Layout.rectangle(0).Size();//  rc.Width() / m_nWidth , rc.Height()/ m_nHeight );
	CRect rc		= m_Layout.RectImageLayout::rectangle();
	std::pair<int, int> p = m_Layout.GetArray();
	Gdiplus::Pen penLine( Color::Black );
	penLine.SetDashStyle( Gdiplus::DashStyleDot );
	for( int i = 1 ; i < p.first; ++i )
	{
		int nOff = rc.left + sizeCell.cx * i;
		Gdiplus::Point ptStart ( nOff, rc.top );
		Gdiplus::Point ptEnd( nOff, rc.bottom );
		gr.DrawLine( &penLine, ptStart, ptEnd );
	}
	for( int i = 1 ; i < p.second; ++i )
	{
		int nOff = rc.top + sizeCell.cy * i;
		Gdiplus::Point ptStart (rc.left, nOff);
		Gdiplus::Point ptEnd( rc.right, nOff );
		gr.DrawLine( &penLine, ptStart, ptEnd );
	}

	CellMap_t::iterator it = m_Cells.begin( );
	for( ; it != m_Cells.end() ; ++it )
	{
		std::pair<WORD, WORD> p = GetCoord( it->first );
		DWORD_PTR dwCookie = it->second;
		CRect rc = GetRect( p.first, p.second );
		m_pCB->DrawItem( gr, rc, dwCookie );
	}
	rc.DeflateRect( 0, 0, 1, 1 );
	gr.DrawRectangle( &penLine, CRectF(rc) );
}

void CLayoutWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	const CSize size(640, 480);
	std::pair<int, int> p = m_Layout.GetArray();
	m_Layout.recalculate( p.first * size.cx, p.second * size.cy, this );
	Invalidate();
	// TODO: Add your message handler code here
}

BOOL CLayoutWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	UNREFERENCED_PARAMETER(pDC);
	return TRUE; //CWnd::OnEraseBkgnd(pDC);
}

CRect	CLayoutWnd::GetRect( WORD x, WORD y ) 
{
	std::pair<int, int> p = m_Layout.GetArray();
	ASSERT( x >= 0 && x < p.first	);
	ASSERT( y >= 0 && y < p.second );

	const SparceRectImageLayout::IndexArray_t& IndArr = m_Layout.GetCells();
	for( size_t i = 0; i < IndArr.size(); ++i )
	{
		if( IndArr[i].m_nX == x &&
			IndArr[i].m_nY == y)
		{
			return m_Layout.rectangle( i );
		}
	}
	throw LayoutException( "No such cell ind SparceRectImageLayout" );
}
void CLayoutWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(!m_bSelectable ) return;
	const int NoImage = 1;

	DWORD dwIndex = GetIndex(point);
	if( dwIndex == ~0 ) return; 
	CellMap_t::iterator it = m_Cells.find( dwIndex );



	std::vector< CString > MenuItems;
	std::pair<WORD, WORD> Coords = GetCoord( dwIndex );
	bool bCanDelete = true;
	bool b = m_pCB->GetItemMenuContent( MenuItems, Coords.first, Coords.second, bCanDelete );
	if( !b) return; 

	CMenu menuSelect;
	menuSelect.CreatePopupMenu();
	if( bCanDelete ) 
		menuSelect.AppendMenu( MF_ENABLED |MF_STRING, NoImage,	_T("No image") ) ;

	for( size_t i = 0; i <  MenuItems.size() ; ++i ) 
	{
		menuSelect.AppendMenu( MF_ENABLED |MF_STRING , NoImage + i + 1, MenuItems[i] ) ;
	}	
	
	CPoint pt;
	GetCursorPos( &pt );
	UINT nID = menuSelect.TrackPopupMenuEx( TPM_RETURNCMD|TPM_NONOTIFY, pt.x, pt.y, this , 0 );
	// HACK
//	if( bCanDelete && nID) ++nID;
	if( nID > 0 ) 
	{
		if( it != m_Cells.end() ) 
		{
			m_pCB->DeleteItem( it->second );
			m_Cells.erase( it );
			Invalidate();
		}
		if( nID > NoImage )
		{
			size_t nIndex = nID - /*( bCanDelete ? (1 + 1) : 1 )*/ 2;
			std::pair< bool, DWORD_PTR> p = m_pCB->SelectMenuItem( nIndex );
			if( p.first )
			{
				m_Cells.insert( std::make_pair( dwIndex, p.second ) );
				Invalidate();
			}
		}
	}

	CWnd::OnRButtonDown(nFlags, point);
}

DWORD	CLayoutWnd::GetIndex( CPoint pt )
{
//	CRect rc;
//	GetClientRect(&rc);
	CRect rc		= m_Layout.RectImageLayout::rectangle();
	if( !rc.PtInRect(pt) ) return DWORD(~0);

	CSize sizeCell	= m_Layout.rectangle(0).Size();

	WORD x = (pt.x - rc.left) / sizeCell.cx; 
	WORD y = (pt.y - rc.top )/ sizeCell.cy;
	return GetIndex( x, y );
}

std::pair<WORD, WORD>	CLayoutWnd::GetSize()
{
	std::pair<int, int> p = m_Layout.GetArray();
	return std::make_pair<WORD,WORD>( p.first, p.second );
}

void CLayoutWnd::GetResultLayout( std::vector<Coord>& c )
{
	c.clear();
	CellMap_t::iterator it = m_Cells.begin( );
	for( ; it != m_Cells.end() ; ++it )
	{
		std::pair<WORD, WORD> p = GetCoord( it->first );
		DWORD_PTR dwCookie = it->second;
		c.push_back( Coord( p.first, p.second, dwCookie ) );
	}
}

int CLayoutWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rc;
	GetClientRect( &rc );
	OnSize ( SIZE_RESTORED, rc.Width(), rc.Height() );
	return 0;
}

void	CLayoutWnd::SetResultLayout( const std::vector<Coord>& c )
{
	m_Cells.clear();
	SparceRectImageLayout::IndexArray_t	IndArr;
	IndArr.reserve( c.size() );
	WORD x = 0, y = 0;
	for( size_t i = 0; i < c.size(); ++i )
	{
		DWORD dwIndex = GetIndex( c[i].x, c[i].y );
		m_Cells.insert( std::make_pair( dwIndex, c[i].dwCookie ) );
		IndArr.push_back( SparceRectImageLayout::Cell( c[i].x, c[i].y ) );
		x = std::max(x, c[i].x);
		y = std::max(y, c[i].y);
	}
	m_Layout.SetCells( IndArr );
	m_Layout.SetArray( x + 1, y + 1 );
	CRect rc;
	GetClientRect( &rc );
	OnSize ( SIZE_RESTORED, rc.Width(), rc.Height() );
}
