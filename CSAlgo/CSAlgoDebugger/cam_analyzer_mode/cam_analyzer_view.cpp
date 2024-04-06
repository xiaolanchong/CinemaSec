#include "stdafx.h"
#include "../algo_debugger.h"
#include "cam_analyzer_doc.h"
#include "cam_analyzer_view.h"
#include "cam_analyzer_frm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCamAnalyzerView, CView)

BEGIN_MESSAGE_MAP(CCamAnalyzerView, CView)
  //{{AFX_MSG_MAP(CCamAnalyzerView)
  ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CCamAnalyzerView::CCamAnalyzerView()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CCamAnalyzerView::~CCamAnalyzerView()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
void CCamAnalyzerView::OnInitialUpdate()
{
  CView::OnInitialUpdate();
}


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework to render an image of the document. */
//-------------------------------------------------------------------------------------------------
void CCamAnalyzerView::OnDraw( CDC * pDC )
{
  CCamAnalyzerDoc * pDoc = GetDocument();

  if ((pDoc != 0) && pDoc->m_layout.ready())
  {
    csutility::DrawColorImage( &(pDoc->m_demoImg.data.second), pDC->GetSafeHdc(), pDoc->m_layout.rectangle() );
  }
}


//-------------------------------------------------------------------------------------------------
// CCamAnalyzerView diagnostics.
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CCamAnalyzerView::AssertValid() const
{
  CView::AssertValid();
}
void CCamAnalyzerView::Dump( CDumpContext & dc ) const
{
  CView::Dump(dc);
}
CCamAnalyzerDoc * CCamAnalyzerView::GetDocument() const
{
  ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CCamAnalyzerDoc ) ) );
  return (CCamAnalyzerDoc*)m_pDocument;
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function after the window's size has changed. */
//-------------------------------------------------------------------------------------------------
void CCamAnalyzerView::OnSize( UINT nType, int cx, int cy )
{
  CView::OnSize( nType, cx, cy );
  CCamAnalyzerDoc * pDoc = GetDocument();
  if (pDoc != 0)
    pDoc->m_layout.recalculate( pDoc->m_demoImg.data.second.width(),
                                pDoc->m_demoImg.data.second.height(), GetSafeHwnd() );
  Invalidate( TRUE );
}


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework after the view's document has been modified;
           this function is called by CDocument::UpdateAllViews and allows the view
           to update its display to reflect those modifications. */
//-------------------------------------------------------------------------------------------------
void CCamAnalyzerView::OnUpdate( CView * pSender, LPARAM lHint, CObject * pHint )
{
  pSender;lHint;pHint;
  CCamAnalyzerDoc * pDoc = GetDocument();
  if (pDoc != 0)
    pDoc->m_layout.recalculate( pDoc->m_demoImg.data.second.width(),
                                pDoc->m_demoImg.data.second.height(), GetSafeHwnd() );
  Invalidate( TRUE );
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function
           when the CWnd object background needs erasing. */
//-------------------------------------------------------------------------------------------------
BOOL CCamAnalyzerView::OnEraseBkgnd( CDC * pDC ) 
{
  CRect clientRect;
  GetClientRect( &clientRect );
  if (!(clientRect.IsRectEmpty()))
    pDC->FillSolidRect( &clientRect, RGB(255,240,200) );
  return TRUE;
	// return CView::OnEraseBkgnd(pDC);
}


