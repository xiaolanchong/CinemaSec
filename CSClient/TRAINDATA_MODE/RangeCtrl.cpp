// RangeCtrl.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
//#include "Slider.h"
#include "RangeCtrl.h"
#include "../CSChair/interface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRangeCtrl

CRangeCtrl::CRangeCtrl():
	m_bEditMode(false),
	m_pRangeList(0),
	m_CurrentContent(Range::undefined)
{
	HRESULT hr = CreateInterface( CHAIR_CONTENT_INTERFACE_EX, (void**)& m_pChairInt );
	ASSERT(hr == S_OK);
}

CRangeCtrl::~CRangeCtrl()
{
	if( m_pChairInt ) m_pChairInt->Release();
}

const UINT c_uSliderId = 12;

BEGIN_MESSAGE_MAP(CRangeCtrl, CWnd)
	//{{AFX_MSG_MAP(CRangeCtrl)
	ON_WM_RBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX( NM_CUSTOMDRAW, c_uSliderId, OnNotifyDraw )
END_MESSAGE_MAP()

int GetRangeHeight( int nAllHeight )
{
	return nAllHeight/3;
}

int GetSliderHeight( int nAllHeight )
{
	return nAllHeight - GetRangeHeight(nAllHeight);
}

/////////////////////////////////////////////////////////////////////////////
// CRangeCtrl message handlers

BOOL	CRangeCtrl::Create( DWORD dwStyle, const CRect& rcWnd, CWnd* pParentWnd, UINT nID )
{
	LPCTSTR szClassName = AfxRegisterWndClass( 0 );
	if( !CWnd::Create( szClassName, _T(""), dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rcWnd, pParentWnd, nID) )
	{	
		TRACE(_T("Failed to create range selector\n"));
		return FALSE;
	}

	CRect rcSlider( 0, 0, rcWnd.Width(), GetSliderHeight(rcWnd.Height()) );
	if( !m_wndSlider.Create( WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_FIXEDLENGTH | TBS_TOOLTIPS | TBS_ENABLESELRANGE | TBS_NOTICKS, rcSlider, this, c_uSliderId ) )
	{
		TRACE(_T("Failed to create slider control\n"));
		return FALSE;
	}

	m_Colors[ Range::unknown	]	= RGB(0,	255,		0);
	m_Colors[ Range::human		]	= RGB(0,	255,		255);
	m_Colors[ Range::clothes	]	= RGB(255,	0,		0);
	m_Colors[ Range::undefined	]	= RGB(128,	128,	128);
	m_Colors[ Range::empty		]	= RGB(255,	255,	0);

	m_wndSlider.SetRange( 0, 100 );
    m_wndSlider.SetPos( 0 );
	m_wndSlider.SendMessage(  TBM_SETTHUMBLENGTH, GetSliderHeight(rcWnd.Height()) - 2, 0);
//	m_Menu.LoadMenu( IDR_MENU1 );

#if 0	// test
	std::pair<IntersectResult, int > p;
	p = InsertSegment(	std::make_pair( 40, 80 ) , std::make_pair( 20, 60 ) );
	ASSERT( p.first == ir_right );
	p = InsertSegment(	std::make_pair( 20, 60 ) , std::make_pair( 40, 80 ) );
	ASSERT( p.first == ir_left );
	p = InsertSegment(	std::make_pair( 20, 80 ) , std::make_pair( 30, 60 ) );
	ASSERT( p.first == ir_erase );
	p = InsertSegment(	std::make_pair( 20, 40 ) , std::make_pair( 50, 60 ) );
	ASSERT( p.first == ir_none );
	p = InsertSegment(	std::make_pair( 20, 40 ) , std::make_pair( 10, 60 ) );
	ASSERT( p.first == ir_divide );
#endif
	return TRUE;
}

void CRangeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CPoint pt(point);
	ClientToScreen(&pt);

	m_Menu.GetSubMenu(0)->TrackPopupMenu( TPM_RETURNCMD|TPM_NONOTIFY, pt.x, pt.y, this );
	CWnd::OnRButtonDown(nFlags, point);
}

void CRangeCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
	CRect rcWnd;
	GetClientRect(&rcWnd);
	CRect rcSlider( 0, GetSliderHeight (rcWnd.Height()), rcWnd.Width(), rcWnd.Height());
	//rcSlider = GetSliderRect(rcSlider);
	CRect rc;
	
	m_wndSlider.GetThumbRect(rc);
	const int BORDERSIZE = 2;
	rcSlider.left	+= BORDERSIZE + rc.Width();
	rcSlider.right	-= BORDERSIZE + rc.Width();
	dc.FillSolidRect( rcSlider, m_Colors[Range::undefined] );


	if( !m_pRangeList) return;
	RangeList_t::const_iterator it = m_pRangeList->begin();
	for( ; it != m_pRangeList->end(); ++it )
	{
		int nStart	= SliderToPhysicPos( rcSlider.left, rcSlider.right, it->m_nStart);
		int nEnd	= SliderToPhysicPos( rcSlider.left, rcSlider.right, it->m_nEnd	);
		dc.FillSolidRect( CRect( nStart, rcSlider.top, nEnd, rcSlider.bottom ), m_Colors[it->m_nContent] );
	}
}

void CRangeCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( pScrollBar->GetSafeHwnd() == m_wndSlider.GetSafeHwnd() )
	{
		if( m_bEditMode )	
		{
			std::pair<int,int> Range = GetCurrentRange() ;

			m_wndSlider.SetSelection( Range.first, Range.second );
			m_wndSlider.Invalidate();
		}
		GetOwner()->SendMessage( WM_HSCROLL, MAKELONG( nPos, nSBCode ), (LPARAM)GetSafeHwnd() );
	}
	
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void	CRangeCtrl::SetRange(int nMin, int nMax)
{
	m_wndSlider.SetRange( nMin, nMax );
/*	if( m_pRangeList )
		m_pRangeList->clear();*/
}

void	CRangeCtrl::StartEdit	( /*Range::CONTENT cont*/ )
{
	m_bEditMode			= true;
//	m_CurrentContent	= cont;
	m_nStartPos = m_wndSlider.GetPos();
}

void	CRangeCtrl::CancelEdit	()
{
	if( m_bEditMode )
	{
		m_bEditMode = false;
		m_wndSlider.SetSelection(0,0);
		Invalidate();
		m_wndSlider.Invalidate();
	}
}

void	CRangeCtrl::EndEdit		()
{
	if( m_bEditMode )
	{
		TRACE1("Pointer %p\n", m_pRangeList);
		m_bEditMode = false;
		m_wndSlider.SetSelection(0,0);
		std::pair<int,int> Range = GetCurrentRange() ;
		if( m_CurrentContent == Range::undefined )
			m_pChairInt->RemoveFromRangeList( *m_pRangeList, Range.first, Range.second );
		else
			m_pChairInt->AddToRangeList( *m_pRangeList, Range.first, Range.second, m_CurrentContent );
		Invalidate();
		m_wndSlider.Invalidate();
	}
	else 
		ASSERT(FALSE);
}

void CRangeCtrl::DrawChannel(CDC& dc, const CRect& rc)
{
	CRect rcTemp(rc);
	dc.DrawEdge( & rcTemp, EDGE_SUNKEN, BF_RECT);
		dc.FillSolidRect( rc.left+2, rc.top+2, (rc.right-rc.left)-4,
                (rc.bottom-rc.top)-4, GetSysColor( COLOR_BTNHIGHLIGHT ) );

		int nSelMin, nSelMax;
		m_wndSlider.GetSelection( nSelMin, nSelMax );
		

        // now highlight the selection range
        if ((m_wndSlider.GetStyle() & TBS_ENABLESELRANGE) &&
            (nSelMin <= nSelMax) /*&& (ptb->lSelEnd > ptb->lLogMin)*/) 
		{
                int iStart, iEnd;

                iStart = SliderToPhysicPos( rc.left, rc.right, nSelMin );
                iEnd   = SliderToPhysicPos( rc.left, rc.right, nSelMax );

                if (iStart + 2 <= iEnd) 
				{
                      //  SetBkColor(ptb->hdc, g_clrHighlight);
                    dc.FillSolidRect( iStart+1, rc.top+3,
                                iEnd-iStart-1, (rc.bottom-rc.top)-6, m_Colors[m_CurrentContent]);
                }
        }
}

BOOL CRangeCtrl::OnNotifyDraw( UINT nID, NMHDR * pNotifyStruct, LRESULT * result  )
{
	if( c_uSliderId != nID ) return FALSE;
	NMCUSTOMDRAW* pDraw = (NMCUSTOMDRAW*)pNotifyStruct;
//	if( pDraw->dwItemSpec == TBCD_CHANNEL )
	{
		switch(pDraw->dwDrawStage) 
		{
		case CDDS_PREPAINT :
			*result  = CDRF_NOTIFYITEMDRAW;
			return TRUE;
		case CDDS_ITEMPREPAINT:
			if( pDraw->dwItemSpec == TBCD_CHANNEL )
			{
			*result = CDRF_SKIPDEFAULT ;
			CDC dc; dc.Attach( pDraw->hdc );
			DrawChannel(dc, pDraw->rc);
			dc.Detach();
			}
			else
				*result = CDRF_DODEFAULT ;
			return TRUE;
		}
	}
	return FALSE;
	//else *result = CDRF_DODEFAULT ;
}



void CRangeCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	//CRect rcSlider( 0, 0, cx, GetSliderHeight(cy) );
	if( ::IsWindow( m_wndSlider.GetSafeHwnd() ) )
		m_wndSlider.SetWindowPos( 0, 0, 0, cx, GetSliderHeight(cy), SWP_NOZORDER|SWP_NOMOVE );
	// TODO: Add your message handler code here
	
}

void	CRangeCtrl::SetContent(RangeList_t* rl)	
{ 
	TRACE1("Pointer %p\n", rl);
	m_pRangeList = rl; 
	Invalidate();
}

void	CRangeCtrl::SetCurrentRangeContent( Range::CONTENT cont )
{
	m_CurrentContent = cont;
	Invalidate(FALSE);
}
