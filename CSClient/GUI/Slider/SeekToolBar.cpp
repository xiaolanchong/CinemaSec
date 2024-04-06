// SeekToolBar.cpp : implementation file
//

#include "stdafx.h"
//#include "MsEdit.h"
#include "SeekToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSeekToolBar

CSeekToolBar::CSeekToolBar():m_pWnd(0)
{
}

CSeekToolBar::~CSeekToolBar()
{
}


BEGIN_MESSAGE_MAP(CSeekToolBar, CAlphaToolBar)
	//{{AFX_MSG_MAP(CSeekToolBar)
	ON_WM_HSCROLL()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeekToolBar message handlers

void CSeekToolBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CAlphaToolBar::OnHScroll(nSBCode, nPos, pScrollBar);
}

int CSeekToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAlphaToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
		
	return 0;
}

void	CSeekToolBar::EnableSeeker( bool bEnable)
{
	m_Slider.EnableWindow(bEnable);
}

const int IDC_MY_STATIC = 10002;

void	CSeekToolBar::CreateSeeker(int size, int nGapRight, UINT nSeekID)
{
/*	SetButtonInfo(3, IDC_COMBO2, TBBS_BUTTON, size);*/
	m_nSeekID = nSeekID;

	CRect Rect;
	int nIndex = CommandToIndex( m_nSeekID );
	ASSERT( nIndex != -1 );
	GetItemRect(nIndex, &Rect);
	HCURSOR h = ::LoadCursor( 0, IDC_HAND );
	CString s = ::AfxRegisterWndClass(0, h);
	m_Slider.Create( s, 0, WS_CHILD|WS_VISIBLE|TBS_NOTICKS, Rect, this, IDC_MY_STATIC );
	SizeSeeker(size, nGapRight);
//	m_Slider.SetMax( nSeekMaxPos);
	m_Slider.SetCurrent(0);


#ifdef USE_POS
	CRect rc(Rect.right+5, Rect.top, Rect.right + 50, Rect.bottom+5);
	m_wndPosition.Create( 0, WS_CHILD | WS_VISIBLE | SS_CENTER   , rc, this  );
	m_wndPosition.m_Font.CreatePointFont(100, _T("Arial"));
	m_wndPosition.SetFont( &m_wndPosition.m_Font );
#endif
	EnableSeeker( false);
}

void			CSeekToolBar::SizeSeeker(int size, int nGapRight)
{
	CRect Rect;
	int nIndex = CommandToIndex( m_nSeekID );
	ASSERT( nIndex != -1 );
	GetItemRect(nIndex, &Rect);

	int nWidthBefore = 0;
	for( int i = 0; i < nIndex; ++i )
	{
		CRect rc;
		GetItemRect( i, &rc );
		nWidthBefore += rc.Width();
	}
	//Rect.InflateRect(0, -5, size, -5);
	const int c_Heigth = 10;
	const int c_nGapLeft	= 5;
	const int c_nGapRight	= 5;
	Rect.SetRect(	nWidthBefore + Rect.top + (Rect.Height() - c_Heigth)/2,  c_nGapLeft, 
					size - c_nGapRight - nGapRight, Rect.bottom - (Rect.Height() - c_Heigth)/2 );
	m_Slider.SetWindowPos( 0, Rect.left, Rect.top, Rect.Width(), Rect.Height(), SWP_NOZORDER );
#if USE_SEPARATOR
	UINT nID, nStyle;
	int nImage;
//	GetButtonInfo( nIndex, nID, nStyle, nImage );
//	SetButtonInfo(nIndex, nID,TBBS_SEPARATOR, Rect.Width() );
//	SetWindowPos( 0, 0, 0, size, 24, SWP_NOZORDER |SWP_NOMOVE );
#endif
#if 0
	CRect Rect;
	GetItemRect(3, &Rect);
	Rect.InflateRect(0, -5, size, -5);	
	m_Slider.SetWindowPos( 0, 0, 0, Rect.Width(), Rect.Height(), SWP_NOZORDER|SWP_NOMOVE );
	m_wndPosition.MoveWindow( Rect.right + 5, Rect.top -3, 50, 15 ) ;
#endif
}

void CSeekToolBar::OnSize(UINT nType, int cx, int cy) 
{
	CAlphaToolBar::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here	
}

