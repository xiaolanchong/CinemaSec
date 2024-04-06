#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "../res/resource.h"
#include "train_data_doc.h"
#include "train_data_view.h"
#include "train_data_frm.h"
#include "../gui/Grid/MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTrainDataView, BaseIdleView)

BEGIN_MESSAGE_MAP(CTrainDataView, BaseIdleView)
  ON_WM_SIZE()
  ON_WM_LBUTTONDOWN()
  ON_WM_ERASEBKGND()

  ON_COMMAND(IDM_Play_First_Frame, OnFirstFrame)
  ON_COMMAND(IDM_Play_NFrames_Backward, OnNFramesBackward)
  ON_COMMAND(IDM_Play_Prev_Frame, OnPrevFrame)
  ON_COMMAND(IDM_Play_Next_Frame, OnNextFrame)
  ON_COMMAND(IDM_Play_NFrames_Forward, OnNFramesForward)
  ON_COMMAND(IDM_Play_Last_Frame, OnLastFrame)
  ON_COMMAND(IDM_Play_Play_Sequence, OnPlaySequence)
  ON_COMMAND(IDM_Play_Stop_Sequence, OnStopSequence)
  ON_COMMAND(IDM_Play_Speed_Up, OnSpeedUp)
  ON_COMMAND(IDM_Play_Speed_Down, OnSpeedDown)

  ON_UPDATE_COMMAND_UI(IDM_Play_First_Frame, OnUpdateFirstFrame)
  ON_UPDATE_COMMAND_UI(IDM_Play_NFrames_Backward, OnUpdateNFramesBackward)
  ON_UPDATE_COMMAND_UI(IDM_Play_Prev_Frame, OnUpdatePrevFrame)
  ON_UPDATE_COMMAND_UI(IDM_Play_Next_Frame, OnUpdateNextFrame)
  ON_UPDATE_COMMAND_UI(IDM_Play_NFrames_Forward, OnUpdateNFramesForward)
  ON_UPDATE_COMMAND_UI(IDM_Play_Last_Frame, OnUpdateLastFrame)
  ON_UPDATE_COMMAND_UI(IDM_Play_Play_Sequence, OnUpdatePlaySequence)
  ON_UPDATE_COMMAND_UI(IDM_Play_Stop_Sequence, OnUpdateStopSequence)
  ON_UPDATE_COMMAND_UI(IDM_Play_Speed_Up, OnUpdateSpeedUp)
  ON_UPDATE_COMMAND_UI(IDM_Play_Speed_Down, OnUpdateSpeedDown)
END_MESSAGE_MAP()

const float PLAY_SPEED[] = { 0.1f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f };

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CTrainDataView::CTrainDataView():
	m_nAnimation(0)
{
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CTrainDataView::~CTrainDataView()
{
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object. */
//-------------------------------------------------------------------------------------------------
void CTrainDataView::Clear()
{
  m_bitmap.DeleteObject();
  m_bmpSize = CSize(0,0);
  m_stretchMode = HALFTONE;
  m_bPlay = false;
  m_bPause = false;
  m_sleepTime = 0;
  m_frame = -1;
  m_fps = -1.0f;
  m_speedNo = 3;
  m_tmpImage.clear();
  m_auxImage.clear();
  m_tmpBuffer.clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework to render an image of the document. */
//-------------------------------------------------------------------------------------------------
void CTrainDataView::OnDraw( CDC * pDC )
{
  CRect          clientRect;
  CTrainDataDoc * pDoc = GetDocument();

  GetClientRect( &clientRect );
  if (clientRect.IsRectEmpty())
    return;

  CMemDC dc(pDC);
  CDC* pMemDC = &dc;
  if (!(pDoc->m_avi.IsOk()) || (pDoc->m_avi.GetDIB() == 0))
  {
    pMemDC->FillSolidRect( &clientRect, RGB(230,240,250) );
    return;
  }

  if ((m_bmpSize.cx == 0) || (m_bmpSize.cy == 0))
  {
    RecalculateLayout( pDC );
  }

  if (m_bPlay) // while playing draw via temporal bitmap to prevent twinkling of drawings
  {
#if 0
    CDC memDC;                                 // temporal device context
    memDC.CreateCompatibleDC( pDC );
    int oldMode = memDC.SetStretchBltMode( m_stretchMode );
    CBitmap * pOldBitmap = memDC.SelectObject( &m_bitmap );
    memDC.IntersectClipRect( 0, 0, m_bmpSize.cx, m_bmpSize.cy );
    //memDC.FillSolidRect( 0, 0, m_bmpSize.cx, m_bmpSize.cy, pDC->GetBkColor() );
    DrawContent( &memDC );
    pDC->BitBlt( 0, 0, m_bmpSize.cx, m_bmpSize.cy, &memDC, 0, 0, SRCCOPY );
    memDC.SelectObject( pOldBitmap );
    memDC.SetStretchBltMode( oldMode );
#endif
	int oldMode = pMemDC->SetStretchBltMode( m_stretchMode );
	DrawContent( pMemDC );
	pMemDC->SetStretchBltMode( oldMode );
  }
  else // in static state draw directly, because it is convenient while editing something
  {
#if 0
    int oldMode = pDC->SetStretchBltMode( HALFTONE );
    DrawContent( pDC );
    pDC->SetStretchBltMode( oldMode );
#endif
	int oldMode = pMemDC->SetStretchBltMode( HALFTONE );
	DrawContent( pMemDC );
	pMemDC->SetStretchBltMode( oldMode );
  }
#if 0
  DrawChair(*pDC);
#endif
  DrawChair( *pMemDC );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function recalculates layout and reallocates temporal bitmap.

  \param  pDC  pointer to the compatible device context. */
//-------------------------------------------------------------------------------------------------
void CTrainDataView::RecalculateLayout( CDC * pDC )
{
  CRect          rect;
  CTrainDataDoc * pDoc = reinterpret_cast<CTrainDataDoc*>( m_pDocument );

  GetClientRect( &rect );
  if ((pDoc->m_avi.IsOk()) && (pDoc->m_avi.GetDIB() != 0) && !(rect.IsRectEmpty()))
  {
    const BITMAPINFOHEADER * pDib = pDoc->m_avi.GetDIB();

    pDoc->m_layout.recalculate( pDib->biWidth, pDib->biHeight, this->GetSafeHwnd() );
    if ((((HBITMAP)m_bitmap) == 0) || (m_bmpSize != rect.Size()))
    {
      m_bitmap.DeleteObject();
      m_bmpSize = rect.Size();
      if (m_bitmap.CreateCompatibleBitmap( pDC, m_bmpSize.cx, m_bmpSize.cy ))
      {
        rect = pDoc->m_layout.rectangle();
        m_stretchMode = ((rect.Width()  < pDib->biWidth) ||
                         (rect.Height() < pDib->biHeight)) ? HALFTONE : COLORONCOLOR;

        CDC       memDC;
        CClientDC dc( this );

        // Fill the newly created bitmap by background color.
        memDC.CreateCompatibleDC( &dc );
        CBitmap * pOldBitmap = memDC.SelectObject( &m_bitmap );
        memDC.IntersectClipRect( 0, 0, m_bmpSize.cx, m_bmpSize.cy );
        memDC.FillSolidRect( 0, 0, m_bmpSize.cx, m_bmpSize.cy, pDC->GetBkColor() );
        memDC.SelectObject( pOldBitmap );
      }
      else
      {
        m_bitmap.DeleteObject();
        m_bmpSize = CSize(0,0);
        TRACE( _T("!CreateCompatibleBitmap()\n") );
      }
    }
  }
  else
  {
    m_bitmap.DeleteObject();
    m_bmpSize = CSize(0,0);
    pDoc->m_layout.recalculate( 0, 0, GetSafeHwnd() );
    m_stretchMode = HALFTONE;
  }
}


//-------------------------------------------------------------------------------------------------
// CTrainDataView diagnostics.
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CTrainDataView::AssertValid() const
{
  CView::AssertValid();
}
void CTrainDataView::Dump( CDumpContext & dc ) const
{
  CView::Dump(dc);
}
CTrainDataDoc * CTrainDataView::GetDocument() const
{
  ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CTrainDataDoc ) ) );
  return (CTrainDataDoc*)m_pDocument;
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function after the window's size has changed. */
//-------------------------------------------------------------------------------------------------
void CTrainDataView::OnSize( UINT nType, int cx, int cy )
{
  BaseIdleView::OnSize( nType, cx, cy );
  CClientDC dc( this );
  RecalculateLayout( &dc );
  Invalidate( FALSE );
}


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework after the view's document has been modified;
           this function is called by CDocument::UpdateAllViews and allows the view
           to update its display to reflect those modifications. */
//-------------------------------------------------------------------------------------------------
void CTrainDataView::OnUpdate( CView * pSender, LPARAM lHint, CObject * pHint )
{
  pSender;lHint;pHint;

  OnStopSequence();
  CTrainDataDoc * pDoc = GetDocument();
  CRect rect = pDoc->m_layout.rectangle();
  CClientDC dc( this );
  RecalculateLayout( &dc );
  Invalidate( rect != pDoc->m_layout.rectangle() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Play commands.
///////////////////////////////////////////////////////////////////////////////////////////////////

void CTrainDataView::OnFirstFrame()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
  {
    CWaitCursor wait;
    pDoc->m_avi.GetFrame( 0 );
    Invalidate( FALSE );
    m_frame = pDoc->m_avi.GetFrameIndex();
    UpdateStatusBarInformation();
  }
}

void CTrainDataView::OnNFramesBackward()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
  {
    CWaitCursor wait;
    int n = (int)(pDoc->m_avi.GetFrameRate() * 0.5 + 0.5);
    int i = pDoc->m_avi.GetFrameIndex();
    pDoc->m_avi.GetFrame( std::max( i-n, 0 ) );
    Invalidate( FALSE );
    m_frame = pDoc->m_avi.GetFrameIndex();
    UpdateStatusBarInformation();
  }
}

void CTrainDataView::OnPrevFrame()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
  {
    CWaitCursor wait;
    int i = pDoc->m_avi.GetFrameIndex();
	if( i == 0 ) return;
    pDoc->m_avi.GetFrame( std::max( i-1, 0 ) );
    Invalidate( FALSE );
    m_frame = pDoc->m_avi.GetFrameIndex();
    UpdateStatusBarInformation();
  }
}

void CTrainDataView::OnNextFrame()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
  {
    CWaitCursor wait;
    int N = pDoc->m_avi.GetFrameNumber();
    int i = pDoc->m_avi.GetFrameIndex();
    pDoc->m_avi.GetFrame( std::min( i+1, N-1 ) );
    Invalidate( FALSE );
    m_frame = pDoc->m_avi.GetFrameIndex();
    UpdateStatusBarInformation();
  }
}

void CTrainDataView::OnNFramesForward()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
  {
    CWaitCursor wait;
    int N = pDoc->m_avi.GetFrameNumber();
    int n = (int)(pDoc->m_avi.GetFrameRate() * 0.5 + 0.5);
    int i = pDoc->m_avi.GetFrameIndex();
    pDoc->m_avi.GetFrame( std::min( i+n, N-1 ) );
    Invalidate( FALSE );
    m_frame = pDoc->m_avi.GetFrameIndex();
    UpdateStatusBarInformation();
  }
}

void CTrainDataView::OnLastFrame()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
  {
    CWaitCursor wait;
    pDoc->m_avi.GetFrame( pDoc->m_avi.GetFrameNumber()-1 );
    Invalidate( FALSE );
    m_frame = pDoc->m_avi.GetFrameIndex();
    UpdateStatusBarInformation();
  }
}

void CTrainDataView::OnPlaySequence()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
  {
    CWaitCursor wait;
    if (!m_bPlay || m_bPause)
    {
      InitPlaying();
    }
    else
    {
      m_bPause = true;
      m_frame = pDoc->m_avi.GetFrameIndex();
      pDoc->m_avi.GetFrame( m_frame );
      Invalidate( FALSE );
    }
    UpdatePlayButtonImage();
    UpdateStatusBarInformation();
    UpdateSpeedInStatusBar();
  }
}

void CTrainDataView::OnStopSequence()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
  {
    CWaitCursor wait;
    if (m_bPlay)
      DonePlaying( !(pDoc->m_avi.IsEnd()) );
  }
}

void CTrainDataView::OnSpeedUp()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
  {
    m_speedNo++;
    m_speedNo = ALIB_LIMIT( m_speedNo, 0, (ALIB_LengOf(PLAY_SPEED)-1) );
    UpdateSpeedInStatusBar();
  }
}

void CTrainDataView::OnSpeedDown()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
  {
    m_speedNo--;
    m_speedNo = ALIB_LIMIT( m_speedNo, 0, (ALIB_LengOf(PLAY_SPEED)-1) );
    UpdateSpeedInStatusBar();
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Update play commands.
///////////////////////////////////////////////////////////////////////////////////////////////////

void CTrainDataView::OnUpdateFirstFrame( CCmdUI * pCmdUI )
{
  pCmdUI->Enable( !m_bPlay || m_bPause );
}

void CTrainDataView::OnUpdateNFramesBackward( CCmdUI * pCmdUI )
{
  pCmdUI->Enable( !m_bPlay || m_bPause );
}

void CTrainDataView::OnUpdatePrevFrame( CCmdUI * pCmdUI )
{
  pCmdUI->Enable( !m_bPlay || m_bPause );
}

void CTrainDataView::OnUpdateNextFrame( CCmdUI * pCmdUI )
{
  pCmdUI->Enable( !m_bPlay || m_bPause );
}

void CTrainDataView::OnUpdateNFramesForward( CCmdUI * pCmdUI )
{
  pCmdUI->Enable( !m_bPlay || m_bPause );
}

void CTrainDataView::OnUpdateLastFrame( CCmdUI * pCmdUI )
{
  pCmdUI->Enable( !m_bPlay || m_bPause );
}

void CTrainDataView::OnUpdatePlaySequence( CCmdUI * pCmdUI )
{
  CTrainDataDoc * pDoc = GetDocument();
  pCmdUI->Enable( (pDoc != 0) && !(pDoc->m_avi.IsEnd()) );
}

void CTrainDataView::OnUpdateStopSequence( CCmdUI * pCmdUI )
{
  pCmdUI->Enable( m_bPlay == true ); 
}

void CTrainDataView::OnUpdateSpeedUp( CCmdUI * pCmdUI )
{
  pCmdUI->Enable( m_speedNo < (ALIB_LengOf(PLAY_SPEED)-1) );
}

void CTrainDataView::OnUpdateSpeedDown( CCmdUI * pCmdUI )
{
  pCmdUI->Enable(  m_speedNo > 0 );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function is called when the frame index in the status-bar must be updated. */
//-------------------------------------------------------------------------------------------------
void CTrainDataView::UpdateStatusBarInformation()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && (pDoc->m_pFrameWin != 0))
  {
    pDoc->m_pFrameWin->SetFrameIndexIndicator( m_frame );
    pDoc->m_pFrameWin->SetFrameRateIndicator( m_fps );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function is called when the speed indicator in the status-bar must be updated. */
//-------------------------------------------------------------------------------------------------
void CTrainDataView::UpdateSpeedInStatusBar()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && (pDoc->m_pFrameWin != 0))
    pDoc->m_pFrameWin->SetSpeedIndicator( PLAY_SPEED[ m_speedNo ] );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function updates play-button's image between pause/play actions. */
//-------------------------------------------------------------------------------------------------
void CTrainDataView::UpdatePlayButtonImage()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && (pDoc->m_pFrameWin != 0))
    pDoc->m_pFrameWin->SetPlayButtonImage( !m_bPause );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes sequence processing starting from the selected frame.

  \param  startFrame  the index of the frame to start from. */
//-------------------------------------------------------------------------------------------------
void CTrainDataView::InitPlayingFromSelectedFrame( int startFrame )
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
  {
    if (!m_bPause)
    {
      startFrame = alib::Limit( startFrame, 0, (int)(pDoc->m_avi.GetFrameNumber()-1) );
      // m_method->Init();
      pDoc->m_avi.GetFrame( startFrame );
    }
    m_bPlay = true;
    m_bPause = false;
    m_sleepTime = 0;
    m_frame = pDoc->m_avi.GetFrameIndex();
    m_fps = -1.0;
    UpdateStatusBarInformation();
    UpdateSpeedInStatusBar();
    if (pDoc->m_pFrameWin != 0)
      pDoc->m_pFrameWin->EnableFrameScroller( false );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function is called before beginning of sequence playing. */
//-------------------------------------------------------------------------------------------------
void CTrainDataView::InitPlaying()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
    InitPlayingFromSelectedFrame( pDoc->m_avi.GetFrameIndex() );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function processes the current frame.

  \return  proceed tracking = true, otherwise stop tracking. */
//-------------------------------------------------------------------------------------------------
bool CTrainDataView::ProceedPlaying()
{
  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc == 0) || !(pDoc->m_avi.IsOk())) //|| !(m_method->Process( pDib, pDoc->m_avi.GetFrameIndex() ))
  {
    DonePlaying( true );
    return false;
  }
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function is called after stopping of sequence playing.

  \param  bInterrupted  true if sequence playing has been interrupted by user. */
//-------------------------------------------------------------------------------------------------
void CTrainDataView::DonePlaying( bool bInterrupted )
{
  CTrainDataDoc * pDoc = GetDocument();
  if (m_bPlay)
  {
    bInterrupted;
    // m_method->Done( bInterrupted );
    // if ((GetParameters()).demo.logging())
    //   StoreLogFile();
  }

  if (pDoc != 0)
  {
    int f = pDoc->m_avi.IsEnd() ? pDoc->m_avi.GetFrameNumber() : pDoc->m_avi.GetFrameIndex();
    pDoc->m_avi.GetFrame( std::max<int>( f-1, 0 ) );
  }

  m_bPlay = false;
  m_bPause = false;
  m_sleepTime = 0;
  m_frame = (pDoc != 0) ? pDoc->m_avi.GetFrameIndex() : -1;
  m_fps = -1.0;
  Invalidate( FALSE );
  UpdateStatusBarInformation();
  UpdateSpeedInStatusBar();
  UpdatePlayButtonImage();
  if ((pDoc != 0) && (pDoc->m_pFrameWin != 0))
    pDoc->m_pFrameWin->EnableFrameScroller( true );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function paints DIB image of the current frame.

  \param  pDC  the underlying device context. */
//-------------------------------------------------------------------------------------------------
void CTrainDataView::DrawContent( CDC * pDC )
{
  bool bInvertY = true;

  CTrainDataDoc * pDoc = GetDocument();
  if ((pDoc != 0) && pDoc->m_avi.IsOk())
  {
	BITMAPINFO				trueBi;
    BITMAPINFOHEADER&         bi = trueBi.bmiHeader;
    const BITMAPINFOHEADER * pDib, *pOriginalDib; pDib = pOriginalDib = pDoc->m_avi.GetDIB();
	const BYTE             * pData, *pOriginalData; pData = pOriginalData = pDoc->m_avi.GetData();
    RECT                     rect = pDoc->m_layout.rectangle();
    bool                     ok = false;
    UByteArr               & scan = m_tmpBuffer;

    if ((pDib != 0) && ((pDib->biCompression == BI_RGB) || (pDib->biCompression == BI_BITFIELDS)))
    {
#if 0
      pData = reinterpret_cast<const char*>( pDib+1 );
      switch (pDib->biBitCount)
      {
        case 1: pData +=   2*sizeof(RGBQUAD);  break;
        case 4: pData +=  16*sizeof(RGBQUAD);  break;
        case 8: pData += 256*sizeof(RGBQUAD);  break;
      }
#endif

      // Correct brightness.
      try
      { 
        CopyMemoryWithOffset( &bi, (const BYTE*&)pDib, sizeof(bi) ) ;

        ALIB_ASSERT((bi.biCompression == BI_RGB) && ((bi.biBitCount >= 24) || (bi.biBitCount == 8)));

        if (bi.biBitCount >= 24)                     // true-color bitmap
        {
          int nByte = (bi.biBitCount == 32) ? 4 : 3; // bytes per pixel

          scan.resize( ((nByte*bi.biWidth + 3) & ~3), 0 );
          m_auxImage.resize( bi.biWidth, bi.biHeight );

          // Read file scan by scan.
          for(int y = 0; y < bi.biHeight; y++)
          {
            unsigned char * pDst = m_auxImage.row_begin( bInvertY ? (bi.biHeight-1-y) : y );
            unsigned char * pSrc = &(*(scan.begin()));

            CopyMemoryWithOffset( &(*(scan.begin())), pData, (int)(scan.size()*sizeof(unsigned char))) ;

            for(int k = 0; k < bi.biWidth; k++)
            {
              unsigned int b = pSrc[0];
              unsigned int g = pSrc[1];
              unsigned int r = pSrc[2];
              (*pDst) = (unsigned char)((r*306 + g*601 + b*117 + 512) >> 10);
              pDst++;
              pSrc += nByte;
            }
          }
        }
        else if (bi.biBitCount == 8)                 // 8-bits bitmap
        {
          RGBQUAD palette[256];

          scan.resize( ((bi.biWidth + 3) & ~3), 0 );
          m_auxImage.resize( bi.biWidth, bi.biHeight );
          memset( palette, 0, sizeof(palette) );

          CopyMemoryWithOffset( palette, (const BYTE*&)pDib, sizeof(palette) )  ;

          // Read file scan by scan.
          for (int y = 0; y < bi.biHeight; y++)
          {
            unsigned char * pDst = m_auxImage.row_begin( bInvertY ? (bi.biHeight-1-y) : y );
            unsigned char * pSrc = &(*(scan.begin()));

            CopyMemoryWithOffset( &(*(scan.begin())), pData, (int)(scan.size()*sizeof(unsigned char)) ) ;

            for (int k = 0; k < bi.biWidth; k++)
            {
              RGBQUAD &    color = palette[ (unsigned int)(*pSrc) ];
              unsigned int b = color.rgbBlue;
              unsigned int g = color.rgbGreen;
              unsigned int r = color.rgbRed;

              (*pDst) = (unsigned char)((r*306 + g*601 + b*117 + 512) >> 10);
              pDst++;
              pSrc++;
            }
          }
        }

        if (csutility::CorrectByteBrightness( &m_auxImage, 70, &m_tmpImage, &m_tmpBuffer ))
        {
          m_auxImage.swap( m_tmpImage );
          alib::RepeatedlySmoothImage( m_auxImage, 2 );
          ok = true;
        }
      }
      catch (std::exception &) {}

      if (ok)
      {
        csutility::DrawGrayImage( &m_auxImage, pDC->GetSafeHdc(), rect, bInvertY, false );
      }
      else
      {
        ::StretchDIBits( pDC->GetSafeHdc(),
                         rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
                         0, 0, pOriginalDib->biWidth, pOriginalDib->biHeight,
                         reinterpret_cast<const void*>( pOriginalData ),
                         reinterpret_cast<const BITMAPINFO*>(& trueBi ),
                         DIB_RGB_COLORS, SRCCOPY );
      }
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function is called during the idle cycle to process the current frame.

  \return  To continue the idle cycle - 'true', otherwise - 'false'. */
//-------------------------------------------------------------------------------------------------
bool CTrainDataView::OnIdle()
{
  static DWORD   lastTime = ::GetTickCount();
  CTrainDataDoc * pDoc = GetDocument();

  // Is the sequence can be played?
  if ((pDoc == 0) || !(pDoc->m_avi.IsOk()) || !m_bPlay || m_bPause) // || pDoc->m_avi.IsEnd())
    return false;
//  if (m_method.get() == 0)
//    return false;

  // Get initial time-stamp.
  DWORD startTime = ::GetTickCount();

  // Do we need to sleep a bit in order to fit the frame rate? If yes, then skip further processing.
  // Always check (startTime >= lastTime), because the time counter overflows at some moments.
  if ((m_sleepTime > 0) && (startTime >= lastTime))
  {
    if (startTime < (m_sleepTime + lastTime))
      return true; // continue the idle-cycle
  }
  m_sleepTime = 0;

  // Do the useful work here using the current frame.
  m_frame = pDoc->m_avi.GetFrameIndex();
  if (!ProceedPlaying())
    return false;

  // Draw current results.
  CClientDC dc( this );
  OnDraw( &dc );

  // Load the next frame.
  if (!(pDoc->m_avi.GetNextFrame()))
  {
    OnStopSequence();
    return false;
  }

  // Update the last time-stamp.
  lastTime = ::GetTickCount();

  // Estimate the time we need to sleep.
  float speed = PLAY_SPEED[ ALIB_LIMIT( m_speedNo, 0, ALIB_LengOf(PLAY_SPEED)-1 ) ];
  long  duration = (long)(std::max( lastTime, startTime ) - startTime);
  long  frameDuration = (long)( pDoc->m_avi.GetFrameDuration()/speed + 0.5 );
  m_sleepTime = std::max( duration, frameDuration ) - duration;

  // Compute frames per second value taking into account the possible sleep-time interval.
  m_fps = (float)( 1000.0/(FLT_EPSILON + std::max( duration, frameDuration )) );

  // Update status-bar information.
  UpdateStatusBarInformation();

  // Continue the idle-cycle.
  return true;
}

const int c_nHuman	= 0;
const int c_nClothes= 1;
const int c_nUnknown= 2;
const int c_nEmpty	= 3;

void CTrainDataView::OnInitialUpdate()
{
	SetScrollSizes(MM_TEXT, CSize(1,1));
	CTrainDataDoc* pDoc = GetDocument();

	
	if( !m_ilContent.GetSafeHandle() )
	{
		const int c_ImageSize = 16;
		m_ilContent.Create( c_ImageSize, c_ImageSize, ILC_COLOR24|ILC_MASK, 3, 1 ); //IDB_CHAIR_CONTENT RGB(192,192,192)
		CBitmap bmp;
		bmp.LoadBitmap( IDB_CHAIR_CONTENT );
		m_ilContent.Add( &bmp, RGB(192,192,192) );

		pDoc->m_dlgMove.Create( COffsetChairDialog::IDD, this );
	}
}

const COLORREF ANIMATED_CURVE_COLOR = RGB(0,128,255);
const COLORREF SELECTED_ANIMATED_CURVE_COLOR = RGB(255,0,0);

void	CTrainDataView::DrawChair(CDC& dc)
{
	CTrainDataDoc* pDoc = GetDocument();

	CPen penWhite( PS_SOLID, 2, RGB(255,255,255) );
	CPen penOver( PS_DASH, 2, ANIMATED_CURVE_COLOR);
	CPen penSelect( PS_DASH, 2, SELECTED_ANIMATED_CURVE_COLOR);
	CPen penSelectChair( PS_DASH, 2, RGB(0, 255, 0) );
	CGdiObject* pOldPen = dc.SelectObject( &penWhite );

	CPen penOverCh( PS_DASH, 1, ANIMATED_CURVE_COLOR);
	CPen penSelectCh( PS_DASH, 1, SELECTED_ANIMATED_CURVE_COLOR);
	CPen penWhiteCh( PS_SOLID, 1, RGB(255,255,255) );

	CSize sizeOffset( GetScrollPos( SB_HORZ ), GetScrollPos( SB_VERT ) );

		MyChair::ChairSet::const_iterator crvIt;
		int nOff = 0;
		  for (crvIt = pDoc->m_ChairSet.begin(); 
				crvIt != pDoc->m_ChairSet.end(); ++crvIt, ++nOff)
		  {
					// FIXME only for zone 0
			if(crvIt->subFrameNo != pDoc->GetCurrentCameraNo() ) continue;
			const Vec2fArr & curve = (crvIt)->curve;
			int             k, 
							nPoint = (int)(curve.size()); 
			for( k=0; k < nPoint; ++k /*, ++iter*/ )
			{
				int newk  = (m_nAnimation + k)%nPoint;
				int nextk = (newk + 1)%nPoint;

				const Vec2f& v1 = curve[newk];
				const Vec2f& v2 = curve[nextk];
				CPoint pt1, pt2;
				pDoc->m_layout.p2s( v1, pt1 );
				pDoc->m_layout.p2s( v2, pt2 );



				// NOTE : if( newk ... ) - static curve 
				if( ( k % 8)  < 4 )
					dc.SelectObject( crvIt == pDoc->m_itCurrentChair ? &penSelectCh : &penOverCh  );
				else
					dc.SelectObject( &penWhiteCh  );
				dc.MoveTo( pt1 );
				dc.LineTo( pt2 );
			}
			CPoint ptCenter;
			Vec2i Center = crvIt->center;
			

			Range::CONTENT cnt = pDoc->GetCurrentContent( crvIt->id );
			int nImageId = -1;
			switch ( cnt )
			{
			case Range::empty : nImageId = c_nEmpty; break;
			case Range::unknown : nImageId = c_nUnknown; break;
			case Range::human : nImageId = c_nHuman; break;
			case Range::clothes : nImageId = c_nClothes; break;
			}
			if( nImageId >= 0)
			{
				IMAGEINFO ii;
				m_ilContent.GetImageInfo( 0, &ii );
				pDoc->m_layout.p2s( CPoint( Center.x, Center.y ), ptCenter );
				CRect rc(ii.rcImage);
				//CSize sizeImage = rc.Size();
				CSize sizeImage(16,16);
				ptCenter.Offset( - sizeImage.cx/2, - sizeImage.cy/2 );
				m_ilContent.DrawEx( &dc, nImageId, ptCenter, sizeImage, CLR_NONE, CLR_NONE, ILD_NORMAL );
			}
		  }
//	m_nAnimation = (m_nAnimation +1 )%8;	
	dc.SelectObject( pOldPen );
}

void CTrainDataView::OnLButtonDown( UINT nFlags, CPoint point )
{
	CTrainDataDoc* pDoc = GetDocument();
	ASSERT(pDoc);
	pDoc->OnLButtonDownHandler( nFlags, point );

	BaseIdleView::OnLButtonDown( nFlags, point );
}

BOOL CTrainDataView::OnEraseBkgnd( CDC* pDC )
{
	UNREFERENCED_PARAMETER(pDC);
	return TRUE;
}

