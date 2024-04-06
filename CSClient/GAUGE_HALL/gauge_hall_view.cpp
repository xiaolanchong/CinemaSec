#include "stdafx.h"
//#include "cinemapark.h"
#include "gauge_hall_doc.h"
#include "gauge_hall_view.h"
#include "gauge_hall_frm.h"
#include "res/resource.h"
#include "../gui/Grid/MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGaugeHallView, BaseIdleView)

BEGIN_MESSAGE_MAP(CGaugeHallView, BaseIdleView)
  //{{AFX_MSG_MAP(CGaugeHallView)
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_RBUTTONDOWN()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_CANCELMODE()
  ON_WM_ERASEBKGND()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CGaugeHallView::CGaugeHallView()
{
  // Compute colors which are used for blinking.
  int nColor = ALIB_LengOf( m_blinkColors );
  ASSERT( (nColor & 1) == 0 );
  for (int k = 0; k < nColor/2; k++)
  {
    int i = (k*255)/(nColor/2-1);
    m_blinkColors[k] = (m_blinkColors[nColor-1-k] = RGB(255-i,i,0));
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CGaugeHallView::~CGaugeHallView()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when an application requests that the Windows
           window be created by calling the Create or CreateEx member function. */
//-------------------------------------------------------------------------------------------------
int CGaugeHallView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
  if (BaseIdleView::OnCreate( lpCreateStruct ) == -1)
    return -1;

#if 0
  if (!(m_menu.CreatePopupMenu()))
    return -1;

  m_menu.AppendMenu( MF_STRING, IDM_Edit_ObjectSelection, _T("Object Selection") );
  m_menu.AppendMenu( MF_STRING, IDM_Edit_DeleteObject,    _T("Delete") );
  m_menu.AppendMenu( MF_STRING, MF_SEPARATOR );
  m_menu.AppendMenu( MF_STRING, IDM_Edit_ObjectContouring, _T("Object Contouring") );
  m_menu.AppendMenu( MF_STRING, IDM_Edit_CurveAdjusting,   _T("Curve Adjusting") );
  m_menu.AppendMenu( MF_STRING, IDM_Edit_CurveSmoothing,   _T("Curve Smoothing") );
  m_menu.AppendMenu( MF_STRING, MF_SEPARATOR );
  m_menu.AppendMenu( MF_STRING, IDM_Edit_AdjustingRadius_1, _T("adjusting radius = 1") );   
  m_menu.AppendMenu( MF_STRING, IDM_Edit_AdjustingRadius_2, _T("adjusting radius = 2") ); 
  m_menu.AppendMenu( MF_STRING, IDM_Edit_AdjustingRadius_3, _T("adjusting radius = 3") );   
  m_menu.AppendMenu( MF_STRING, IDM_Edit_AdjustingRadius_4, _T("adjusting radius = 4") );    
  m_menu.AppendMenu( MF_STRING, IDM_Edit_AdjustingRadius_5, _T("adjusting radius = 5") ); 
  m_menu.AppendMenu( MF_STRING, IDM_Edit_AdjustingRadius_6, _T("adjusting radius = 6") );
  m_menu.AppendMenu( MF_STRING, IDM_Edit_AdjustingRadius_7, _T("adjusting radius = 7") );
  m_menu.AppendMenu( MF_STRING, IDM_Edit_AdjustingRadius_8, _T("adjusting radius = 8") );  
  m_menu.AppendMenu( MF_STRING, IDM_Edit_AdjustingRadius_9, _T("adjusting radius = 9") );    
#else
  if (!(m_menu.LoadMenu( IDR_GaugeHallTYPE )))
	  return -1;
#endif
  return 0;
}
/*
static bool bGrabbed	= false;
static bool bInited		= false;
*/
//-------------------------------------------------------------------------------------------------
/** \brief Function draws grayscaled image.

  \param  image     the image to be drawn.
  \param  hdc       device context.
  \param  rect      destination rectangle.
  \param  bInvertY  true, if Y-axis must be inverted.
  \param  bColor16  true, if the first 16 palette entries were reserved for system colors. */
//-------------------------------------------------------------------------------------------------
void MyDrawGrayImageEx( const BYTE* pByte, size_t size, int Width, int Height, HDC hdc, CRect rect, bool bInvertY, bool bColor16 )
{
  int W = Width;
  int H = Height;

  if (W & 3)
  {
    TRACE( _T("The image width must be aligned by 4 bytes") );
    return;
  }
  if (size == 0)
    return;

  // Make 256-color bitmap header.
  ubyte        buf[ sizeof(BITMAPINFO)+256*sizeof(RGBQUAD) ];
  BITMAPINFO * pbi = reinterpret_cast<BITMAPINFO*>( &(buf[0]) );

  memset( buf, 0, sizeof(buf) );
  pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
  pbi->bmiHeader.biWidth = W; 
  pbi->bmiHeader.biHeight = bInvertY ? -H : +H;
  pbi->bmiHeader.biPlanes = 1; 
  pbi->bmiHeader.biBitCount = 8; 
  pbi->bmiHeader.biCompression = BI_RGB;
  pbi->bmiHeader.biSizeImage = size * sizeof(Arr2ub::value_type);

  // Fill palette.
  MyInitColorGrayPalette( &(pbi->bmiColors[0]), 256, bColor16 );

  // Draw the image.
  int   newMode = ((rect.Width() < W) || (rect.Height() < H)) ? HALFTONE : COLORONCOLOR;
  int   oldMode = ::SetStretchBltMode( hdc, newMode );
  POINT point;

  point.y = (point.x = 0);
  if (newMode == HALFTONE)
    ::SetBrushOrgEx( hdc, 0, 0, &point );

  ::StretchDIBits( hdc, rect.left, rect.top, rect.Width(), rect.Height(),
                   0, 0, W, H,pByte, pbi, DIB_RGB_COLORS, SRCCOPY );

  if (newMode == HALFTONE)
    ::SetBrushOrgEx( hdc, point.x, point.y, 0 );

  ::SetStretchBltMode( hdc, oldMode );
}

//-------------------------------------------------------------------------------------------------
/** \brief CGaugeHallView drawing. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallView::OnDraw( CDC * pDC )
{
	CGaugeHallDoc * pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CMemDC MemDC(pDC);
	pDoc->DrawView(MemDC);
}


//-------------------------------------------------------------------------------------------------
// CGaugeHallView diagnostics.
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CGaugeHallView::AssertValid() const
{
  CView::AssertValid();
}
void CGaugeHallView::Dump( CDumpContext & dc ) const
{
  CView::Dump( dc );
}
CGaugeHallDoc * CGaugeHallView::GetDocument() // debug version
{
  ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CGaugeHallDoc ) ) );
  return (CGaugeHallDoc*)m_pDocument;
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief Function performs idle-time processing. */
//-------------------------------------------------------------------------------------------------
bool CGaugeHallView::OnIdle()
{
  static clock_t  pre = ::clock();
  clock_t         cur = ::clock();
  CGaugeHallDoc * pDoc = GetDocument();

  if ((pDoc != 0) && (((cur-pre)<<5) >= CLOCKS_PER_SEC))
  {
    CClientDC dc( this );
//	CMemDC MemDC(&dc);

    if (pDoc->GetCountouting())
    {
      static int n = 0;
      int        nPoint = (int)(pDoc->GetCurPatch().size());

      for (int k = 0; k < nPoint; k++)
      {
        dc.SetPixel( (POINT&)(pDoc->GetCurPatch()[k]), m_blinkColors[n] );
      }
      n = (n+1) % ALIB_LengOf( m_blinkColors );
    }
    else
    {
		pDoc->DrawObject(dc);
    }
    pre = cur;
  }
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework after the view is first attached to the document,
           but before the view is initially displayed. */
//-------------------------------------------------------------------------------------------------

void CGaugeHallView::OnInitialUpdate()
{
	
	CGaugeHallDoc * pDoc = GetDocument();
	UNREFERENCED_PARAMETER(pDoc);
	
	static bool bInited = false;
	if(! bInited ) 
	{	
		pDoc->OnInit();
		bInited = true;
	}
	BaseIdleView::OnInitialUpdate();
	SetScrollSizes(MM_TEXT , CSize(1,1));
}

UINT CGaugeHallView::InvokeGrabDialog(CGaugeHallDoc* pDoc)
{
	UNREFERENCED_PARAMETER(pDoc);
	return 0;
}

//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework after the view's document has been modified; this function is
           called by CDocument::UpdateAllViews and allows the view to update its display
           to reflect those modifications. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallView::OnUpdate( CView * pSender, LPARAM Hint, CObject * pHint )
{
  pSender;Hint;pHint;
  CGaugeHallDoc * pDoc = GetDocument();
  if (pDoc != 0)
    pDoc->RecalcLayout( true );
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function after the window's size has changed. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallView::OnSize( UINT nType, int cx, int cy )
{
  BaseIdleView::OnSize( nType, cx, cy );
  CGaugeHallDoc * pDoc		= GetDocument();
  CGaugeHallFrame* pFrame	= STATIC_DOWNCAST(  CGaugeHallFrame, GetParentFrame() );
  if (pDoc != 0 && pFrame && pFrame->IsReady() )
    pDoc->RecalcLayout( false );
/*

  int w = 640*2;//pDoc->m_FrameAcc.GetHeader().biWidth;
  int h = 480*3;//pDoc->m_FrameAcc.GetHeader().biHeight;

  pDoc->m_layout.recalculate( w, h, this );*/
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when the user presses the left mouse button. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallView::OnLButtonDown( UINT nFlags, CPoint point )
{
  CGaugeHallDoc * pDoc = GetDocument();
  if (pDoc != 0)
    pDoc->OnLButtonDownHandler( nFlags, point );
  BaseIdleView::OnLButtonDown( nFlags, point );
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when the user releases the left mouse button. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallView::OnLButtonUp( UINT nFlags, CPoint point )
{
  CGaugeHallDoc * pDoc = GetDocument();
  if (pDoc != 0)
    pDoc->OnLButtonUpHandler( nFlags, point );
  BaseIdleView::OnLButtonUp( nFlags, point );
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when the mouse cursor moves. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallView::OnMouseMove( UINT nFlags, CPoint point )
{
  CGaugeHallDoc * pDoc = GetDocument();
  if (pDoc != 0)
    pDoc->OnMouseMoveHandler( nFlags, point );
  BaseIdleView::OnMouseMove( nFlags, point );
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when the user presses the right mouse button. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallView::OnRButtonDown( UINT nFlags, CPoint point )
{
  CGaugeHallDoc * pDoc = GetDocument();
  if (pDoc == 0) return;
  CMenu* pMenu = m_menu.GetSubMenu(0);
  pDoc->OnRButtonDownHandler(this, nFlags, point, pMenu);
  BaseIdleView::OnRButtonDown( nFlags, point );
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when a nonsystem key is pressed. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallView::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
  CGaugeHallDoc * pDoc = GetDocument();
  if (pDoc != 0)
    pDoc->OnKeyDownHandler( nChar, nRepCnt, nFlags );
  BaseIdleView::OnKeyDown( nChar, nRepCnt, nFlags );
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when a nonsystem key is released. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallView::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
  CGaugeHallDoc * pDoc = GetDocument();
  if (pDoc != 0)
    pDoc->OnKeyUpHandler( nChar, nRepCnt, nFlags );
  BaseIdleView::OnKeyUp( nChar, nRepCnt, nFlags );
}


void CGaugeHallView::OnCancelMode()
{
  CGaugeHallDoc * pDoc = GetDocument();
  if (pDoc != 0)
    pDoc->OnCancelMode( );
  BaseIdleView::OnCancelMode(  );	
}

bool	CGaugeHallView::SetImageButtonText(const std::vector<int>& Images )
{
	CFrameWnd* pWnd = GetParentFrame();
	CGaugeHallFrame * pFrame = dynamic_cast<CGaugeHallFrame*> (pWnd);
	if(pFrame)
	{
		return pFrame->SetImageButtonText(Images);
	}	
	else return false; 
}

// don't flick
BOOL CGaugeHallView::OnEraseBkgnd(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
	return TRUE;
}
