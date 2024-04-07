#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "res/resource.h"
#include "gauge_hall/gauge_hall_doc.h"
#include "gauge_hall/gauge_hall_view.h"
#include "gauge_hall/gauge_hall_frm.h"
#include "FileDialogEx.h"
#include "../Chair/ChairSerializer.h"
#include "../DBFacet/CfgDB.h"
#include "../DBFacet/DBSerializer.h"
#include "../../CSEngine/DBFacet/CinemaDB.h"
#include "../../CSAlgo/interfaces/parameter_dialog.h"

#include "../LongOperation.h"
#include "../DBFacet/DBSerializer.h"
#include "../DBFacet/CfgDB.h"
#include "../Video/CameraManager.h"
#include "../Layout/CameraGrabDialog.h"
#include "../Layout/LayoutDialog.h"
#include "../Layout/FileGrabDialog.h"
#include "../Layout/LayoutValidate.h"
#include "../Layout/RenumberDialog.h"
#include "../Layout/LayoutIDDialog.h"
#include "../Layout/SelectArrangementDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

template <class InIt, class OutIt> void AssignCurve( InIt start, InIt end, OutIt copy )
{
	for( ; start != end; ++start, ++ copy)
	{
		copy->x = start->x;
		copy->y = start->y;
	}
}

IMPLEMENT_DYNCREATE(CGaugeHallDoc, CDocument)

BEGIN_MESSAGE_MAP(CGaugeHallDoc, CDocument)
  //{{AFX_MSG_MAP(CGaugeHallDoc)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)

	ON_COMMAND(ID_FILE_SAVE_GRAPH, OnFileSaveGraph )
	ON_COMMAND(ID_FILE_LOAD_GRAPH, OnFileLoadGraph )
	ON_COMMAND(ID_FILE_LOAD_IMAGE, OnFileLoadImage )
	ON_COMMAND(ID_FILE_SAVE_IMAGE, OnFileSaveImage )

	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_GRAPH, OnFileSaveGraphUpdate )
	ON_UPDATE_COMMAND_UI(ID_FILE_LOAD_GRAPH, OnFileLoadGraphUpdate )
	ON_UPDATE_COMMAND_UI(ID_FILE_LOAD_IMAGE, OnFileLoadImageUpdate )
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_IMAGE, OnFileSaveImageUpdate )

	ON_COMMAND( ID_DELETE_ITEM, OnDeleteItem)
	ON_UPDATE_COMMAND_UI( ID_DELETE_ITEM, OnDeleteItemUpdate )

	ON_COMMAND(ID_DB_IMAGE_LOAD, OnDBImageLoad )
	ON_COMMAND(ID_DB_IMAGE_SAVE, OnDBImageSave )
	ON_COMMAND(ID_DB_LOAD_GRAPH, OnDBGraphLoad )
	ON_COMMAND(ID_DB_SAVE_GRAPH, OnDBGraphSave )
	

	ON_UPDATE_COMMAND_UI(ID_DB_IMAGE_LOAD, OnDBImageLoadUpdate )
	ON_UPDATE_COMMAND_UI(ID_DB_IMAGE_SAVE, OnDBImageSaveUpdate )
	ON_UPDATE_COMMAND_UI(ID_DB_LOAD_GRAPH, OnDBGraphLoadUpdate )
	ON_UPDATE_COMMAND_UI(ID_DB_SAVE_GRAPH, OnDBGraphSaveUpdate )

	ON_COMMAND( ID_IMAGE_GRAB, OnImageGrab )
	ON_COMMAND( ID_NEW_GRAPH, OnNewGraph )

	ON_COMMAND( ID_CLONE, OnClone )

	ON_COMMAND( ID_GRAB_CAMERA, OnGrabCamera )
	ON_UPDATE_COMMAND_UI(ID_GRAB_CAMERA, OnGrabCameraUpdate )

	ON_COMMAND( ID_LAYOUT, OnLayout )
	ON_UPDATE_COMMAND_UI(ID_LAYOUT, OnLayoutUpdate )

	ON_COMMAND( ID_SHOW_PARAM, OnParamShow )
	ON_UPDATE_COMMAND_UI( ID_SHOW_PARAM, OnParamShowUpdate)

	ON_COMMAND( ID_DB_SAVE_MODEL, OnDBSaveModel )
	ON_UPDATE_COMMAND_UI( ID_DB_SAVE_MODEL, OnDBSaveModelUpdate )

	ON_COMMAND( ID_MOVE_CHAIR, OnMoveChair )
	ON_UPDATE_COMMAND_UI( ID_MOVE_CHAIR, OnMoveChairUpdate )

	ON_COMMAND( ID_MOVE_IMAGE, OnMoveImage )
	ON_UPDATE_COMMAND_UI( ID_MOVE_CHAIR, OnMoveImageUpdate )

  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

class LongOperationAnim : public LongOperation
{
public:
	LongOperationAnim( HWND hParentWnd ) :
	  LongOperation (hParentWnd , GetModuleHandle(0), IDS_LONGOPERATION, IDA_GEAR ) {}

};

//-------------------------------------------------------------------------------------------------
/** \brief Function specifies decay of influence in local editing operations. */
//-------------------------------------------------------------------------------------------------
static inline double ShiftFunc( double dist, double radius )
{
  return exp( -0.5*(dist*dist)/(radius*radius) );
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CGaugeHallDoc::CGaugeHallDoc() : 
	m_bStandAlone(false),
	m_cbMoveChair(this),
	m_cbMoveImage(this)
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CGaugeHallDoc::~CGaugeHallDoc()
{
}


//-------------------------------------------------------------------------------------------------
// CGaugeHallDoc diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CGaugeHallDoc::AssertValid() const
{
  CDocument::AssertValid();
}
void CGaugeHallDoc::Dump( CDumpContext & dc ) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief Function returns pointer to CGaugeHallView class. */
//-------------------------------------------------------------------------------------------------
CGaugeHallView * CGaugeHallDoc::GetView()
{
  for (POSITION pos = GetFirstViewPosition(); pos != 0;)
  {
    CView * p = GetNextView( pos );
    if ((p != 0) && p->IsKindOf( RUNTIME_CLASS( CGaugeHallView ) ))
    {
      return reinterpret_cast<CGaugeHallView*>( p );
    }
  }
  return 0;
}

//-------------------------------------------------------------------------------------------------
/** \brief Implements the ID_FILE_SAVE_AS command. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::OnFileSaveAs()
{
}

void CGaugeHallDoc::OnDeleteItem()
{
#if 0
	AllImageData& All = *m_AllImageData[ GetCurrentRoom() ];
	if( All.HasSelectedChair()  )
	{
		All.DeleteCurrentChair();
	}
	if( All.HasSelectedLink() )
	{
		All.DeleteCurrentLink();
	}
	UpdateAllViews(0);
#else
	if(IsViewAll())
	{
		int nRoomID = GetCurrentRoom();
		AllImageData&	All	= *m_AllImageData[ nRoomID ];
		All.DeleteCurrentChair();
	}
	else
	{
		GetImageData().DeleteCurrentChair();
	}
#endif
}

void CGaugeHallDoc::OnDeleteItemUpdate(CCmdUI* pCmdUI)
{
#if 0
	AllImageData& All = *m_AllImageData[ GetCurrentRoom() ];
	bool bRes = IsViewAll() && ( All.HasSelectedChair()  || All.HasSelectedLink()   );

	pCmdUI->Enable( bRes? TRUE : FALSE  );
#else
	if(IsViewAll())
	{
		int nRoomID = GetCurrentRoom();
		AllImageData& All = *m_AllImageData[ nRoomID ];
		bool bRes = All.HasSelectedChair();
		pCmdUI->Enable( bRes? TRUE : FALSE  );
	}
	else
	{
		bool bRes = GetImageData().HasSelectedChair();
		pCmdUI->Enable( bRes? TRUE : FALSE  );
	}
#endif
}

//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework before a modified document is to be closed. */
//-------------------------------------------------------------------------------------------------
BOOL CGaugeHallDoc::SaveModified()
{
  return TRUE;
}


//-------------------------------------------------------------------------------------------------
/** \brief Handler of LButtonDown message. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::OnLButtonDownHandler( CView* pView, UINT nFlags, CPoint point )
{
  nFlags;point;


  if (m_subMode == IDM_Edit_ObjectSelection) // object's selection sub-mode
  {
    m_nCurrentChair = INVALID_CHAIR;
    for (std::vector<ChairCache>::iterator it = m_ChairCache.begin(); it != m_ChairCache.end(); ++it)
    {
      if (vislib::CurveAndPointRelation( (*it).icurve.begin(), (*it).icurve.end(),
                                         point.x, point.y ) > 0)
      {
        PartialClear();
        m_nCurrentChair = it - m_ChairCache.begin();
        break;
      }
    }
    m_subMode = IDM_Edit_ObjectSelection;
    return; // no screen update
  }
  if (m_subMode == IDM_Edit_ObjectContouring) // object's contouring sub-mode
  {
    // If the contouring has finished and the curve is not empty, then just update the demo-curve.
    if (!m_bContouring && !(m_curve.empty()))
    {
      CopyCurveToPixelCurve( PRODUCE_CLOSED_CURVE );
      return;
    }

    // If cursor is nearby the start point, then enclose curve and finish contouring.
    if (m_bContouring && (!(m_curve.empty()) || !(m_liveSnake.GetCurve().empty())))
    {
      Vec2f first = m_curve.empty() ? m_liveSnake.GetCurve().front() : m_curve.front();
      Vec2f last;

      m_layout.s2p( point, last );
      if (first.distance( last ) <= 3)
      {
        last = first;
        first = m_curve.empty() ? m_liveSnake.GetCurve().front() : m_curve.back();
        CalcSnake( first, last );
        EndContouring(pView);
        return;
      }
    }

    // If contouring is under way and the snake is not empty,
    // then add snake to the current curve and update demo-curve.
    if (m_bContouring && !(m_liveSnake.GetCurve().empty()))
    {
      m_curve.insert( m_curve.end(), m_liveSnake.GetCurve().begin(), m_liveSnake.GetCurve().end() );
      CopyCurveToPixelCurve( false, false );
    }

    // Find the starting point for an empty curve or begin the new patch from the last curve's point.
    if (m_curve.empty())
    {
      CPoint pixel = point;
      m_layout.s2p( pixel, m_headPt );
      m_liveSnake.GetEquilibriumPoint( m_energyGrads, m_headPt );
      m_layout.p2s( m_headPt, pixel );
      
      CPoint cursor = pixel;
      pView->ClientToScreen( &cursor );
      ::SetCursorPos( cursor.x, cursor.y );
    }
    else
    {
      m_headPt = m_curve.back();
    }

    // The current point is the second end point of the snake.
    m_layout.s2p( point, m_tailPt );
    m_curPatch.clear();
    m_liveSnake.Clear();
    m_bContouring = true;
  }
  else if ((m_subMode == IDM_Edit_CurveAdjusting) || // object's adjusting or smoothing sub-mode
           (m_subMode == IDM_Edit_CurveSmoothing))
  {
    // If the currently selected curve was specified, then choose it for editing.
    if ( HasSelectObject() && !(IsSelectedObjectEmpty()))
    {
      Vec2f pt;
      m_curve = GetCurrentChair()->curve;
      m_auxCurve = m_curve;
      m_layout.s2p( point, pt );
      Vec2fArrIt it = vislib::FindNearestPoint( m_curve.begin(), m_curve.end(), pt.x, pt.y );
      m_selPointNo = (it != m_curve.end()) ? (int)(std::distance( m_curve.begin(), it )) : -1;
    }
  }
  m_bEditMode = true;

  pView->Invalidate( FALSE );
}


//-------------------------------------------------------------------------------------------------
/** \brief Handler of LButtonUp message. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::OnLButtonUpHandler( CView* pView, UINT nFlags, CPoint point )
{
  nFlags;point;

  if (pView == 0)
    return;

	if( !m_bEditMode ) return;

  if ((m_subMode == IDM_Edit_CurveSmoothing) || // object's adjusting or smoothing sub-mode
     ((m_subMode == IDM_Edit_CurveAdjusting) &&
       !(alib::AreContainersSimilar( m_curve, m_auxCurve ))) )
  {
    // Push the current curve into undo list.
    if (m_undoCurveLst.size() >= 100)
      m_undoCurveLst.pop_front();
    m_undoCurveLst.push_back( m_curve );

    // Either locally smooth the curve or accept adjusted one.
    if (m_subMode == IDM_Edit_CurveSmoothing)
    {
      LocallySmoothCurve();
      CopyCurveToPixelCurve( PRODUCE_CLOSED_CURVE, true ); // 'true' is better, but overall smoothing via resampling takes place
    }                                                      // 'false' slows down smoothing as point density increases, but has no side effects
    else
    {
      m_curve = m_auxCurve;
      CopyCurveToPixelCurve( PRODUCE_CLOSED_CURVE, false );
    }

    // Update currently selected curve.
    if (HasSelectObject())
    {
      GetCurrentChair()->curve = m_curve;
      GetCurrentChairCache()->icurve.resize( m_pixCurve.size() );
	  AssignCurve(m_pixCurve.begin(), m_pixCurve.end(), GetCurrentChairCache()->icurve.begin() );
	  //GetCurrentChairCache()->icurve.begin ( (const Vec2i*)&(*m_pixCurve.begin()), (const Vec2i*)&(*m_pixCurve.end()) );
      ASSERT( vislib::IsValidCurve( GetCurrentChairCache()->icurve.begin(), GetCurrentChairCache()->icurve.end() ) );
    }

    pView->Invalidate( FALSE );
  }
  m_auxCurve.clear();
  m_bEditMode = false;
}

void CGaugeHallDoc::ImageData::OnCancelMode(CView* pView)
{
	if(m_bEditMode)
	{
      PartialClear();
      RecalcLayout( pView, false );
	  m_bEditMode = false;
      pView->Invalidate( FALSE );
	}
}

//-------------------------------------------------------------------------------------------------
/** \brief Handler of MouseMove message. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::OnMouseMoveHandler( CView* pView, UINT nFlags, CPoint point )
{
  nFlags;point;
 
  if (pView == 0)
    return;

 // if( !m_bEditMode ) return;

  if (m_subMode == IDM_Edit_ObjectContouring) // object's contouring sub-mode
  {
    // Compute and draw an active contour (snake).
    if (m_bContouring)
    {
      CClientDC dc( pView );
      m_layout.s2p( point, m_tailPt );
      CalcSnake( m_headPt, m_tailPt );

      // Draw and edit the last patch of the current curve only.
      DrawPixColArr( m_curPatch, &dc );
      InitPixColArr( m_liveSnake.GetCurve(), &dc, m_curPatch );
      DrawPixColArr( m_curPatch, &dc, true, CURVE_COLOR );
    }
  }
  else if (m_subMode == IDM_Edit_CurveAdjusting) // object's adjusting sub-mode
  {
    // Locally draw out the current curve nearby selected point.
    if (!(m_auxCurve.empty()) && alib::IsRange( m_selPointNo, 0, (int)(m_auxCurve.size()) ))
    {
      CClientDC dc( pView );
      Vec2f     shift, pre, cur;
      double    radius = alib::Sqr( m_adjustRadius );
      double    bound = 5.0*radius;
      int       N = (int)(m_auxCurve.size());

      ASSERT( radius > FLT_EPSILON );
      m_layout.s2p( point, shift );
      shift -= m_auxCurve[ m_selPointNo ];

      // Shift left-side and right-side neighbours of selected point.
      for (int inc = -1; inc <= 1; inc += 2)
      {
        pre = m_auxCurve[ m_selPointNo ];
        double crvLeng = 0.0;

        // The shift decreases as a point moves far away from selected one.
        for (int count = 0, i = (m_selPointNo+inc); count < N; i += inc, count++)
        {
          if (PRODUCE_CLOSED_CURVE)
            cur = m_auxCurve[(i+N)%N];
          else if (ALIB_IS_RANGE(i, 0, N))
            cur = m_auxCurve[i];
          else
            break;

          crvLeng += (cur-pre).length();
          if (crvLeng > bound)
            break;
          m_auxCurve[(i+N)%N] += shift * (float)ShiftFunc( crvLeng, radius );
          pre = cur;
        }
      }
      m_auxCurve[ m_selPointNo ] += shift;
      m_selPointNo = KeepPointDensity( m_auxCurve );

      // Draw the curve in location.
      DrawPixColArr( m_pixCurve, &dc );
      InitPixColArr( m_auxCurve, &dc, m_pixCurve );
      DrawPixColArr( m_pixCurve, &dc, true, CURVE_COLOR );
      m_tailPt = m_headPt;
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Handler of KeyDown message. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::OnKeyDownHandler( CView* pView, UINT nChar, UINT nRepCnt, UINT nFlags )
{
  nChar;nRepCnt;nFlags;

  if (pView == 0)
    return;

  CPoint pixel;
  switch (nChar)
  {
    case VK_ESCAPE:     // drop the currently created contour
      PartialClear();
      RecalcLayout( pView, false );
      pView->Invalidate( FALSE );
      break;

    case VK_RETURN:     // accept the currently created contour and finalize process
      if (m_bContouring)
        EndContouring(pView);
      break;

    case 65: // 'a' - adjusting
    case 67: // 'c' - contouring
    case 83: // 's' - smoothing
      if (!m_bContouring)
      {
        m_subMode = (nChar == 65) ? IDM_Edit_CurveAdjusting   :
                    (nChar == 67) ? IDM_Edit_ObjectContouring : IDM_Edit_CurveSmoothing;
        pView->Invalidate( FALSE );
      }
      break;

/*
    case 69: // 'e', set fixed equilibrium point
      if (m_bContouring && !(m_liveSnake.GetCurve().empty()) && ::GetCursorPos( &pixel ))
      {
        Vec2f point;
        pView->ScreenToClient( &pixel );
        m_layout.s2p( pixel, point );
        m_liveSnake.GetEquilibriumPoint( m_energyGrads, point );
        CalcSnake( m_liveSnake.GetCurve().front(), point );
        m_layout.p2s( point, pixel );
        OnLButtonDownHandler( 0, pixel );
        pView->ClientToScreen( &pixel );
        ::SetCursorPos( pixel.x, pixel.y );
      }
      break;
*/

    case 76: // 'l', create line instead of snake
      if (m_bContouring && !(m_bLine) && ::GetCursorPos( &pixel ))
      {
        pView->ScreenToClient( &pixel );
        OnLButtonDownHandler( pView, 0, pixel );
        m_bLine = true;
      }
      break;
	case VK_DELETE:
		m_pDoc->DeleteObject();
		break;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Handler of KeyUp message. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::OnKeyUpHandler( CView* pView, UINT nChar, UINT nRepCnt, UINT nFlags )
{
  nChar;nRepCnt;nFlags;
 
  if (pView == 0)
    return;

  CPoint pixel;
  switch (nChar)
  {
    case 76: // 'l', release 'line' button and return to snake mode
      if (::GetCursorPos( &pixel ))
      {
        pView->ScreenToClient( &pixel );
        OnLButtonDownHandler( pView, 0, pixel );
      }
      m_bLine = false;
      break;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function finalizes contouring process, accepts the current curve and clears
           temporal variables including undo buffer. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::EndContouring(CView* pView)
{
  m_curve.insert( m_curve.end(), m_liveSnake.GetCurve().begin(), m_liveSnake.GetCurve().end() );
  CopyCurveToPixelCurve( PRODUCE_CLOSED_CURVE, true );

  MyChair::ChairSet::iterator itrCS = InsertChair();
  m_Chairs.push_back( itrCS );
  m_nCurrentChair = m_Chairs.end() - m_Chairs.begin() - 1;

  GetChair(m_nCurrentChair).curve = m_curve;

  //
	Vec2f MassCenter(0.0f, 0.0f);
  for(DWORD i=0; i < m_curve.size(); ++i)
  {
		MassCenter.x += m_curve[i].x;
		MassCenter.y += m_curve[i].y;
  }
  MassCenter.x /= m_curve.size();
  MassCenter.y /= m_curve.size();
	GetChair(m_nCurrentChair).center = Vec2i( int(MassCenter.x), int(MassCenter.y) ) ;
  //

  m_ChairCache.push_back( ChairCache() );
  ChairCache& cc = m_ChairCache.back();
  cc.icurve.resize( m_pixCurve.size() );
  AssignCurve( m_pixCurve.begin(), m_pixCurve.end(), cc.icurve.begin() );
  ASSERT( vislib::IsValidCurve( cc.icurve.begin(), cc.icurve.end() ) );

  m_bContouring = false;
  m_liveSnake.Clear();
  m_curPatch.clear();
  m_curve.clear();
  m_pixCurve.clear();
  m_auxCurve.clear();
  m_undoCurveLst.clear();

	RecalcLayout(pView, false);

  if (pView != 0)
    pView->Invalidate( FALSE );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function is called when the frame window is resized or image dimensions are changed. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::RecalcLayout( CView* pView, bool bUpdate )
{
	
  if (pView != 0)
  {
    m_pixCurve.clear();
    m_curPatch.clear();
    m_auxCurve.clear();
    m_liveSnake.Clear();
	
	DWORD dwW = m_demoImage.width();
	DWORD dwH = m_demoImage.height();
	m_layout.recalculate( dwW, dwH, pView->GetSafeHwnd() );

    // Recalculate demo-version of the current curve.
    if (!(m_curve.empty()) && m_layout.ready())
      CopyCurveToPixelCurve( PRODUCE_CLOSED_CURVE, false );

    // Recalculate demo-versions of outline contours of previously outlined objects.
	std::vector<ChairCache>::iterator crvcacheIt = m_ChairCache.begin();
    for ( std::vector< MyChair::ChairSet::iterator>::iterator crvIt = m_Chairs.begin(); 
			crvIt != m_Chairs.end(); ++crvIt, ++crvcacheIt)
    {
      crvcacheIt->icurve.resize( (*crvIt)->curve.size() );

      for (int i = 0, n = (int)((*crvIt)->curve.size()); i < n; i++)
      {
        POINT pt;
        m_layout.p2s( (*crvIt)->curve[i], pt );
        crvcacheIt->icurve[i].x = pt.x;
		crvcacheIt->icurve[i].y = pt.y;
      }

      vislib::ValidateCurve( (*crvcacheIt).icurve, true );
    }

    if (bUpdate)
      pView->Invalidate( TRUE );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function draws demo version of the current curve.

  \param  source      source points
  \param  pDC         underlaying device context
  \param  bSameColor  if true, then use the same color for all points
  \param  color       the color used when the flag 'bSameColor' is set on. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::DrawPixColArr( const PixColArr & source, CDC * pDC,
                                   bool bSameColor, COLORREF color )
{
	if (source.empty())
		return;
  const PixCol * points = &(*(source.begin()));

  if (bSameColor)
  {
    for (int i = 0, n = (int)(source.size()); i < n; i++)
    {
      pDC->SetPixel( (POINT&)(points[i]), color );
    }
  }
  else
  {
    for (int i = 0, n = (int)(source.size()); i < n; i++)
    {
      pDC->SetPixel( (POINT&)(points[i]), (points[i]).color );
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function stores colors of underlying screen points in demo version of the current curve.

  \param  src  source points.
  \param  pDC  underlaying device context.
  \param  dst  destination points. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::InitPixColArr( const Vec2fArr & src, CDC * pDC, PixColArr & dst)
{
	if (src.empty())
		return;
  dst.resize( src.size() );

  POINT         pt;
  const Vec2f * s = &(*(src.begin()));
  PixCol      * d = &(*(dst.begin()));

  for (int i = 0, n = (int)(src.size()); i < n; i++)
  {
    m_layout.p2s( s[i], pt );
    (POINT&)(d[i]) = pt;
    d[i].color = pDC->GetPixel( pt );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Implements the ID_EDIT_UNDO command. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::OnEditUndo(CView *pView) 
{
 
  if ((pView == 0) || !(m_layout.ready()))
    return;

  if (m_subMode == IDM_Edit_ObjectContouring) // object's contouring sub-mode
  {
    m_undoCurveLst.clear();

    if (!(m_liveSnake.GetCurve().empty())) // remove up to 10 pixels of the last curve's patch, if it is non-empty
    {
      m_liveSnake.EraseEndPoints(10);
      m_curPatch.resize( m_liveSnake.GetCurve().size() );
      m_tailPt = m_liveSnake.GetCurve().empty() ? m_headPt : m_liveSnake.GetCurve().back();
    }
    else if (!(m_curve.empty())) // remove up to 10 last pixels of the curve, if it is not empty
    {
      m_curve.erase( m_curve.end()-std::_cpp_min<uint>( m_curve.size(), 10 ), m_curve.end() );
      CopyCurveToPixelCurve( false, false );
      m_headPt = (m_tailPt = m_curve.empty() ? Vec2f() : m_curve.back());
    }

    if (m_liveSnake.GetCurve().empty() && m_curve.empty())
    {
      PartialClear();             // clear temporal variable in the case of empty curve
      RecalcLayout( pView, false );
    }
    else
    {
      CPoint cursor;                     // proceed from snake contouring from the last position
      m_layout.p2s( m_tailPt, cursor );
      m_bContouring = true;
      pView->ClientToScreen( &cursor );
      ::SetCursorPos( cursor.x, cursor.y );
    }
  }
  else if ((m_subMode == IDM_Edit_CurveAdjusting) || // object's adjusting or smoothing sub-mode
           (m_subMode == IDM_Edit_CurveSmoothing))
  {
    if (!(m_undoCurveLst.empty()) && HasSelectObject() )
    {
      m_curve.assign( m_undoCurveLst.back().begin(), m_undoCurveLst.back().end() );
      CopyCurveToPixelCurve( PRODUCE_CLOSED_CURVE, false );
      m_auxCurve.clear();
      m_undoCurveLst.pop_back();

      GetCurrentChair()->curve = m_curve;
      GetCurrentChairCache()->icurve.resize( m_pixCurve.size() );
   //   std::copy( m_pixCurve.begin(), m_pixCurve.end(), GetCurrentChairCache()->icurve.begin() );
	  GetCurrentChairCache()->icurve.resize( m_pixCurve.size() );
  AssignCurve( m_pixCurve.begin(), m_pixCurve.end(), GetCurrentChairCache()->icurve.begin() );
	 // GetCurrentChairCache()->icurve.assign ( (const Vec2i*) &(*m_pixCurve.begin()), (const Vec2i*)&(*m_pixCurve.end()) );
      ASSERT( vislib::IsValidCurve( GetCurrentChairCache()->icurve.begin(), GetCurrentChairCache()->icurve.end() ) );
    }
  }
  pView->Invalidate( FALSE );
}


//-------------------------------------------------------------------------------------------------
/** \brief Implements update of the ID_EDIT_UNDO command. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::OnUpdateEditUndo( CCmdUI * pCmdUI ) 
{
  bool bEnable = ((HasSelectObject()) && (m_undoCurveLst.empty() == false));
  switch (m_subMode)
  {
    case IDM_Edit_ObjectContouring: bEnable = (!(m_curve.empty()) ||
                                               !(m_liveSnake.GetCurve().empty()));
                                    break;
    case IDM_Edit_CurveAdjusting  :
    case IDM_Edit_CurveSmoothing  : break;
    default                       : bEnable = false;
  }
  pCmdUI->Enable( bEnable == true );
}

//FIXME: can we copy not all data members?

CGaugeHallDoc::ImageData::ImageData( CGaugeHallDoc*	 pDoc, MyChair::ChairSet& cs, int nCameraNo,IdGenerator& ig) : 
	m_ChairSet(cs),
	m_IdGenerator(ig),
//	m_hallImage(),
	m_nZoneNumber( nCameraNo ),
	m_pDoc(pDoc),
	m_nCurrentChair(INVALID_CHAIR),
	m_subMode(IDM_Edit_ObjectContouring)
{
  m_pixCurve.reserve( 1<<14 );
  m_curPatch.reserve( 1<<14 );
  m_curve.reserve   ( 1<<14 );
  m_auxCurve.reserve( 1<<14 );
  m_GaussFilter.create( CURVE_SMOOTHING_SIGMA );
  m_adjustRadius = 2;
  m_bEditMode = false;
  m_bContouring = false;
  m_bEditMode = false;
  PartialClear();
}

CGaugeHallDoc::ImageData::ImageData(const CGaugeHallDoc::ImageData& id):
	m_ChairSet(id.m_ChairSet),
	m_IdGenerator(id.m_IdGenerator),
	m_nCurrentChair(id.m_nCurrentChair),
	m_adjustRadius( id.m_adjustRadius ),
	m_GaussFilter(id.m_GaussFilter),
	m_hallImage(id.m_hallImage),
	m_pDoc(id.m_pDoc),
	m_bEditMode(id.m_bEditMode)
{
	// all other data members must be empty	
}

CGaugeHallDoc::ImageData& CGaugeHallDoc::ImageData::operator = 
	( const CGaugeHallDoc::ImageData& id)
{
	m_ChairSet		= id.m_ChairSet;
	m_IdGenerator	= id.m_IdGenerator;
	m_nZoneNumber	= id.m_nZoneNumber;	
	m_bEditMode		= id.m_bEditMode;
	// all other data members must be empty
	return *this;
}

bool CGaugeHallDoc::ImageData::PartialClear()
{
  m_headPt.set(0,0);
  m_tailPt.set(0,0);
  m_pixCurve.clear();
  m_curPatch.clear();
  m_curve.clear();
  m_auxCurve.clear();
  m_selPointNo = -1;
  m_liveSnake.Clear();
  m_bContouring = false;
  m_bLine = false;
  m_subMode = IDM_Edit_ObjectContouring;
  m_undoCurveLst.clear();
//  m_pSelCurve = 0;	
  return true;
}

bool CGaugeHallDoc::ImageData::DeleteContents()
{
  PartialClear();
  m_hallImage.clear();
  m_gradients.clear();
  m_demoImage.clear();
  m_energyGrads.clear();
  m_Chairs.clear();
  return true;
}

void CGaugeHallDoc::ImageData::CalcGradOfEnergy( const Vec2fImg & dirs, const Arr2f & grads )
{
  ASSERT( alib::AreDimensionsEqual( dirs, grads ) );

  Vec2f derv;
  int   W = grads.width();
  int   H = grads.height();
  
  m_energyGrads.resize( W, H );
  for (int y = 1; y < H-1; y++)
  {
    const Vec2f * d = dirs.row_begin( y );
    const float * g = grads.row_begin( y );
    Vec2f       * e = m_energyGrads.row_begin( y );

    for (int x = 1; x < W-1; x++)
    {
      derv.x = g[x+1] - g[x-1];
      derv.y = g[x+W] - g[x-W];
      e[x] = -(d[x] * (float)(d[x] & derv));
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function creates pixel-color representation of the current curve.

  \param  bClosed    true, if the current curve should be enclosed one.
  \param  bResample  true, if curve's resampling with one-pixel step is required. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::CopyCurveToPixelCurve( bool bClosed, bool bResample )
{
  if (bResample)
    vislib::ResampleCurve( m_curve, 1.0, bClosed );

  m_pixCurve.resize( m_curve.size() );
  for (int i = 0, n = (int)(m_curve.size()); i < n; i++)
   // m_layout.p2s( m_curve[i], m_pixCurve[i] );
    m_layout.p2s( m_curve[i], m_pixCurve[i] );

  vislib::ValidateCurve( m_pixCurve, bClosed );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function locally smoothes the current curve nearby selected point. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::LocallySmoothCurve()
{
  if ((m_curve.size() == 0) || !(alib::IsRange( m_selPointNo, 0, (int)(m_curve.size()) )))
    return;

  #define INDEX(i) (bClosed ? (((i)+N)%N) : (ALIB_LIMIT(i,0,N-1)))

  const FloatArr & filter = m_GaussFilter.filter();
  int              fSize = (int)(filter.size());
  const int        N = (int)(m_curve.size());
  bool             bClosed = PRODUCE_CLOSED_CURVE;

  m_auxCurve = m_curve;
  for (int d = 0, range = (int)ceil( 5.0*CURVE_SMOOTHING_SIGMA ); d < range; d++)
  {
	  for (int s = ((d > 0) ? 2 : 1); s > 0; s--)
    {
      int   pointNo = (s == 1) ? INDEX(m_selPointNo-d) : INDEX(m_selPointNo+d);
      float share = (float)exp( -d/CURVE_SMOOTHING_SIGMA );
      Vec2f sum( m_auxCurve[pointNo] * filter[0] );

      for (int i = 1; i < fSize; i++)
      {
        sum += (m_auxCurve[ INDEX(pointNo-i) ] + m_auxCurve[ INDEX(pointNo+i) ]) * filter[i];
      }
      m_curve[pointNo] = sum*share + m_auxCurve[pointNo]*(1.0f-share);
    }
  }
  
  #undef  INDEX
}


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates snake or inserts straight line given two end points.

  \param  p1  the 1st end point.
  \param  p2  the 2nd end point. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::ImageData::CalcSnake( Vec2f p1, Vec2f p2 )
{
  m_liveSnake.m_alpha = 0.35f;
  m_liveSnake.m_beta  = 0.5f * m_liveSnake.m_alpha;

  if (m_bLine || !(m_liveSnake.Run( m_energyGrads, p1, p2, true )))
  {
    m_liveSnake.MakeStraightLine( p1, p2 );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function enforces point density of the curve to be approximately
           one sample point per one pixel.

  \param  curve  the curve to be processed.
  \return        the new index of the currently selected point, if any, on resampled curve or -1. */
//-------------------------------------------------------------------------------------------------
int CGaugeHallDoc::ImageData::KeepPointDensity( Vec2fArr & curve )
{
  if (curve.size() <= 1)
    return -1;

  // Get the currently selected point, if any.
  bool  bSelPt = alib::IsRange( m_selPointNo, 0, (int)(curve.size()) );
  Vec2f selPt = bSelPt ? curve[m_selPointNo] : Vec2f();

  // Perform rough resampling.
  for (Vec2fArr::iterator cur = curve.begin(); cur != curve.end();)
  {
    Vec2fArr::iterator F = curve.begin();
    Vec2fArr::iterator L = curve.end()-1;

    if (!PRODUCE_CLOSED_CURVE && (cur == F))
    {
      cur++;
      continue;
    }

    Vec2fArr::iterator pre = (cur != F) ? (cur-1) : L;
    double             d = (*pre).distance( *cur );

    if (d < 0.5)
    {
      cur = curve.erase( cur );
      continue;
    }
    else if (d > 1.5)
    {
      int   nStep = (int)ceil( d );
      Vec2f delta = ((*cur)-(*pre))/(float)nStep;
      Vec2f pt = (*pre);

      for (int i = 0; i < (nStep-1); i++)
      {
        pt += delta;
        cur = curve.insert( cur, pt );
        cur++;
      }
    }
    cur++;
  }

  // Renew index of the currently selected point.
  if (bSelPt)
  {
    Vec2fArr::iterator it = vislib::FindNearestPoint( curve.begin(), curve.end(), selPt.x, selPt.y );
    return (int)(std::distance( curve.begin(), it ));
  }
  return -1;
}

void CGaugeHallDoc::ImageData::LoadFloatImage()
{
	USES_CONVERSION;
 try
 {

    CWaitCursor              wait;
    vislib::GrayEdgeDetector ed;

//	m_phallImage = FloatImage;
	
    alib::RepeatedlySmoothImage( m_hallImage, 1 );
    if (!(ed.Run( m_hallImage )))
      ALIB_THROW( _T("!GrayEdgeDetector()") );
    vislib::EquateGradientPeaks( ed.GetGradDirections(), ed.GetGradients(), 0, 0,
                                 2.0f*ed.GetGradNoise(), 0.25f, false, true, m_gradients );
    CalcGradOfEnergy( ed.GetGradDirections(), m_gradients );
    MyFloatImageToByteImage( m_hallImage, m_demoImage, 1, true );
  }
  catch (std::exception & e)
  {
    DeleteContents();
    alib::ErrorMessage( CString(e.what()) );
  }
  catch (...)
  {
    DeleteContents();
    alib::ErrorMessage( ALIB_UNSUPPORTED_EXCEPTION );
  }
 // 
}

void CGaugeHallDoc::ImageData::DrawObject(CDC& dc)
{
	std::vector<ImageData::ChairCache>::iterator crvIt;
	int nOff = 0;
      for (crvIt = m_ChairCache.begin(); 
			crvIt != m_ChairCache.end(); ++crvIt, ++nOff)
      {
        const Vec2iArr & curve = (*crvIt).icurve;
        int              k, nPoint = (int)(curve.size()), a = (*crvIt).animation;
        COLORREF         color = (m_nCurrentChair == nOff) ? SELECTED_ANIMATED_CURVE_COLOR
                                                                    : ANIMATED_CURVE_COLOR;

		for (k = 0; (k+a) < nPoint; k += 8) dc.SetPixel( CPoint(curve[k+a].x, curve[k+a].y), RGB(255,255,255) );
        a = (a+4)%8;
        for (k = 0; (k+a) < nPoint; k += 8) dc.SetPixel( CPoint(curve[k+a].x, curve[k+a].y), color );
        (*crvIt).animation = ((*crvIt).animation+1)%8;
      }
}

void CGaugeHallDoc::ImageData::DrawView(CView* pView, CDC& dc)
{
		  if (!(m_layout.ready()))
		  {
			CRect clientRect;
			pView->GetClientRect( &clientRect );
			if (!(clientRect.IsRectEmpty()))
			  dc.FillSolidRect( &clientRect, RGB(230,240,250) );
			return;
		  }
		  MyDrawGrayImage( m_demoImage, dc.GetSafeHdc(),
						   m_layout.rectangle(), INVERT_AXIS_Y, true );

		  if (m_bContouring )
		  {
			for (int i = 0, n = (int)(m_pixCurve.size()); i < n; i++)
			{
			  dc.SetPixel( (POINT&)(m_pixCurve[i]), CURVE_COLOR );
			}
		  }
}

void	CGaugeHallDoc::ImageData::DeleteCurrentChair()
{
	if( HasSelectObject() )
	{
		std::vector<MyChair::ChairSet::iterator>::iterator itr = m_Chairs.begin() + m_nCurrentChair;
		m_pDoc->DeleteChair( *itr );
	}
}

void CGaugeHallDoc::ImageData::InitMode( CView* pView )
{
	ASSERT_KINDOF( CScrollView, pView );
	((CScrollView*)pView)->SetScrollSizes( MM_TEXT, CSize(1, 1) );
}

bool CGaugeHallDoc::ImageData::OnDeleteChair( MyChair::ChairSet::iterator			itr )
{
	std::vector<MyChair::ChairSet::iterator>::iterator itrCache = 
		std::find( m_Chairs.begin(), m_Chairs.end(), itr );
	if( itrCache != m_Chairs.end() )
	{
		int nIndex = itrCache - m_Chairs.begin();
		m_Chairs.erase( itrCache );
		m_ChairCache.erase( m_ChairCache.begin() + nIndex );
		//FIXME: just correct chair index
		m_nCurrentChair = INVALID_CHAIR;
	}
	return true;
}

bool CGaugeHallDoc::ImageData::OnDeleteLink( ChairLinkList_t::iterator	itr )
{
	// ignore
	UNREFERENCED_PARAMETER(itr);
	return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears only temporal variables. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::PartialClear()
{
	std::map<int, boost::shared_ptr<ImageData> > ::iterator it = m_ImageData.begin();
	for(; it != m_ImageData.end(); ++it)
	{
		it->second->PartialClear();
	}
}


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework to delete the document's data
           without destroying the CDocument object itself. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::DeleteContents()
{
//	std::for_each( m_ImageData.begin(), m_ImageData.end(), std::mem_fun_ref( ImageData::DeleteContents ) );
	std::map<int, boost::shared_ptr<ImageData> > ::iterator it = m_ImageData.begin();
	for(; it != m_ImageData.end(); ++it)
	{
		it->second->DeleteContents();
	}
  CDocument::DeleteContents();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates gradient vectors of potential energy.

  \param  dirs   directions at image points.
  \param  grads  gradient modules (or squared gradient modules) at image points. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::CalcGradOfEnergy( const Vec2fImg & dirs, const Arr2f & grads )
{
	GetImageData().CalcGradOfEnergy( dirs, grads );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function creates pixel-color representation of the current curve.

  \param  bClosed    true, if the current curve should be enclosed one.
  \param  bResample  true, if curve's resampling with one-pixel step is required. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::CopyCurveToPixelCurve( bool bClosed, bool bResample )
{
  GetImageData().CopyCurveToPixelCurve( bClosed, bResample );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function locally smoothes the current curve nearby selected point. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::LocallySmoothCurve()
{
  GetImageData().LocallySmoothCurve();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function calculates snake or inserts straight line given two end points.

  \param  p1  the 1st end point.
  \param  p2  the 2nd end point. */
//-------------------------------------------------------------------------------------------------
void CGaugeHallDoc::CalcSnake( Vec2f p1, Vec2f p2 )
{
  GetImageData().CalcSnake( p1, p2 );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function enforces point density of the curve to be approximately
           one sample point per one pixel.

  \param  curve  the curve to be processed.
  \return        the new index of the currently selected point, if any, on resampled curve or -1. */
//-------------------------------------------------------------------------------------------------
int CGaugeHallDoc::KeepPointDensity( Vec2fArr & curve )
{
  return GetImageData().KeepPointDensity(curve);
}



void CGaugeHallDoc::DrawPixColArr( const PixColArr & src, CDC * pDC, bool bSameColor , COLORREF color )
{
  GetImageData().DrawPixColArr( src, pDC, bSameColor, color );
}

void CGaugeHallDoc::InitPixColArr( const Vec2fArr & src, CDC * pDC, PixColArr & dst )
{
  GetImageData().InitPixColArr( src, pDC, dst );
}

void CGaugeHallDoc::EndContouring()
{
  CGaugeHallView * pView = GetView();
  if (pView == 0)
    return;

  GetImageData().EndContouring( pView );
}

void CGaugeHallDoc::OnLButtonDownHandler( UINT nFlags, CPoint point )
{
  CGaugeHallView * pView = GetView();
  if (pView == 0)
    return;

  if(IsViewAll())
  {
	  pView->SetCapture();
	  CRect rc;
	  pView->GetClientRect( &rc );
	  pView->ClientToScreen( &rc );
	  ::ClipCursor(&rc);

	  int nRoomID = GetCurrentRoom();
	  AllImageData&		All		= *m_AllImageData[ nRoomID ];
		All.OnLButtonDownHandler(pView, nFlags, point);
  }
  else
  {
	  
	GetImageData().OnLButtonDownHandler( pView, nFlags, point );
  }
}

void CGaugeHallDoc::OnLButtonUpHandler( UINT nFlags, CPoint point )
{
  CGaugeHallView * pView = GetView();
  if (pView == 0)
    return;
  if(IsViewAll())
  {
	  int nRoomID = GetCurrentRoom();
	  AllImageData&		All		= *m_AllImageData[ nRoomID ];
	All.OnLButtonUpHandler(pView, nFlags, point);
	::ReleaseCapture();
	  ::ClipCursor(NULL);
  }
  else
  {
  GetImageData().OnLButtonUpHandler( pView, nFlags, point );
  }
}

void CGaugeHallDoc::OnMouseMoveHandler( UINT nFlags, CPoint point )
{
  CGaugeHallView * pView = GetView();
  if (pView == 0)
    return;
  if(IsViewAll())
  {
	  int nRoomID = GetCurrentRoom();
	  AllImageData&		All		= *m_AllImageData[ nRoomID ];
		All.OnMouseMoveHandler(pView, nFlags, point);
  }
  else
  {
  GetImageData().OnMouseMoveHandler( pView, nFlags, point );
  }
}

void CGaugeHallDoc::OnCancelMode()
{
  CGaugeHallView * pView = GetView();
  CGaugeHallFrame* pFrame = GetHallFrame();
  if (pView == 0 || !pFrame || !pFrame->IsReady() )
    return;
  if(IsViewAll())
  {
//		m_AllImageData.OnMouseMoveHandler(pView, nFlags, point);
  }
  else
  {
  GetImageData().OnCancelMode( pView );
  }
}

void CGaugeHallDoc::OnKeyDownHandler( UINT nChar, UINT nRepCnt, UINT nFlags )
{
  CGaugeHallView * pView = GetView();
  if (pView == 0)
    return;
  if(IsViewAll())
  {
	  int nRoomID = GetCurrentRoom();
	  AllImageData&		All		= *m_AllImageData[ nRoomID ];
	All.OnKeyDownHandler(pView, nChar, nRepCnt, nFlags);
  }
  else
  {
  GetImageData().OnKeyDownHandler(pView, nChar, nRepCnt, nFlags);
  }
}

void CGaugeHallDoc::OnKeyUpHandler( UINT nChar, UINT nRepCnt, UINT nFlags )
{
  CGaugeHallView * pView = GetView();
  if (pView == 0)
    return;
  if(IsViewAll())
  {

  }
  else
  {
  GetImageData().OnKeyUpHandler(pView, nChar, nRepCnt, nFlags);
  }
}

void CGaugeHallDoc::OnEditUndoHandler()
{
  CGaugeHallView * pView = GetView();
  if (pView == 0)
    return;
/*
   if(m_bViewAll)
  {

  }
  else
  {
	  GetImageData().OnEditUndoHandler(pView);
  }*/
}

void CGaugeHallDoc::RecalcLayout( bool bUpdate )
{
	CGaugeHallView * pView = GetView();
	CGaugeHallFrame* pFrame = GetHallFrame();
	if (pView == 0 || !pFrame || !pFrame->IsReady() )
		return;

  if(IsViewAll())
  {
	  int nRoomID = GetCurrentRoom();
	  AllImageData&		All		= *m_AllImageData[ nRoomID ];
	All.RecalcLayout( pView, bUpdate );
  }
  else
  {
	GetImageData().RecalcLayout( pView, bUpdate);
  }
}

void CGaugeHallDoc::OnEditUndo()
{
  CGaugeHallView * pView = GetView();
  if (pView == 0)
    return;
	GetImageData().OnEditUndo( pView );
}

void CGaugeHallDoc::OnUpdateEditUndo( CCmdUI * pCmdUI )
{
  CGaugeHallView * pView = GetView();
  if (pView == 0)
    return;

  GetImageData().OnUpdateEditUndo( pCmdUI );
}

void CGaugeHallDoc::LoadFloatImage(DWORD dwImageNumber)
{
	ASSERT(FALSE);
}

bool CGaugeHallDoc::InitBackground(const SparceRectImageLayout::IndexArray_t& arr)
{
	UNREFERENCED_PARAMETER(arr);
	return true;
}
#if 0
void CGaugeHallDoc::OnImage( UINT nID )
{
	int nIndex = nID - ID_IMAGE_1;

	m_bViewAll = false;
	m_nCurrentIndex = nIndex;
	GetImageData().InitMode( GetView() );
	UpdateAllViews(0);
}

void CGaugeHallDoc::OnImageUpdate( CCmdUI* pCmdUI)
{
	int nIndex = pCmdUI->m_nID - ID_IMAGE_1;
	
	pCmdUI->SetCheck(( nIndex == m_nCurrentIndex) ? 1 : 0);
	pCmdUI->Enable( m_ImageData.size() > DWORD(nIndex) );
}
#endif
void	CGaugeHallDoc::DeleteObject()
{
  CGaugeHallView * pView = GetView();
  if (pView == 0)
    return;
  
    if ( HasSelectObject() &&
         (::AfxMessageBox( IDS_TITLE_DELETE, MB_YESNO | MB_ICONQUESTION ) == IDYES))
	{
		GetImageData().DeleteCurrentChair();
		pView->Invalidate(FALSE);
	}
}

void CGaugeHallDoc::DrawObject(CDC& dc)
{
	CView* pView = GetView();
	AllImageData& All = *m_AllImageData[ GetCurrentRoom() ];
	if(IsViewAll())
	{
		 All.DrawObject( pView, dc);
	}
	else
	{
		GetImageData().DrawObject(dc);
	}
}

//FIXME: to resources

static LPCTSTR szWC_XML = _T("*.xml");
//static LPCTSTR szWC_GhgFile	= _T("Gauge hall graph(*.xml)|*.xml|All files(*.*)|*.*||");

void CGaugeHallDoc::OnFileSaveGraph()
{
	CString sChairFilter;
	sChairFilter.LoadString( IDS_FILTER_CHAIR );
	CFileDialogEx dlg(	FALSE, szWC_XML, 0, 
						OFN_READONLY|OFN_OVERWRITEPROMPT, 
						sChairFilter, 0 );
	if( dlg.DoModal() == IDOK)
	{
		try
		{
			int nRoomID = GetCurrentRoom();
			const MyChair::ChairSet&		ChSet	= m_ChairSet[ nRoomID ];
			const ChairLinkList_t&			ChLink	= m_ChairLink[ nRoomID ];
			IChairSerializer::PositionMap_t PosMap	= GetCameraPosition( nRoomID );
			ChairSaver cs;
			cs.Save( dlg.GetPathName(), ChSet, ChLink, PosMap );
		}
		catch(ChairSerializerException)
		{
			AfxMessageBox( IDS_ERROR_FILE_SAVING, MB_OK|MB_ICONERROR);
		}
	}
}

void CGaugeHallDoc::OnFileLoadGraph()
{
	CString sChairFilter;
	sChairFilter.LoadString( IDS_FILTER_CHAIR );
	CFileDialogEx 	dlg(	TRUE, szWC_XML, 0, 0, sChairFilter, 0 );
	if( dlg.DoModal() == IDOK)
	{
		try
		{
			int nRoomID = GetCurrentRoom();
#if 0
			MyChair::ChairSet&	ChSet	= m_ChairSet[ nRoomID ];
			ChairLinkList_t&	ChLink	= m_ChairLink[ nRoomID ];
#endif
			MyChair::ChairSet	ChSet;
			ChairLinkList_t	ChLink;

			ChairLoader cl;
			IChairSerializer::PositionMap_t PosMap;
			cl.Load( dlg.GetPathName(), ChSet, ChLink, PosMap );
			PostLoadProcessGraph( nRoomID, ChSet, ChLink );
			PostLoadGraph();
		}
		catch(ChairSerializerException)
		{
			AfxMessageBox(IDS_ERROR_FILE_LOADING, MB_OK|MB_ICONERROR);
		}
	}	
}

void CGaugeHallDoc::PostLoadGraph()
{
#if  0
	std::for_each( m_ImageData.begin(), m_ImageData.end(), std::mem_fun_ref( ImageData::PreLoad ) );

	int nRoomID = GetCurrentRoom();
	MyChair::ChairSet&		ChSet	= m_ChairSet[ nRoomID ];

	MyChair::ChairSet::iterator itr = ChSet.begin();
	DWORD dwMaxId = 0;
	RenumberZoneIndex();
	for(; itr != ChSet.end(); ++itr)
	{
		DWORD dwZoneNumber = IdToIndex(itr->subFrameNo);
		dwMaxId = std::_cpp_max<DWORD>( dwMaxId, itr->id );
		if(dwZoneNumber < m_ImageData.size())
		{
			m_ImageData[dwZoneNumber].InsertCache( itr );
		}
	}
	m_IdGenerator.SetCounter( dwMaxId );
	RecalcLayout(true);
#endif

	int nRoomID = GetCurrentRoom();
	MyChair::ChairSet&		ChSet	= m_ChairSet[ nRoomID ];
	std::vector< std::pair<int, ImageData*> > ImgData = GetImageDataInRoomPair( nRoomID );
	std::map<int, size_t> OurCameras;
	for( size_t i = 0; i < ImgData.size(); ++i )
	{
		ImgData[i].second->PreLoad();
		OurCameras.insert( std::make_pair( ImgData[i].first, i ) );
	}

	LONG dwMaxId = 0;//m_IdGenerator.GetCounter();
	MyChair::ChairSet::iterator itr = ChSet.begin();
	for(; itr != ChSet.end(); ++itr)
	{
		int nCameraNo = itr->subFrameNo;
		dwMaxId = std::_cpp_max<DWORD>( dwMaxId, itr->id );
		std::map<int, size_t>::iterator it = OurCameras.find( nCameraNo );
		if( it != OurCameras.end() )
		{
			size_t nIndex = it->second;
			ImgData[nIndex].second->InsertCache( itr );
		}
	}
	m_IdGenerator.SetCounter( dwMaxId + 1 );
	RecalcLayout(true);

}

void CGaugeHallDoc::OnFileLoadImage()
{
	if( IsViewAll() ) return;

	int nCamID = GetCurrentCamera();
	int nRoomID = GetCurrentRoom();
	std::pair<int, int> pos = IDToPosition( nCamID );
	CString sTitle, sFileName;
	sFileName = PostitionToName( pos.first, pos.second, GetCamerasInRoom( nRoomID ) );
	if( !sFileName.IsEmpty() ) sFileName += _T(".bmp");

	CFileDialogEx dlg( TRUE, WILDCARD_BMP, sFileName/*.IsEmpty() ? NULL : sFileName*/, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, BMP_FILTER, 0 );
	// FIXME : resources
	sTitle.Format( _T("позиция %d,%d"), pos.first+1, pos.second+1 );
	dlg.m_ofn.lpstrTitle = sTitle;
	if( dlg.DoModal() == IDOK )
	{
		CString strName = dlg.GetPathName();
		if( ! CheckImage( strName )  )
		{
			AfxMessageBox( IDS_ERROR_IMAGESIZE, MB_OK|MB_ICONERROR );
			return;
		}

		LongOperationAnim op( AfxGetMainWnd()->GetSafeHwnd() );
		
		ImageMap_t::iterator it = m_ImageData.find( nCamID );
		AllImageMap_t::iterator itA = m_AllImageData.find( nRoomID );
		// FIXME : size of image may vary
		if( it != m_ImageData.end() )
		{
			Arr2f& z = it->second->m_hallImage;
			bool res = LoadBackgroundImage( dlg.GetPathName(), &z, 1  );
			if( itA != m_AllImageData.end() )
			{
				itA->second->SetImageDim( z.width(), z.height() );
				itA->second->RecalcLayout( GetView(), true );
			}
			PostLoadImage( *it->second );
		}
		InitMode();
	}
}

void	CGaugeHallDoc::LoadImage( AllImageData& All,int x, int y )
{
	if( IsViewAll() ) return;
	CFileDialogEx dlg( TRUE, WILDCARD_BMP, 0, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, BMP_FILTER, 0 );

	if( dlg.DoModal() == IDOK )
	{
		CString strName = dlg.GetPathName();
		if( ! CheckImage( strName )  )
		{
			AfxMessageBox( IDS_ERROR_IMAGESIZE, MB_OK|MB_ICONERROR );
			return;
		}

		LongOperationAnim op( AfxGetMainWnd()->GetSafeHwnd() );
		
		int	nCameraID = PositionToID(  x,  y, All.GetRoomID() );
		ImageMap_t::iterator it = m_ImageData.find( nCameraID );
		// FIXME : size of image may vary
		if( it != m_ImageData.end() )
		{
			Arr2f& z = it->second->m_hallImage;
			bool res = LoadBackgroundImage( dlg.GetPathName(), &z, 1  );
			All.SetImageDim( z.width(), z.height() );
			All.RecalcLayout( GetView(), true );
			PostLoadImage( *it->second );
		}
		InitMode();
	}
}

void CGaugeHallDoc::DrawView(CDC& dc)
{
	CView* pView = GetView();
	AllImageMap_t::iterator it = m_AllImageData.find( GetCurrentRoom() );
	if( it == m_AllImageData.end() ) return;
	AllImageData& All = *it->second;
	if(IsViewAll())
	{
		 All.DrawView(pView, dc);
	}
	else
	{
		GetImageData().DrawView(pView, dc);
	}
}

void CGaugeHallDoc::DeleteChair( MyChair::ChairSet::iterator			itr )
{
	int nRoomID = GetCurrentRoom();
	AllImageData&		All		= *m_AllImageData[ nRoomID ];
	MyChair::ChairSet&	ChSet	= m_ChairSet[ nRoomID ];
	ChairLinkList_t&	ChLink	= m_ChairLink[ nRoomID ];
	std::vector<int> CameraNo;
	GetCameras( nRoomID, CameraNo );
	ChSet.erase( itr );
	All.OnDeleteChair(itr);
	DeleteLinkWithItem( ChLink, itr );
	for( size_t i=0; i < CameraNo.size(); ++i )
	{
		int nCameraID = CameraNo[i];
		m_ImageData[nCameraID]->OnDeleteChair(itr);
	}
}

void CGaugeHallDoc::DeleteLink( ChairLinkList_t::iterator	itr )
{
	int nRoomID = GetCurrentRoom();
	AllImageData&		All		= *m_AllImageData[ nRoomID ];
	MyChair::ChairSet&	ChSet	= m_ChairSet[ nRoomID ];
	ChairLinkList_t&	ChLink	= m_ChairLink[ nRoomID ];
	std::vector<int> CameraNo;
	GetCameras( nRoomID, CameraNo );

	ChLink.erase( itr );
	All.OnDeleteLink(itr);
	for( size_t i=0; i < CameraNo.size(); ++i )
	{
		int nCameraID = CameraNo[i];
		m_ImageData[nCameraID]->OnDeleteLink(itr);
	}
}

void CGaugeHallDoc::OnRButtonDownHandler( CView* pView, UINT nFlags, CPoint point, CMenu* pMenu )
{
	if( !IsViewAll() )
	{
		POINT p;
		::GetCursorPos( &p );
		pMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON, (int)(p.x), (int)(p.y), ::AfxGetMainWnd(), 0 );
	}
	else
	{
		int nRoomID = GetCurrentRoom();
		AllImageMap_t::iterator it = m_AllImageData.find( nRoomID );
		if( it == m_AllImageData.end() ) return;
		it->second->OnRButtonDownHandler( pView, nFlags, point );
	}
}

////////////////////////////////////////////////////////////

CGaugeHallDoc::AllImageData::AllImageData
	(	CGaugeHallDoc* pDoc,
		std::map<int, boost::shared_ptr<ImageData> >& id, 
		int	nRoomID,
		MyChair::ChairSet& cs,
		ChairLinkList_t& cl
		):
	m_ImageData(id),
	m_ChairSet(cs),
	m_ChairLink(cl),
	m_nAnimation(0),
	m_CurrentChair(cs.end()),
	m_CurrentLink(cl.end()),
	m_pDoc(pDoc),
	m_nRoomID(nRoomID)
{

}

void CGaugeHallDoc::AllImageData::DrawObject(CView* pView, CDC& dc)
{
	CPen penWhite( PS_SOLID, 2, RGB(255,255,255) );
	CPen penOver( PS_DASH, 2, ANIMATED_CURVE_COLOR);
	CPen penSelect( PS_DASH, 2, SELECTED_ANIMATED_CURVE_COLOR);
	CPen penSelectChair( PS_DASH, 2, RGB(0, 255, 0) );
	CGdiObject* pOldPen = dc.SelectObject( &penWhite );

	CPen penOverCh( PS_DASH, 1, ANIMATED_CURVE_COLOR);
	CPen penSelectCh( PS_DASH, 1, SELECTED_ANIMATED_CURVE_COLOR);
	CPen penWhiteCh( PS_SOLID, 1, RGB(255,255,255) );

	CSize sizeOffset( pView->GetScrollPos( SB_HORZ ), pView->GetScrollPos( SB_VERT ) );

	//int nRoomID = GetCurrentRoom();
	std::vector<ImageData*> ImArr = m_pDoc->GetImageDataInRoom( m_nRoomID );
	for(size_t i=0; i < ImArr.size(); ++i)
	{
		std::vector< MyChair::ChairSet::iterator>::iterator crvIt;
		int nOff = 0;
		  for (crvIt = ImArr[i]->m_Chairs.begin(); 
				crvIt != ImArr[i]->m_Chairs.end(); ++crvIt, ++nOff)
		  {
			const Vec2fArr & curve = (*crvIt)->curve;
			int             k, 
							nPoint = (int)(curve.size()); 
			for( k=0; k < nPoint; ++k /*, ++iter*/ )
			{
				int newk  = (m_nAnimation + k)%nPoint;
				int nextk = (newk + 1)%nPoint;

				const Vec2f& v1 = curve[newk];
				const Vec2f& v2 = curve[nextk];
				CPoint pt1, pt2;
				int nIndex = i;
				if( m_pDoc->IsStandAlone() )
				{
					nIndex =  std::find(m_pDoc->m_StandAloneCameras.begin(),
										m_pDoc->m_StandAloneCameras.end(),
										ImArr[i]->m_nZoneNumber ) - m_pDoc->m_StandAloneCameras.begin();
				}
				m_layout.p2s( nIndex, v1, pt1 );
				m_layout.p2s( nIndex, v2, pt2 );

				pt1.Offset( -sizeOffset );
				pt2.Offset( -sizeOffset );

				// NOTE : if( newk ... ) - static curve 
				if( ( k % 8)  < 4 )
					dc.SelectObject( *crvIt == m_CurrentChair ? &penSelectCh : &penOverCh  );
				else
					dc.SelectObject( &penWhiteCh  );
				dc.MoveTo( pt1 );
				dc.LineTo( pt2 );
			}		
		  }
	}
	m_nAnimation = (m_nAnimation +1 )%8;
	// code for links, not required for now
#if USE_OLD_HALL
	ChairLinkList_t::const_iterator itrLink = m_ChairLink.begin();
	for(; itrLink != m_ChairLink.end(); ++itrLink)
	{
		Vec2i CenterFirst	= itrLink->m_itrFirst->center;
		Vec2i CenterSecond	= itrLink->m_itrSecond->center;
		
		int zoneFirst	= m_pDoc->IdToIndex(itrLink->m_itrFirst->subFrameNo);
		int zoneSecond	= m_pDoc->IdToIndex(itrLink->m_itrSecond->subFrameNo);
		CPoint ptStartOld( CenterFirst.x, CenterFirst.y ) ;
		CPoint ptEndOld( CenterSecond.x, CenterSecond.y );
		CPoint ptStart, ptEnd;

		m_layout.p2s( zoneFirst, ptStartOld, ptStart);
		m_layout.p2s( zoneSecond, ptEndOld, ptEnd);

		ptStart.Offset( -sizeOffset );
		ptEnd.Offset( -sizeOffset );
		
		dc.SelectObject( &penWhite );
		dc.MoveTo( ptStart );
		dc.LineTo( ptEnd );
		dc.SelectObject( itrLink == m_CurrentLink ? &penSelect : &penOver  );
		if( itrLink->m_itrFirst == m_CurrentChair || itrLink->m_itrSecond == m_CurrentChair )
			dc.SelectObject( &penSelectChair );
		dc.MoveTo( ptStart );
		dc.LineTo( ptEnd );
	}
#endif
	dc.SelectObject( pOldPen );
/*
	////
	std::set< std::pair<const MyChair::Chair*, const MyChair::Chair*> > DrewChairs;
	MyChair::ChairSet::const_iterator itrChair = m_ChairSet.begin();
	for(;  itrChair != m_ChairSet.end() ; ++itrChair )
	{
		const MyChair::Chair* ThisChair = &(*itrChair);
		if( DrewChairs.find( ThisChair ) == DrewChairs.end() )
		{
			// not yet drew
			DrewChairs.insert( &(*itrChair));
			DrawChairLink( dc, sizeOffset, ThisChair, ThisChair->pLeftChair );
			DrawChairLink( dc, sizeOffset, ThisChair, ThisChair->pRightChair );
			DrawChairLink( dc, sizeOffset, ThisChair, ThisChair->pBackChair[0] );
			DrawChairLink( dc, sizeOffset, ThisChair, ThisChair->pBackChair[1] );
			DrawChairLink( dc, sizeOffset, ThisChair, ThisChair->pBackChair[2] );
			DrawChairLink( dc, sizeOffset, ThisChair, ThisChair->pFrontChair[0] );
			DrawChairLink( dc, sizeOffset, ThisChair, ThisChair->pFrontChair[1] );
			DrawChairLink( dc, sizeOffset, ThisChair, ThisChair->pFrontChair[2] );
		}
	}
	////*/

}

void CGaugeHallDoc::AllImageData::DrawView(CView* pView, CDC& dc)
{
	if (!(m_layout.ready()))
	{
	CRect clientRect;
	pView->GetClientRect( &clientRect );
	if (!(clientRect.IsRectEmpty()))
		dc.FillSolidRect( &clientRect, RGB(230,240,250) );
	return;
	}

	bool res = GetCfgDB().UseDB() && GetDB().IsConnected();
	if( m_pDoc->IsStandAlone() ) 
	{
		const SparceRectImageLayout::IndexArray_t& Arr = m_layout.GetCells();
		std::vector<ImageData*> ImgArr = m_pDoc->GetImageDataInRoom( m_nRoomID );
		ASSERT( ImgArr.size() == Arr.size() );
		ASSERT( Arr.size() == m_pDoc->m_StandAloneCameras.size() );
		for( size_t i = 0; i != ImgArr.size();  ++i )
		{
			ImageMap_t::iterator it = m_ImageData.find( m_pDoc->m_StandAloneCameras[i] );
			ASSERT( it != m_ImageData.end( ));
			CRect rc = m_layout.rectangle( i );
			MyDrawGrayImage( it->second->m_demoImage, dc.GetSafeHdc(), rc, INVERT_AXIS_Y );
		}
	}
	else
	{

	std::set<int>   Cams = GetDB().GetTableCameraFacet().SelectCamerasFromRoom( m_nRoomID );
	std::set<int>::const_iterator itC = Cams.begin();
	//const std::set<CameraRow>& Cams GetDB().GetTableCameraFacet()
	SparceRectImageLayout::IndexArray_t Arr;
	for( DWORD i = 0; itC != Cams.end(); ++itC, ++i )
	{
		ImageMap_t::iterator it = m_ImageData.find( *itC );
		ASSERT( it != m_ImageData.end( ));
		CRect rc = m_layout.rectangle( i );
		MyDrawGrayImage( it->second->m_demoImage, dc.GetSafeHdc(), rc, INVERT_AXIS_Y );
	}
	}
}

void CGaugeHallDoc::AllImageData::RecalcLayout( CView* pView, bool bUpdate )
{
	std::pair<DWORD,DWORD> MaxRect = m_layout.GetMaxNonEmptyRect();
	const int dwW = m_ImageDimension.first	* MaxRect.first;
	const int dwH = m_ImageDimension.second * MaxRect.second;
	m_layout.recalculate( dwW, dwH, pView );

    if (bUpdate)
      pView->Invalidate( TRUE );
}

void CGaugeHallDoc::AllImageData::OnLButtonDownHandler(CView* pView,  UINT nFlags, CPoint point )
{
	UNREFERENCED_PARAMETER(nFlags);
#if 0
	CPoint ptPos;
	CSize sizeOffset ( pView->GetScrollPos( SB_HORZ ), pView->GetScrollPos( SB_VERT ) );
	CPoint ptTruePoint(point);
	ptTruePoint.Offset( sizeOffset );

	m_CurrentChair	= m_ChairSet.end();
	m_CurrentLink	= m_ChairLink.end();

	for( ChairLinkList_t::iterator itLink = m_ChairLink.begin(); itLink != m_ChairLink.end(); ++itLink )
	{
		MyChair::ChairSet::iterator itF = itLink->m_itrFirst;
		MyChair::ChairSet::iterator itS = itLink->m_itrSecond;
		int ZoneF = m_pDoc->IdToIndex(itF->subFrameNo);
		int ZoneS = m_pDoc->IdToIndex(itS->subFrameNo);

		Vec2i CenterF = itF->center;
		Vec2i CenterS = itS->center;

		CPoint ptScreenF;
		CPoint ptScreenS;
		CPoint ptPos(point);
		m_layout.p2s( ZoneF, CPoint(CenterF.x, CenterF.y) , ptScreenF );
		m_layout.p2s( ZoneS, CPoint(CenterS.x, CenterS.y) , ptScreenS );
		ptPos.Offset( sizeOffset );
		if( PtInLine( ptPos, ptScreenF, ptScreenS, 5 ) )
		{
			m_CurrentLink = itLink;
			m_CurrentChair = m_ChairSet.end();
			return;
		}
	}

    for (MyChair::ChairSet::iterator it = m_ChairSet.begin(); it != m_ChairSet.end(); ++it)
    {
		const Vec2fArr& Curve = it->curve;
		m_layout.s2p( m_pDoc->IdToIndex(it->subFrameNo), point, ptPos );
		ptPos.Offset( sizeOffset );

      if (vislib::CurveAndPointRelation( Curve.begin(), Curve.end(), ptPos.x, ptPos.y ) > 0)
      {
		  StartEdit(pView);
		std::auto_ptr<CurrentLine> Temp( new CurrentLine( it, ptTruePoint ) );
		m_pCurrentLine = Temp;
		m_pCurrentLine->Draw(pView);
		break;
      }
    }
#endif
}

class TransformLayer
{
	DWORD						m_nIndex;
	SparceRectImageLayout&	m_layout;
public:
	TransformLayer( SparceRectImageLayout& layout, DWORD nIndex ): m_nIndex(nIndex) , m_layout(layout){}
	TransformLayer( const  TransformLayer& tl ): 
		m_nIndex(tl.m_nIndex) , m_layout(tl.m_layout){}

	CPoint operator()( const Vec2f& vec) 
	{
		CPoint point;
		m_layout.p2s( m_nIndex, vec, point );
		return point;
	}
};

void CGaugeHallDoc::AllImageData::OnLButtonUpHandler( CView* , UINT nFlags, CPoint  )
{

}

void CGaugeHallDoc::AllImageData::OnRButtonDownHandler( CView* pView, UINT nFlags, CPoint point )
{
	UNREFERENCED_PARAMETER(nFlags);
	CSize sizeOffset ( pView->GetScrollPos( SB_HORZ ), pView->GetScrollPos( SB_VERT ) );
	CPoint ptPos(point);
	ptPos.Offset( sizeOffset );

	for( size_t i = 0; i < m_layout.GetCells().size(); ++i )
	{
		CRect rc = m_layout.rectangle(i);
		if( rc.PtInRect( point )  )
		{
			CMenu menu;
			menu.CreatePopupMenu();
			menu.AppendMenu( MF_BYCOMMAND, 1, _T("Load image...") );
			CPoint ptCursor;
			GetCursorPos(&ptCursor);
			BOOL res = menu.TrackPopupMenu( TPM_NONOTIFY|TPM_RETURNCMD, ptCursor.x, ptCursor.y, 0 );
			if( res )
			{
				m_pDoc->LoadImage( *this, m_layout.GetCells()[i].m_nX, m_layout.GetCells()[i].m_nY  );
			}
		}
	}
}

void CGaugeHallDoc::AllImageData::OnMouseMoveHandler( CView* pView, UINT nFlags, CPoint point )
{
	UNREFERENCED_PARAMETER(nFlags);
	if( m_pCurrentLine.get() )
	{
		CSize sizeOffset ( pView->GetScrollPos( SB_HORZ ), pView->GetScrollPos( SB_VERT ) );
		CPoint ptTruePoint(point);
		ptTruePoint.Offset( sizeOffset );
		//m_pCurrentLine
		m_pCurrentLine->Move(ptTruePoint);
		m_pCurrentLine->Draw(pView);
	}
}

void CGaugeHallDoc::AllImageData::OnKeyDownHandler( CView* pView, UINT nChar, UINT nRepCnt, UINT nFlags )
{
	UNREFERENCED_PARAMETER(pView);
	UNREFERENCED_PARAMETER(nChar);
	UNREFERENCED_PARAMETER(nRepCnt);
	UNREFERENCED_PARAMETER(nFlags);
}

void CGaugeHallDoc::AllImageData::OnKeyUpHandler( CView* pView, UINT nChar, UINT nRepCnt, UINT nFlags )
{
	UNREFERENCED_PARAMETER(pView);
	UNREFERENCED_PARAMETER(nChar);
	UNREFERENCED_PARAMETER(nRepCnt);
	UNREFERENCED_PARAMETER(nFlags);
}

void CGaugeHallDoc::AllImageData::InitMode( CView* pView )
{
	//FIXME: 
	ASSERT_KINDOF( CScrollView, pView );

	if( m_layout.GetMode() == RectImageLayout::ftKeepSize )
	{

	std::pair<DWORD,DWORD> MaxRect = m_layout.GetMaxNonEmptyRect();
	const int dwW = m_ImageDimension.first	* MaxRect.first;
	const int dwH = m_ImageDimension.second * MaxRect.second;
	((CScrollView*)pView)->SetScrollSizes( MM_TEXT, CSize(dwW, dwH) );
	}
	else
	{
		((CScrollView*)pView)->SetScrollSizes( MM_TEXT, CSize(1, 1) );
	}
}

void CGaugeHallDoc::AllImageData::StartEdit(CView* pView)
{
	pView->SetCapture();

	CRect rcClient;
	pView->GetClientRect(&rcClient);
	pView->ClientToScreen(&rcClient);
	ClipCursor(&rcClient);
}

void CGaugeHallDoc::AllImageData::EndEdit(CView* pView)
{
	ClipCursor(NULL);
	ReleaseCapture();

	if( m_pCurrentLine.get() )
	{
		m_pCurrentLine->Invalidate(pView);
		std::auto_ptr<CurrentLine> Temp( NULL );
		m_pCurrentLine = Temp;
	}
}

void CGaugeHallDoc::AllImageData::DeleteCurrentChair()
{
	m_pDoc->DeleteChair( m_CurrentChair );
	m_CurrentChair = m_ChairSet.end();
}

void CGaugeHallDoc::AllImageData::DeleteCurrentLink()
{
	m_pDoc->DeleteLink( m_CurrentLink );
	m_CurrentLink = m_ChairLink.end();
}

bool CGaugeHallDoc::AllImageData::OnDeleteChair( MyChair::ChairSet::iterator			itr )
{
	UNREFERENCED_PARAMETER(itr);
	return true;	
}

bool CGaugeHallDoc::AllImageData::OnDeleteLink( ChairLinkList_t::iterator	itr )
{
	UNREFERENCED_PARAMETER(itr);
	return true;
}



/// Here is again CGaugeHallDoc

void CGaugeHallDoc::PostLoadImage(ImageData& id)
{
	id.LoadFloatImage();
	UpdateAllViews(0);
}

void CGaugeHallDoc::DisableDBButton( CCmdUI * pCmdUI, bool bMustViewAll )
{
	bool res;
	if( IsStandAlone() )
		res = false ;
	else
		res = GetCfgDB().UseDB() && GetDB().IsConnected() && bMustViewAll ? IsViewAll() : true  ;
	pCmdUI->Enable( res ? TRUE : FALSE);
}

void CGaugeHallDoc::OnDBGraphLoad()
{
	try
	{
		int nRoomID = GetCurrentRoom();
#if 0
		MyChair::ChairSet&	ChSet	= m_ChairSet[ nRoomID ];
		ChairLinkList_t&	ChLink	= m_ChairLink[ nRoomID ];
#endif
		MyChair::ChairSet	ChSet;
		ChairLinkList_t	ChLink;

		{
		LongOperationAnim op( AfxGetMainWnd()->GetSafeHwnd() );
		DBSerializer dbs;
		std::vector<BYTE> ChairBinaryArr;
		dbs.LoadChairs( nRoomID, ChairBinaryArr );
		ChairLoader cl;
		IChairSerializer::PositionMap_t PosMap;
		cl.LoadFromBinary( ChairBinaryArr, ChSet, ChLink, PosMap );
		}
		PostLoadProcessGraph( nRoomID, ChSet, ChLink );
		PostLoadGraph();
	}
	catch(CommonException&)
	{
		AfxMessageBox( IDS_ERROR_DB_LOADING, MB_OK|MB_ICONERROR );
	}
}

void CGaugeHallDoc::OnDBImageLoad(  )
{
	int nRoomID = GetCurrentRoom();
	// hazard
	AllImageData& All = *m_AllImageData[ nRoomID ];
	std::vector< std::pair<int, ImageData* > > ImgData = GetImageDataInRoomPair( nRoomID );
	bool bAllImages = true;
	
	try
	{
		{
		LongOperationAnim op( AfxGetMainWnd()->GetSafeHwnd() );
		DBSerializer dbs;
		for( size_t i = 0; i < ImgData.size() ; ++i)
		{
			ImageData* id = ImgData[i].second;
			try
			{
				dbs.LoadBackgroundImage( ImgData[i].first, id->m_hallImage );
				PostLoadImage(*id);
			}
			catch( CommonException )
			{
				bAllImages = false;
			}
		}
		}
		if( !bAllImages )
			AfxMessageBox( IDS_ERROR_DB_LOADING, MB_OK|MB_ICONERROR );
		else
			All.PostLoadImage();
	}
	catch(CommonException )
	{
		AfxMessageBox( IDS_ERROR_DB_LOADING, MB_OK|MB_ICONERROR );
	}
}

void CGaugeHallDoc::OnDBImageLoadUpdate(CCmdUI* pCmdUI)
{
	DisableDBButton( pCmdUI, true );
}

void CGaugeHallDoc::OnDBGraphLoadUpdate(CCmdUI* pCmdUI)
{
	DisableDBButton( pCmdUI, true );
}

void CGaugeHallDoc::OnDBGraphSave()
try
{
	int nRoomID = GetCurrentRoom();
	MyChair::ChairSet&	ChSet	= m_ChairSet[ nRoomID ];
	ChairLinkList_t		ChLink	= m_ChairLink[ nRoomID ];

	ChairSaver cs;
	std::vector<BYTE> ChairBinaryArr;
	IChairSerializer::PositionMap_t PosMap	= GetCameraPosition( nRoomID );
	cs.SaveToBinary( ChairBinaryArr, ChSet, ChLink, PosMap );
	DBSerializer dbs;
	dbs.SaveChairs( nRoomID, ChairBinaryArr );
}
catch(CommonException&)
{
	AfxMessageBox( IDS_ERROR_DB_SAVING, MB_OK|MB_ICONERROR );
};

void CGaugeHallDoc::OnDBGraphSaveUpdate(CCmdUI* pCmdUI)
{
	DisableDBButton( pCmdUI, true );
}

void CGaugeHallDoc::OnInit()
{
	InitAll();

	CWnd* pView = GetView();
	m_dlgMoveImage.Create( COffsetChairDialog::IDD, pView);
	m_dlgMoveChair.Create( COffsetChairDialog::IDD, pView);

	m_dlgMoveChair.SetCallback( &m_cbMoveChair );
	m_dlgMoveImage.SetCallback( &m_cbMoveImage );
}

void CGaugeHallDoc::OnNewGraph()
{
	int nRoomID = GetCurrentRoom();
	m_ChairSet[nRoomID].clear();
	m_ChairLink[nRoomID].clear();
	PostLoadGraph();
}

void CGaugeHallDoc::OnFileSaveImage()
{
	// FIXME to Update function
	if( IsViewAll() ) return;
	int nCameraID = GetCurrentCamera();
	ImageMap_t::iterator it = m_ImageData.find( nCameraID );
	if( it == m_ImageData.end() ) return;
	if( it->second->m_hallImage.empty() ) return;
	

	CFileDialogEx dlg( FALSE, WILDCARD_BMP,0, OFN_READONLY|OFN_OVERWRITEPROMPT, BMP_FILTER, ::AfxGetMainWnd() );
	if( dlg.DoModal() == IDOK)
	{
		SaveFloatImage( dlg.GetPathName(), it->second->m_hallImage, INVERT_AXIS_Y );
	}
}

void CGaugeHallDoc::OnDBImageSave()
{
//	AllImageData& All = *m_AllImageData[ GetCurrentRoom() ];
#if 0
	CCameraDialog dlg(  All.m_layout.GetCells() ) ;
	if( dlg.DoModal() == IDOK )
	{
		try
		{		
		const std::vector<int>& Ids = dlg.GetResultVector();
		std::vector< std::pair< int,int> > IndexMap = dlg.GetFromLayout(
			All.m_layout.GetCells() );
		DBSerializer dbs;

			LongOperationAnim op( AfxGetMainWnd()->GetSafeHwnd() );
			for(size_t i = 0; i < IndexMap.size(); ++i)
			{
				int nImageIndex = IndexMap[i].first;
				int nCameraID	= Ids[ IndexMap[i].second ];
				CString str;
				dbs.SaveBackgroundImage( nCameraID, m_hallImage[ nImageIndex ] );
				//	str.Format( _T("%d::%d\n"), nImageIndex, nCameraID  );
				//	OutputDebugString(str);
			}
		}
		catch(CommonException& )
		{
			//FIXME: to resources
			AfxMessageBox( IDS_ERROR_DB_SAVING, MB_OK|MB_ICONERROR );
		}
	}
#endif
	int nRoomID = GetCurrentRoom();
	// danger
	AllImageData& All = *m_AllImageData[ nRoomID ];
	std::vector< std::pair<int, ImageData* > > ImgData = GetImageDataInRoomPair( nRoomID );
	bool bAllImages = true;
	try
	{
		{
			LongOperationAnim op( AfxGetMainWnd()->GetSafeHwnd() );
			DBSerializer dbs;
			for( size_t i = 0; i < ImgData.size() ; ++i)
			{
				ImageData* id = ImgData[i].second;
				try
				{
					dbs.SaveBackgroundImage( ImgData[i].first, id->m_hallImage );
					PostLoadImage(*id);
				}
				catch( CommonException )
				{
					bAllImages = false;
				}
			}
		}
		if( !bAllImages )
			AfxMessageBox( IDS_ERROR_DB_SAVING, MB_OK|MB_ICONERROR );
	}
	catch(CommonException )
	{
		AfxMessageBox( IDS_ERROR_DB_SAVING, MB_OK|MB_ICONERROR );
	}
}

void CGaugeHallDoc::OnDBImageSaveUpdate(CCmdUI* pCmdUI)
{
	DisableDBButton( pCmdUI, true );
}

void	CloneFromZone( int SrcZone, int nDstZone, const MyChair::ChairSet& SrcSet, MyChair::ChairSet& DstSet, int& Id )
{
	MyChair::ChairSet::const_iterator itr = SrcSet.begin();
	for(; itr != SrcSet.end(); ++itr)
	{
		MyChair::Chair ch = (*itr);
		if( ch.subFrameNo == SrcZone )
		{
			ch.id			= Id++;
			ch.subFrameNo	= nDstZone;
			DstSet.push_back( ch );
		}
	}
}

void CGaugeHallDoc::OnClone()
{
#ifdef  USE_OLD_HALL
	std::vector< int > NewZoneIndex( m_ZoneIndex  ) ;
	NewZoneIndex.resize( m_hallImage.size() );
	CCloneIDDialog dlg( GetView(), NewZoneIndex );
	std::vector<int> CloneIndices;
	if( dlg.DoModal() == IDOK )
	{
		CloneIndices = dlg.m_Result;
		ASSERT( NewZoneIndex.size() == CloneIndices.size() );

		MyChair::ChairSet::iterator itr = m_ChairSet.begin();
		int dwMaxId = 0;
		for(; itr != m_ChairSet.end(); ++itr)
		{
			dwMaxId = std::_cpp_max<int>( dwMaxId, itr->id );
		}
		++dwMaxId;

		MyChair::ChairSet NewChairSet;
		for( size_t i = 0; i < CloneIndices.size(); ++i)
		{
			if( CloneIndices[i] == CCloneIDDialog::NO_CLONE )
			{
				CloneFromZone( NewZoneIndex[i], NewZoneIndex[i], m_ChairSet, NewChairSet, dwMaxId );
			}
			else
			{
				CloneFromZone( CloneIndices[i], NewZoneIndex[i], m_ChairSet, NewChairSet, dwMaxId );
			}
		}
		m_ChairSet = NewChairSet;
		PostLoadGraph();
	}
#endif
}

CGaugeHallFrame*		CGaugeHallDoc::GetHallFrame()
{
	CGaugeHallFrame* z = dynamic_cast<CGaugeHallFrame*>( GetView()->GetParentFrame() );
	ASSERT(z);
	return z;
}

bool	CGaugeHallDoc::IsViewAll()
{
	return GetHallFrame()->ShowAllCameras();
}

int		CGaugeHallDoc::GetCurrentCamera()
{
	return GetHallFrame()->GetCurCamera();
}

int		CGaugeHallDoc::GetCurrentRoom()
{
	const int c_nAlwaysRoom = 1;
	return IsStandAlone()?c_nAlwaysRoom :  GetHallFrame()->GetCurRoom();
}

void	CGaugeHallDoc::GetCameras( int nRoomID, std::vector<int>& CameraNo )
{
	if( GetCfgDB().UseDB() && GetDB().IsConnected() )
	{
		std::set<int> z = GetDB().GetTableCameraFacet().SelectCamerasFromRoom( nRoomID );
		CameraNo.assign( z.begin(), z.end() );
	}
	else
	{
		//ASSERT(FALSE);
		CameraNo.clear();
		ImageMap_t::iterator it = m_ImageData.begin();
		for(; it != m_ImageData.end() ; ++it)
		{
			CameraNo.push_back( it->first );
		}
	}
}

void	CGaugeHallDoc::InitAll()
{
	bool res = GetCfgDB().UseDB() && GetDB().IsConnected();
	m_bStandAlone = !res;
	if( res )
	{
		std::map<int, int> z = GetDB().GetTableRoomFacet().GetRoomNumbers();
		std::map<int, int>::const_iterator it = z.begin();
		for( ; it != z.end(); ++it )
		{
			int nRoomID =  it->first;
			MyChair::ChairSet&	ChSet	= m_ChairSet[ nRoomID ];
			ChairLinkList_t&	ChLink	= m_ChairLink[ nRoomID ];

			boost::shared_ptr<AllImageData> p ( new AllImageData( this, m_ImageData, nRoomID, ChSet, ChLink  ) );
			m_AllImageData.insert(  std::make_pair(nRoomID, p ));
		}
		const std::set<CameraRow>& Cams = GetDB().GetTableCameraFacet().GetCameras();
		std::set<CameraRow>::const_iterator itC = Cams.begin();
		for( ; itC != Cams.end(); ++itC )
		{
			MyChair::ChairSet& ChSet = m_ChairSet[ itC->m_nRoomID ];
			boost::shared_ptr<ImageData> p ( new ImageData( this, ChSet, itC->m_nID, m_IdGenerator ));
			m_ImageData.insert(  std::make_pair( itC->m_nID , p ));
		}
	}
	else
	{
	//	ASSERT(FALSE);
		int CamIDArr[] =  { 1 };
		const int nRoomID = 1;

		MyChair::ChairSet&	ChSet	= m_ChairSet[ nRoomID ];
		ChairLinkList_t&	ChLink	= m_ChairLink[ nRoomID ];

		for( size_t i = 0 ; i < sizeof(CamIDArr)/sizeof(int); ++i )
		{
			int nCameraID = CamIDArr[i];
			MyChair::ChairSet& ChSet = m_ChairSet[ nCameraID ];
			boost::shared_ptr<ImageData> p ( new ImageData( this, ChSet, nCameraID, m_IdGenerator ));
			m_ImageData.insert(  std::make_pair( nCameraID , p ));
			m_StandAloneCameras.push_back( nCameraID );
		}
		boost::shared_ptr<AllImageData> p ( new AllImageData( this, m_ImageData, nRoomID, ChSet, ChLink  ) );
		m_AllImageData.insert(  std::make_pair(nRoomID, p ));
	}
	GetHallFrame()->InitRoomChange(  );
	RecalcLayout( true );
}

void CGaugeHallDoc::OnRoomChange( int nRoomID )
{
	// one room
	bool res = GetCfgDB().UseDB() && GetDB().IsConnected();
	if( !res ) return;
	AllImageMap_t::iterator it = m_AllImageData.find( GetCurrentRoom() );
	if( it == m_AllImageData.end() ) return;
	AllImageData& All = *it->second;
	std::set<int>  Cams = GetDB().GetTableCameraFacet().SelectCamerasFromRoom( GetCurrentRoom() );
	const std::set<CameraRow>&  AllCams = GetDB().GetTableCameraFacet().GetCameras( );
	std::set<int>::const_iterator itC = Cams.begin();
	SparceRectImageLayout::IndexArray_t Arr;
	for( ; itC != Cams.end(); ++itC )
	{
		std::set<CameraRow>::const_iterator it = AllCams.find( *itC );
		if( it != AllCams.end() )
		{
			std::pair<int, int> p = it->m_Pos;
			SparceRectImageLayout::Cell c( p.first , p.second );
			Arr.push_back( c );
		}
	}
	SparceRectImageLayout TmpLayer;
	TmpLayer.SetCells( Arr );
	std::pair<DWORD, DWORD> rectNonEmpty = TmpLayer.GetMaxNonEmptyRect();
	All.m_layout.SetArray(rectNonEmpty.first, rectNonEmpty.second);
	All.m_layout.SetMode( RectImageLayout::ftKeepSize );
	All.m_layout.SetCells( TmpLayer.GetCells() );
	//GetView()->SetWindowPos( 0, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE );
	All.RecalcLayout( GetView(), true );
	InitMode();
}

void CGaugeHallDoc::OnCameraChange( int nCameraID )
{
	//if( nCameraID )
	if( IsViewAll() )
	{
		AllImageMap_t::iterator it = m_AllImageData.find( GetCurrentRoom() );
		if( it == m_AllImageData.end() ) return;
		AllImageData& All = *it->second;
		All.InitMode( GetView() );
	}
	else
	{
		ImageMap_t::iterator it = m_ImageData.find( nCameraID );
		if( it != m_ImageData.end() )
		{
			it->second->InitMode( GetView() );
		}
	}
}

void CGaugeHallDoc::OnImageGrab()
{
#if 0
	bool res = GetCfgDB().UseDB() && GetDB().IsConnected();
	if( !res ) return;
#endif

	int nRoomID = GetCurrentRoom();
	if( GetCfgDB().UseDB() && GetDB().IsConnected() )
	{
		if( !IsLayoutValid(nRoomID) ) return ;
	}

	AllImageMap_t::iterator it = m_AllImageData.find( nRoomID );
	if( it == m_AllImageData.end() ) return;
	AllImageData& All = *it->second;

	std::vector<int> CamArr;
	if( GetCfgDB().UseDB() && GetDB().IsConnected() )
	{
		std::set<int> Cams = GetDB().GetTableCameraFacet().SelectCamerasFromRoom( nRoomID );
		CamArr.assign( Cams.begin(), Cams.end() );
	}
	else
	{
		CamArr = m_StandAloneCameras;
	}

	CFileGrabDialog	dlg( All.m_layout, CamArr );
	if( dlg.DoModal() == IDOK )
	{
		int nRoomID = GetCurrentRoom();
		AllImageMap_t::iterator itA = m_AllImageData.find( nRoomID );
		for( size_t i = 0; i < CamArr.size(); ++i )
		{
			int nCamID = CamArr[i];
			ImageMap_t::iterator it = m_ImageData.find( nCamID );

			// FIXME : size of image may vary
			if( it != m_ImageData.end() )
			{
				Arr2f& z = it->second->m_hallImage;
				z = dlg.GetImages()[i];
				if( itA != m_AllImageData.end()  && i == 0)
				{
					itA->second->SetImageDim( z.width(), z.height() );
					itA->second->RecalcLayout( GetView(), true );
					//		itA->second->InitMode( GetView() );
				}
				PostLoadImage( *it->second );
			}
		}
		InitMode();
	}
}

void CGaugeHallDoc::OnGrabCamera()
{
	bool res = GetCfgDB().UseDB() && GetDB().IsConnected()  ;
	if( !res ) return;
	int nRoomID = GetCurrentRoom();
	res = IsLayoutValid(nRoomID);
	if( !res ) return;
	AllImageMap_t::iterator it = m_AllImageData.find( nRoomID );
	if( it == m_AllImageData.end() ) return;
	AllImageData& All = *it->second;
	
	const std::set<CameraRow> &  Cams = GetDB().GetTableCameraFacet().GetCameras( );
	std::set<CameraRow>::const_iterator itC = Cams.begin();
	SparceRectImageLayout::IndexArray_t Arr;
	std::vector< std::pair< int, std::wstring> > CamArr;
	for( ; itC != Cams.end(); ++itC )
	{
		if( itC->m_nRoomID == nRoomID )
		{
			std::pair<int, int> p = itC->m_Pos;
			SparceRectImageLayout::Cell c( p.first , p.second );
			Arr.push_back( c );
			CamArr.push_back( std::make_pair( itC->m_nID, itC->m_sGUID )  );
		}
	}
	SparceRectImageLayout TmpLayer;
	TmpLayer.SetCells( Arr );
	std::pair<DWORD, DWORD> rectNonEmpty = TmpLayer.GetMaxNonEmptyRect();
	TmpLayer.SetArray(rectNonEmpty.first, rectNonEmpty.second);
	TmpLayer.SetMode( RectImageLayout::ftKeepAspectRatio );
	
	std::vector< boost::shared_ptr< IBaseGrabber > > GrabArr;
	for( size_t i= 0; i < CamArr.size() ; ++i )
	{
		boost::shared_ptr<IBaseGrabber> grb = GetCameraManager().CreateGrabber( CamArr[i].first, CamArr[i].second.c_str(), 8, true );
		if( !grb )
		{
			CString s;
#if 0
			s.Format( _T("Failed to create grabber for camera %d w/ source %s"), CamArr[i].first, CamArr[i].second.c_str() );
#else
			s.Format(IDS_ERROR_GRABBER, CamArr[i].first);
#endif
			AfxMessageBox( s, MB_OK|MB_ICONERROR );
			return;
		}
		GrabArr.push_back( grb );
	}

	CameraGrabDialog dlg( TmpLayer, GrabArr );
	if( dlg.DoModal() == IDOK )
	{
		int nRoomID = GetCurrentRoom();
		AllImageMap_t::iterator itA = m_AllImageData.find( nRoomID );
		for(size_t i = 0; i < CamArr.size(); ++i )
		{
			int nCamID = CamArr[i].first;
			ImageMap_t::iterator it = m_ImageData.find( nCamID );
			
			// FIXME : size of image may vary
			if( it != m_ImageData.end() )
			{
				Arr2f& z = it->second->m_hallImage;
				z = dlg.GetImages()[i];
				if( itA != m_AllImageData.end()  && i == 0)
				{
					itA->second->SetImageDim( z.width(), z.height() );
					itA->second->RecalcLayout( GetView(), true );
			//		itA->second->InitMode( GetView() );
				}
				PostLoadImage( *it->second );
			}
		}
		InitMode();
	}
}

void CGaugeHallDoc::OnLayout()
{
	int nRoomID = GetCurrentRoom();
	AllImageMap_t::iterator it = m_AllImageData.find( nRoomID );
	if( it == m_AllImageData.end() ) return;
	AllImageData& All = *it->second;

	if( IsStandAlone()  )
	{
		CSelectArrangementDlg dlg;
		if( dlg.DoModal() != IDOK ) return;

		m_ImageData.clear();
		m_StandAloneCameras = dlg.GetCameras();

		for( size_t i = 0 ; i < m_StandAloneCameras.size() ; ++i )
		{
			int nCameraID = m_StandAloneCameras[i];
			MyChair::ChairSet& ChSet = m_ChairSet[ nRoomID ];
			boost::shared_ptr<ImageData> p ( new ImageData( this, ChSet, nCameraID, m_IdGenerator ));
			m_ImageData.insert(  std::make_pair( nCameraID , p ));
		}

		All.m_layout.SetCells( dlg.GetLayout().GetCells() );
		std::pair<int, int> p = dlg.GetLayout().GetArray();
		All.m_layout.SetArray( p.first, p.second );
		GetHallFrame()->InitRoomChange(  );
		RecalcLayout( true );
		return;
	}

	const std::set<CameraRow> &  Cams = GetDB().GetTableCameraFacet().GetCameras( );
	std::set<CameraRow>::const_iterator itC = Cams.begin();
	SparceRectImageLayout::IndexArray_t Arr;
	std::vector< std::pair< int, std::wstring> > CamArr;
	for( ; itC != Cams.end(); ++itC )
	{
		if( itC->m_nRoomID == nRoomID )
		{
			std::pair<int, int> p = itC->m_Pos;
			SparceRectImageLayout::Cell c( p.first , p.second );
			Arr.push_back( c );
			CamArr.push_back( std::make_pair( itC->m_nID, itC->m_sGUID )  );
		}
	}
	SparceRectImageLayout TmpLayer;
	TmpLayer.SetCells( Arr );
	std::pair<DWORD, DWORD> rectNonEmpty = TmpLayer.GetMaxNonEmptyRect();
	TmpLayer.SetArray(rectNonEmpty.first, rectNonEmpty.second);
	TmpLayer.SetMode( RectImageLayout::ftKeepAspectRatio );

	std::vector< boost::shared_ptr< IBaseGrabber > > GrabArr;
	for( size_t i= 0; i < CamArr.size() ; ++i )
	{
		boost::shared_ptr<IBaseGrabber> grb = GetCameraManager().CreateGrabber( CamArr[i].first, CamArr[i].second.c_str(), 24, true );
		if( !grb )
		{
			CString s;
#if 0
			s.Format( _T("Failed to create grabber for camera %d w/ source %s"), CamArr[i].first, CamArr[i].second.c_str() );
#else
			s.Format(IDS_ERROR_GRABBER, CamArr[i].first);
#endif
			AfxMessageBox( s, MB_OK|MB_ICONERROR );
			return;
		}
		GrabArr.push_back( grb );
	}

	AvailableSourceArr_t CamSrc;
	for( size_t i= 0; i < CamArr.size() ; ++i )
	{
		CamSrc.push_back( std::make_pair( CamArr[i].first, 
			boost::shared_ptr< CBackground > ( new CFrameBackground( GrabArr[i] ) )
			) );
	}

	CLayoutDialog dlg( CamSrc, true );
	if( dlg.DoModal() == IDOK )
	{
		const SparceRectImageLayout& Tmp =  dlg.GetResult();
		std::pair<int, int> z = Tmp.GetArray();
		All.m_layout.SetArray( z.first, z.second );
		All.m_layout.SetCells( Tmp.GetCells() );

		ASSERT(CamArr.size() == Tmp.GetCells().size()  );
		std::map<int, std::pair<int, int> > LayoutMap;
		for( size_t i =0; i < Tmp.GetCells().size(); ++i )
		{
			const SparceRectImageLayout::Cell& c = Tmp.GetCells()[i];
			LayoutMap[ CamArr[i].first ] = std::make_pair( c.m_nX, c.m_nY ) ;
			GetDB().GetTableCameraFacet().UpdateLayout( LayoutMap );
		}

		UpdateAllViews(0);
	}
}

std::vector< CGaugeHallDoc::ImageData*	> CGaugeHallDoc::GetImageDataInRoom( int nRoomID )
{
	std::vector< CGaugeHallDoc::ImageData*	> Images;
	bool res = GetCfgDB().UseDB() && GetDB().IsConnected();
	if( res )
	{
		std::set<int> Cams = GetDB().GetTableCameraFacet().SelectCamerasFromRoom( nRoomID );
		std::set<int>::const_iterator it = Cams.begin();
		for(; it != Cams.end(); ++it )
		{
			ImageMap_t::iterator itID = m_ImageData.find( *it );
			if( itID != m_ImageData.end() )
			{
				Images.push_back( itID->second.get() );
			}
		}
	}
	else
	{
		// just copy from m_ImageData when nRoomID == 1
		ImageMap_t::iterator itID = m_ImageData.begin();
		for( ; itID != m_ImageData.end(); ++itID )
		{
			Images.push_back( itID->second.get() );
		}
	}
	return Images;
}

std::vector< std::pair<int, CGaugeHallDoc::ImageData*>	>	CGaugeHallDoc::GetImageDataInRoomPair( int nRoomID )
{
	std::vector< std::pair<int, ImageData*>	> Images;
	bool res = GetCfgDB().UseDB() && GetDB().IsConnected();
	if( res )
	{
		std::set<int> Cams = GetDB().GetTableCameraFacet().SelectCamerasFromRoom( nRoomID );
		std::set<int>::const_iterator it = Cams.begin();
		for(; it != Cams.end(); ++it )
		{
			ImageMap_t::iterator itID = m_ImageData.find( *it );
			if( itID != m_ImageData.end() )
			{
				Images.push_back( std::make_pair( itID->first, itID->second.get() ) );
			}
		}
	}
	else
	{
		// just copy from m_ImageData when nRoomID == 1
		ImageMap_t::iterator itID = m_ImageData.begin();
		for( ; itID != m_ImageData.end(); ++itID )
		{
			Images.push_back( std::make_pair( itID->first, itID->second.get() ) );
		}
	}
	return Images;
}

bool	CGaugeHallDoc::AllImageData::PostLoadImage( )
{
	std::vector< CGaugeHallDoc::ImageData*	> ImgArr = m_pDoc->GetImageDataInRoom( m_nRoomID );
	// TODO : check image size
	for( size_t i = 0; i < ImgArr.size(); ++i )
	{
		if( i == 0 )
		{
			const Arr2f& z = ImgArr[i]->m_hallImage;
			SetImageDim( z.width(), z.height() );
		}
	}
	RecalcLayout( m_pDoc->GetView(), true );
	InitMode( m_pDoc->GetView() );
	return true;
}

void	CGaugeHallDoc::InitMode()
{
	if( IsViewAll() )
	{
		int nRoomID = GetCurrentRoom();
		AllImageMap_t::iterator it = m_AllImageData.find( nRoomID );
		if( it == m_AllImageData.end() ) return;
		AllImageData& All = *it->second;
		All.InitMode( GetView() );
	}
	else
	{
		int nCameraID = GetCurrentCamera();
		ImageMap_t::iterator it = m_ImageData.find( nCameraID );
		if( it == m_ImageData.end() ) return;
		ImageData& All = *it->second;
		All.InitMode( GetView() );
	}
}

void CGaugeHallDoc::OnFileSaveGraphUpdate (CCmdUI* pCmdUI)
{
	int nCameraID = GetCurrentCamera();
	if( !IsViewAll() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
}

void CGaugeHallDoc::OnFileLoadGraphUpdate (CCmdUI* pCmdUI)
{
	int nCameraID	= GetCurrentCamera();
	int nRoomID		= GetCurrentRoom();
	if( !IsViewAll()					|| 
		!IsAllImagesLoaded( nRoomID )	||
		!IsLayoutValid(nRoomID) )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
}

void CGaugeHallDoc::OnFileLoadImageUpdate (CCmdUI* pCmdUI)
{
	int nCameraID = GetCurrentCamera();
	if(  nCameraID == -1 )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if( IsViewAll() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
}

void CGaugeHallDoc::OnFileSaveImageUpdate (CCmdUI* pCmdUI)
{
	int nCameraID = GetCurrentCamera();
	if(  nCameraID == -1 )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if( IsViewAll() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	ImageMap_t::iterator it = m_ImageData.find( nCameraID );
	if( it == m_ImageData.end() ) 
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	ImageData& All = *it->second;
	if( All.m_hallImage.empty() )
	{
		pCmdUI->Enable(FALSE);
		return;
	}
}

bool	CGaugeHallDoc::IsLayoutValid( int nRoomID )
{
	AllImageMap_t::iterator it = m_AllImageData.find( nRoomID );
	if( it == m_AllImageData.end() ) return false;
	AllImageData& All = *it->second;
	LayoutValidate lv;
	return lv.Process( All.m_layout );
}

bool	CGaugeHallDoc::IsAllImagesLoaded( int nRoomID )
{
	std::vector< ImageData*	>	ImgArr = GetImageDataInRoom(  nRoomID );
//	bool bOK = true;
	for( size_t i = 0 ; i <  ImgArr.size(); ++i)
	{
		if( ImgArr[i]->m_hallImage.empty() ||
			ImgArr[i]->m_demoImage.empty() )
		{
			return false;
		}
	}
	return true;
}

void	CGaugeHallDoc::PostLoadProcessGraph( int nRoomID, MyChair::ChairSet& ChSet, ChairLinkList_t& ChLink )
{
	AllImageMap_t::iterator it = m_AllImageData.find( nRoomID );
	if( it == m_AllImageData.end() ) return ;
	AllImageData& All = *it->second;

	std::vector< std::pair<int, ImageData*>	>	ImgArr = GetImageDataInRoomPair(  nRoomID );

	std::vector<Arr2ub*>	SrcArr;
	std::vector<int>		IdArr;
	for( size_t i = 0; i < ImgArr.size(); ++i )
	{
		SrcArr.push_back( &ImgArr[i].second->m_demoImage );
		IdArr.push_back( ImgArr[i].first );
	}

	CRenumberDialog dlg( SrcArr, IdArr, All.m_layout, ChSet );
	if( dlg.DoModal() == IDOK )
	{
		m_ChairSet[ nRoomID ]	= ChSet;
		m_ChairLink[ nRoomID ]	= ChLink;
	}
}

void CGaugeHallDoc::OnParamShow()
{
	bool res = GetCfgDB().UseDB() && GetDB().IsConnected();
	if( !res ) return;
	int nRoomID = GetCurrentRoom();
	if( nRoomID < 0 ) return;

	try
	{
		std::vector<BYTE>	ParamArr;
		bool res = GetDB().GetTableRoomFacet().GetAlgoParam( nRoomID, ParamArr );
		Int8Arr Param;
		if( !res )
		{
			HRESULT hr = SetDefaultParameters( &Param, NULL );
			ASSERT( hr == S_OK );
		}
		Param.assign( ParamArr.begin(), ParamArr.end() ) ;
		//Param.swap( ParamArr );
		HRESULT hr = ShowParameterDialog( GetView()->GetSafeHwnd(), &Param, NULL );
		if( hr == S_FALSE )
		{
			// user have canceled the action
			return;
		}
		else if( hr == E_INVALIDARG )
		{
			res = AfxMessageBox( IDS_TITLE_PARAM_INVALID, MB_OKCANCEL|MB_ICONWARNING );
			if(  res == IDOK )
			{
				HRESULT hr = SetDefaultParameters( &Param, NULL );
				ASSERT( hr == S_OK );
			}
			else return;
		}
		ParamArr.assign( Param.begin(), Param.end()  );
		res = GetDB().GetTableRoomFacet().SetAlgoParam( nRoomID, ParamArr );
		ASSERT( res );
	}
/*	catch(DataBaseException)
	{
		AfxMessageBox( _T("DB error") );
	}
	catch( CommonException )
	{
		AfxMessageBox( _T("Common error") );
	}*/
	catch( ... )
	{
		AfxMessageBox( IDS_ERROR_DB_LOADING, MB_OK|MB_ICONERROR );
	}
}

void CGaugeHallDoc::OnParamShowUpdate( CCmdUI* pCmdUI )
{
	bool res = GetCfgDB().UseDB() && GetDB().IsConnected();
	pCmdUI->Enable( res ? TRUE : FALSE );
}

void CGaugeHallDoc::OnGrabCameraUpdate( CCmdUI* pCmdUI )
{
	bool res = IsStandAlone();
	pCmdUI->Enable( res ? FALSE : TRUE );
}

void CGaugeHallDoc::OnLayoutUpdate( CCmdUI* pCmdUI )
{
	bool res = true;
	pCmdUI->Enable( res ? TRUE : FALSE );
}

void CGaugeHallDoc::OnDBSaveModel()
try
{
	CString sModelFilter;
	sModelFilter.LoadString( IDS_FILTER_MODEL );
	CFileDialogEx 	dlg(	TRUE, 0, 0, 0, sModelFilter, 0 );
	if( dlg.DoModal() == IDOK )
	{
		std::vector<BYTE>	DataArr;
		CFile fileModel( dlg.GetPathName(), CFile::modeRead );
		size_t nSize = (size_t) fileModel.GetLength();
		DataArr.resize( nSize );
		fileModel.Read( &DataArr[0], DataArr.size() );
		int nRoomID = GetCurrentRoom();
		GetDB().GetTableRoomFacet().SetMergeModel( nRoomID, DataArr );
	}
}
catch( CommonException )
{
	AfxMessageBox( IDS_ERROR_DB_SAVING, MB_OK|MB_ICONERROR );
};

void CGaugeHallDoc::OnDBSaveModelUpdate(CCmdUI* pCmdUI)
{
	bool res = IsStandAlone();
	pCmdUI->Enable( res ? FALSE : TRUE );
}

void CGaugeHallDoc::OnMoveImage(  )
{
	m_dlgMoveImage.ShowWindow(SW_SHOW);
}

void CGaugeHallDoc::OnMoveChair(  )
{
	m_dlgMoveChair.ShowWindow(SW_SHOW);
}

void CGaugeHallDoc::OnMoveImageUpdate( CCmdUI* pCmdUI )
{

}

void CGaugeHallDoc::OnMoveChairUpdate( CCmdUI* pCmdUI )
{

}

void CMoveImageCallback ::Move(int dx, int dy)
{
	m_pDoc->MoveImage(dx, dy);
}

void CMoveChairCallback ::Move(int dx, int dy)
{
	m_pDoc->MoveChair(dx, dy);
}

void CGaugeHallDoc ::MoveImage(int dx, int dy)
{
	int nCameraNo	= GetCurrentCamera();
	if( m_ImageData.find( nCameraNo ) == m_ImageData.end() ) return;
	ImageData&	CurImageData = *m_ImageData[ nCameraNo ].get();
	if( CurImageData.m_hallImage.empty() )	return;
	ShiftImage(CurImageData.m_hallImage, dx, dy);
	PostLoadImage( CurImageData );
	UpdateAllViews(0);	
}

void CGaugeHallDoc ::MoveChair(int dx, int dy)
{
	int nRoomID		= GetCurrentRoom();
	int nCameraNo	= GetCurrentCamera();
	if( m_ChairSet.find( nRoomID ) == m_ChairSet.end() ) return;
	MyChair::ChairSet&	ChSet	= m_ChairSet[ nRoomID ];

	MyChair::ChairSet::iterator crvIt;
	for (crvIt = ChSet.begin(); crvIt != ChSet.end(); ++crvIt)
	{
		if( crvIt->subFrameNo == nCameraNo )
			for(DWORD i=0; i < crvIt->curve.size(); ++i)
			{
				crvIt->curve[i] += Vec2f( float( dx) , float(dy) );
			}
	}
	UpdateAllViews(0);	
}

void CGaugeHallDoc::ShiftImage( Arr2f& img, int dx, int dy)
{
	Arr2f	tmpImg( img );
	for( int y = 0; y < tmpImg.height(); ++y )
		for( int x = 0; x < tmpImg.width(); ++x )
		{
			img.set_if_in( x + dx, y + dy, tmpImg(x, y) );
		}
}

IChairSerializer::PositionMap_t		CGaugeHallDoc::GetCameraPosition( int nRoomID )
{
	IChairSerializer::PositionMap_t PosMap;
	AllImageData& All = *m_AllImageData[ nRoomID ];
	const SparceRectImageLayout::IndexArray_t&	IndArr = All.m_layout.GetCells();
	std::vector< std::pair<int, ImageData*>	>	ImageArr = GetImageDataInRoomPair( nRoomID );
	ASSERT( IndArr.size() == ImageArr.size() );
	for(size_t i =0; i < IndArr.size(); ++i )
	{
		std::pair<int,int> pos( IndArr[i].m_nX, IndArr[i].m_nY );
		PosMap.insert( std::make_pair( ImageArr[i].first, pos ) );
	}
	return PosMap;
}

bool	CGaugeHallDoc::CheckImage( CString sName )
{
	static struct 
	{
		int x, y;
	} 
	s_AvailableSize[] = 
	{
		{ 640, 480 },	// for test
		{ 720, 576 },	// max PAL
		{ 740, 480 }	// max NTSC
	};

	HBITMAP hBmp = (HBITMAP)::LoadImage( AfxGetInstanceHandle(), sName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_LOADFROMFILE );
	ASSERT(hBmp);
	BITMAP BmpStruct;
	::GetObject( hBmp, sizeof(BITMAP), &BmpStruct );
	// FIXME see Width and try to guess Height from templates 640x480 and so on

	bool bFoundSize = false;
	for( size_t i = 0; i < sizeof(s_AvailableSize)/sizeof( s_AvailableSize[0] ); ++i )
	{
		if( s_AvailableSize[i].x == BmpStruct.bmWidth	&&
			s_AvailableSize[i].y == BmpStruct.bmHeight	)
		{
			bFoundSize = true; 
			break;
		}
	}
	::DeleteObject( hBmp );
	return bFoundSize;

}

int		CGaugeHallDoc::PositionToID( int x, int y, int nRoomID )
{
	IChairSerializer::PositionMap_t	PosMap = GetCameraPosition( nRoomID );
	IChairSerializer::PositionMap_t::const_iterator it = PosMap.begin();
	for( ; it != PosMap.end(); ++it )
	{
		if( it->second.first == x	&& 
			it->second.second == y )
		{
			return it->first;
		}
	}
	return -1;
}

std::pair<int,int>	CGaugeHallDoc::IDToPosition( int nCameraID )
{
	if( IsStandAlone() ) 
	{
		AllImageData& All = *m_AllImageData[ GetCurrentRoom() ];
		const SparceRectImageLayout::IndexArray_t& Arr = All.m_layout.GetCells();
		ASSERT( Arr.size() == m_StandAloneCameras.size() );
		for( size_t i = 0; i != Arr.size();  ++i )
		{
			if( m_StandAloneCameras[i] == nCameraID )
			{
				return std::make_pair( Arr[i].m_nX, Arr[i].m_nY );
			}
		}
	}
	else
	{
//		std::set<int>  Cams = GetDB().GetTableCameraFacet().SelectCamerasFromRoom( nRoomID );
		const std::set<CameraRow>&  AllCams = GetDB().GetTableCameraFacet().GetCameras( );
		std::set<CameraRow>::const_iterator itC = AllCams.find( nCameraID );
		return itC->m_Pos;
	}
	return std::make_pair( -1, -1 );
}

size_t	CGaugeHallDoc::GetCamerasInRoom(int nRoomID)
{
	if( IsStandAlone() ) 
	{
		return m_StandAloneCameras.size();
	}
	else
	{
		std::set<int>  Cams = GetDB().GetTableCameraFacet().SelectCamerasFromRoom( nRoomID );
		return Cams.size();
	}
}

CString	CGaugeHallDoc::PostitionToName( int x, int y, size_t nAll )
{
	struct 
	{
		int x; 
		int y; 
		size_t nAll;
		LPCTSTR szName;
	}
		Names [] = 
	{	
		{ 0, 0, 1, _T("tl") },

		{ 0, 0, 4, _T("tl") },
		{ 0, 1, 4, _T("bl") },
		{ 1, 0, 4, _T("tr") },
		{ 1, 1, 4, _T("br") },

		{ 0, 0, 6, _T("tl") },
		{ 0, 1, 6, _T("ml") },
		{ 0, 2, 6, _T("bl") },
		{ 1, 0, 6, _T("tr") },
		{ 1, 1, 6, _T("mr") },
		{ 1, 2, 6, _T("br") }
	};	
	for( size_t i = 0; i < sizeof(Names)/sizeof(Names[0]); ++i )
	{
		if( Names[i].x == x && Names[i].y == y && Names[i].nAll == nAll  )	return CString( Names[i].szName );
	}
	return CString();
}