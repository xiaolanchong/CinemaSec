/********************************************************************************
  view.cpp
  ---------------------
  begin     : May 2003
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  e-mail    : aaah@mail.ru, aaahaaah@hotmail.com
********************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "view.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CClientView, CWnd)
  ON_WM_PAINT()
  ON_WM_SIZE()
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CClientView::CClientView()
{
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CClientView::~CClientView()
{
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object. */
//-------------------------------------------------------------------------------------------------
void CClientView::Clear()
{
  m_output;
  m_reader.reset();
  m_writer.reset();
  m_image.clear();
  m_layout.clear();
  m_file.clear();
  m_grabberId = GRABBER_UNKNOWN;
  m_videoInfo.clear();
  m_colorScheme = avideolib::IBaseVideo::GRAYSCALED;
  m_width = 0;
  m_height = 0;
  m_frameNum = 0;
  m_frameNo = 0;
  m_frameTime = 0;
  m_bPlay = false;
  m_sleepTime = 0;
  m_fps = -1.0f;
  m_speedNo = ALIB_LengOf(PLAY_SPEED)/2;
  m_headFrame = 0;
  m_lastFrame = 0;
  m_writeFPS = 1.0f;
}


//-------------------------------------------------------------------------------------------------
/** \brief Called before the creation of the Windows window attached to this CWnd object. */
//-------------------------------------------------------------------------------------------------
BOOL CClientView::PreCreateWindow( CREATESTRUCT & cs ) 
{
  if (!CWnd::PreCreateWindow( cs ))
    return FALSE;

  cs.dwExStyle |= WS_EX_CLIENTEDGE;
  cs.style &= ~WS_BORDER;
  cs.lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
                                      ::LoadCursor( NULL, IDC_ARROW ),
                                      reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL );
  return TRUE;
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when Windows or an application makes
           a request to repaint a portion of an application's window. */
//-------------------------------------------------------------------------------------------------
void CClientView::OnPaint() 
{
  CPaintDC dc( this );
  if (!(m_layout.ready( &m_image )))
    m_layout.recalculate( m_image.width(), m_image.height(), GetSafeHwnd() );
  DrawColorImage< AImage, DOWNWARD_AXIS_Y >( &m_image, dc.GetSafeHdc(), m_layout.rectangle() );
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function after the window's size has changed. */
//-------------------------------------------------------------------------------------------------
void CClientView::OnSize( UINT nType, int cx, int cy )
{
  CWnd::OnSize( nType, cx, cy );
  m_layout.clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function creates appropriate frame grabber. */
//-------------------------------------------------------------------------------------------------
bool CClientView::CreateFrameGrabber()
{
  CWaitCursor wait;

  m_reader.reset();
  if (m_grabberId == GRABBER_BITMAP)
  {
    m_reader.reset( avideolib::CreateBitmapFrameGrabber( m_file.c_str(), &m_output ) );
  }
  else if (m_grabberId == GRABBER_AVIDEO)
  {
    m_reader.reset( avideolib::CreateAVideoReader( m_file.c_str(), &m_output, avideolib::INVERT_AXIS_Y ) );
  }
  else m_grabberId = GRABBER_UNKNOWN;

  if (m_reader.get() == 0)
    Clear();
  return (m_grabberId != GRABBER_UNKNOWN);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function gets information about currently opened videofile. */
//-------------------------------------------------------------------------------------------------
bool CClientView::GetVideoInfo()
{
  alib::TStateFlag flag;
  TCHAR            ver[5];
  StdStrStream     s;
  __int32          version = 0;

  m_videoInfo.clear();
  if (m_reader.get() != 0)
  {
    flag.data = avideolib::IBaseVideo::VERSION;
    if (m_reader->GetData( &flag ))
    {
      version = flag.data;
      flag.data = avideolib::IBaseVideo::WIDTH;
      if (m_reader->GetData( &flag ))
      {
        m_width = flag.data;
        flag.data = avideolib::IBaseVideo::HEIGHT;
        if (m_reader->GetData( &flag ))
        {
          m_height = flag.data;
          flag.data = avideolib::IBaseVideo::FRAME_NUMBER;
          if (m_reader->GetData( &flag ))
          {
            m_frameNum = flag.data;
            flag.data = avideolib::IBaseVideo::COLORSCHEME;
            if (m_reader->GetData( &flag ))
            {
              m_colorScheme = flag.data;
              flag.data = avideolib::IBaseVideo::FPS_1000;
              if (m_reader->GetData( &flag ))
              {
                m_fps = (float)(0.001*flag.data);

                // Print file info.
                {
                  s << _T("Version\t\t") << (alib::FourCC2Str( version, ver )) << std::endl;
                  s << _T("Frame width\t") << m_width << std::endl;
                  s << _T("Frame height\t") << m_height << std::endl;
                  s << _T("Frame number\t") << m_frameNum << std::endl;
                  s << _T("Color scheme\t")
                    << ((m_colorScheme == avideolib::IBaseVideo::TRUECOLOR)  ? _T("true-color") :
                       ((m_colorScheme == avideolib::IBaseVideo::GRAYSCALED) ? _T("grayscaled") :
                       _T("gray + 16 colors"))) << std::endl;
                  s << _T("FPS rate\t\t") << m_fps << std::endl;
                  m_videoInfo = s.str();
                }
                return true;
              }
            }
          }
        }
      }
    }
  }
  return false;
}


