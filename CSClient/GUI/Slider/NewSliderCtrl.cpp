// NewSliderCtrl.cpp : implementation file
//

#include "stdafx.h"
//#include "Slider.h"
#include "NewSliderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewSliderCtrl

CNewSliderCtrl::CNewSliderCtrl():
	m_nMaxValue(200),
	m_nCurValue(0),
	m_bDragMode(false),
	m_pReceiver(0)
	
{
}

CNewSliderCtrl::~CNewSliderCtrl()
{
}


BEGIN_MESSAGE_MAP(CNewSliderCtrl, CWnd)
	//{{AFX_MSG_MAP(CNewSliderCtrl)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNewSliderCtrl message handlers

void CNewSliderCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
	CRect rc, rc2;
	GetClientRect( &rc );
	GetClientRect( &rc2 );

	CDC MemDC;
	MemDC.CreateCompatibleDC( &dc );
	CBitmap Bmp;
	Bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	MemDC.SelectObject( &Bmp );

	Draw(&MemDC);

	dc.BitBlt( 0, 0, rc2.Width(), rc2.Height(), &MemDC, 0, 0, SRCCOPY );
}

void	CNewSliderCtrl::Step()
{
	//if( ++m_dwCurValue > m_dwMaxValue) m_dwCurValue = 0; 
	++m_nCurValue;
	Normalize();
	Invalidate(FALSE);
}

int CNewSliderCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}



void CNewSliderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bDragMode)
	{
		CRect rc;
		GetClientRect(&rc);		
		m_nCurValue = ValueType( double(point.x * m_nMaxValue) / rc.Width() );
		Normalize();
		Invalidate(FALSE);
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CNewSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;
	GetClientRect(&rc);

	m_nCurValue = ValueType( double(point.x * m_nMaxValue) / rc.Width() );
	Normalize();

	m_bDragMode = true;

	SetCapture();
	SetTimer(0, 300, 0);
	Invalidate(FALSE);
	if(m_pReceiver) m_pReceiver->SendMessage(wm_slider, sm_start_drag, 0);
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CNewSliderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	OnCancelMode();
	CWnd::OnLButtonUp(nFlags, point);
}

void	CNewSliderCtrl::Normalize()
{
	if(m_nCurValue < 0) m_nCurValue = 0;
	else if(m_nCurValue > m_nMaxValue) m_nCurValue = m_nMaxValue;
}

BOOL CNewSliderCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	UNREFERENCED_PARAMETER(pDC);
	return /*CWnd::OnEraseBkgnd(pDC)*/TRUE;
}

void Gradient(CDC* pDC, const CRect& rc, COLORREF cr1, COLORREF cr2)
{
	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert [0] .x      = rc.left;
	vert [0] .y      = rc.top;
	vert [0] .Red    = GetRValue(cr1) << 8;
	vert [0] .Green  = GetGValue(cr1) << 8;
	vert [0] .Blue   = GetBValue(cr1) << 8;
	vert [0] .Alpha  = 0;

	vert [1] .x      = rc.right;
	vert [1] .y      = rc.bottom; 
	vert [1] .Red    = GetRValue(cr2) << 8;
	vert [1] .Green  = GetGValue(cr2) << 8;
	vert [1] .Blue   = GetBValue(cr2) << 8;
	vert [1] .Alpha  = 0;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill(pDC->GetSafeHdc() ,vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
}

void	CNewSliderCtrl::	Draw(CDC* pDC) const 
{
	CRect rc, rc2;
	GetClientRect( &rc );
	GetClientRect( &rc2 );

	COLORREF cr1 = GetSysColor( COLOR_ACTIVEBORDER );
	pDC->FillSolidRect( rc, cr1 );
	rc.right = rc.left + DWORD(rc.Width()*double(m_nCurValue)/m_nMaxValue);
//	pDC->FillSolidRect( rc, RGB(0,0,128) );
/*
	if( rc.right - rc.left > 5 )
	{
		int a =1;
	}
*/
	IsWindowEnabled() ? Gradient(pDC, rc, GetSysColor( COLOR_GRADIENTACTIVECAPTION ), GetSysColor( COLOR_ACTIVECAPTION )) :
						Gradient(pDC, rc, GetSysColor( COLOR_GRADIENTINACTIVECAPTION ), GetSysColor( COLOR_INACTIVECAPTION ));

	CPen pen;
	cr1 = /*IsWindowEnabled() ? */GetSysColor( COLOR_WINDOW )/* : GetSysColor( COLOR_BTNFACE )*/;
	pen.CreatePen(PS_SOLID, 1, cr1 );
	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	CPen* pOldPen = (CPen*)pDC->SelectObject(&pen);
	pDC->RoundRect( rc2.left, rc2.top, rc2.right-1, rc2.bottom-1, rc2.Height()/2, rc2.Height() );
	pDC->SelectObject( pOldPen );
	pDC->SelectObject( pOldBrush );
}

void CNewSliderCtrl::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
#if 0
	if(m_pReceiver) m_pReceiver->SendMessage(wm_slider, sm_change_pos, 0);
#endif
	CWnd::OnTimer(nIDEvent);
}

void CNewSliderCtrl::OnSize( UINT nType, int cx, int cy )
{
	CWnd::OnSize(nType, cx, cy);
	CRgn m_rgn;
   // destroy old region
   m_rgn.DeleteObject();
   // create rounded rect region based on new window size
   CRect rc;                  // rounded rectangle 
   GetWindowRect(&rc);        // window rect in screen coords
   rc -= rc.TopLeft();        // convert to window coords: top left = (0,0)
   m_rgn.CreateRoundRectRgn(rc.left,rc.top,rc.right,rc.bottom,
      rc.Height()/ 2, rc.Height()/2);                 // rounded rect w/50 pixel corners
   SetWindowRgn(m_rgn,TRUE);  // set window region to make rounded window
}

void CNewSliderCtrl::OnCancelMode()
{
	if( m_bDragMode )
	{
	m_bDragMode = false;
	ReleaseCapture();
	Invalidate(FALSE);
	KillTimer(0);
	if(m_pReceiver) m_pReceiver->SendMessage(wm_slider, sm_end_drag, 0);
	OutputDebugString(_T("process OnCancelMode\n"));
	}
}