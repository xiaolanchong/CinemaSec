/****************************************************************************
  layout.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

//=================================================================================================
/** \class OneImageLayout.
    \brief Object defines a single-image layout. */
//=================================================================================================
class /*CS_UTIL_API*/ OneImageLayout
{
  enum { GAP = 5 };          //!< the gap between image and window's frame

private:
  bool             m_bReady; //!< true when layout is valid
  int              m_imgW;   //!< the actual image width
  int              m_imgH;   //!< the actual image height
  CRect            m_rect;   //!< image rectangle in screen coordinates
  CCriticalSection m_locker; //!< the object ensures that the layout will be recalculated coherently

public:
  bool m_bKeepAspectRatio;   //!< flag enforces identical horizontal and vertical scales

public:

//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object. */
//-------------------------------------------------------------------------------------------------
void clear()
{
  m_locker.Lock();
  m_bReady = false;
  m_imgW = 0;
  m_imgH = 0;
  m_rect.SetRectEmpty();
  m_locker.Unlock();
}

//-------------------------------------------------------------------------------------------------
/** \brief Function recalculates this layout given image sizes and a window.

  \param  imageWidth   the actual width of the underlaying image.
  \param  imageHeight  the actual height of the underlaying image.
  \param  hwnd         the window for painting. */
//-------------------------------------------------------------------------------------------------
void recalculate( int imageWidth, int imageHeight, HWND hwnd )
{
  clear();
  m_locker.Lock();
  if ((imageWidth > 0) && (imageHeight > 0) && (hwnd != 0))
  {
    RECT clientRect;
    ASSERT( ::IsWindow( hwnd ) );
    ::GetClientRect( hwnd, &clientRect );

    double scaleX = (double)(abs( clientRect.right-clientRect.left ) - 2*GAP) / (double)imageWidth;
    double scaleY = (double)(abs( clientRect.bottom-clientRect.top ) - 2*GAP) / (double)imageHeight;

    if (m_bKeepAspectRatio)
      scaleY = (scaleX = std::min( scaleX, scaleY ));

    int W = (int)(scaleX * imageWidth  + 0.5);
    int H = (int)(scaleY * imageHeight + 0.5);

    if ((W > 1) && (H > 1))
    {
      int gapX = (abs( clientRect.right-clientRect.left ) - W) / 2;
      int gapY = (abs( clientRect.bottom-clientRect.top ) - H) / 2;

      m_rect.SetRect( gapX, gapY, gapX+W, gapY+H );
      m_imgW = imageWidth;
      m_imgH = imageHeight;
      m_bReady = true;
    }
  }
  m_locker.Unlock();
}

//-------------------------------------------------------------------------------------------------
/** \brief Function returns bounding rectangle of this layout.

  \return  bounding rectangle of this layout. */
//-------------------------------------------------------------------------------------------------
const CRect & rectangle()
{
  return m_rect;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function converts an image point to a screen pixel.

  \param  point   an image point.
  \param  screen  out: the destination screen point.
  \return         the point mapped to the screen coordinates. */
//-------------------------------------------------------------------------------------------------
void p2s( const POINT & point, POINT & screen )
{
  if (m_bReady)
  {
    int y = (DOWNWARD_AXIS_Y) ? point.y : (m_imgH-1-point.y);
    screen.x = m_rect.left + ((2*point.x+1) * (m_rect.right - m_rect.left)) / (2*m_imgW);
    screen.y = m_rect.top + ((2*y+1) * (m_rect.bottom - m_rect.top)) / (2*m_imgH);
  }
  else screen.x = (screen.y = 0);
}

//-------------------------------------------------------------------------------------------------
/** \brief Function converts an image point to a screen pixel.

  \param  point   an image point.
  \param  screen  out: the destination screen point.
  \return         the point mapped to the screen coordinates. */
//-------------------------------------------------------------------------------------------------
void p2s( const Vec2f & point, POINT & screen )
{
  if (m_bReady)
  {
    float y = (DOWNWARD_AXIS_Y) ? point.y : (float)(m_imgH-1.0f-point.y);
    screen.x = alib::Round( m_rect.left + (point.x * (m_rect.right - m_rect.left)) / (double)m_imgW );
    screen.y = alib::Round( m_rect.top + (y * (m_rect.bottom - m_rect.top)) / (double)m_imgH );
  }
  else screen.x = (screen.y = 0);
}

//-------------------------------------------------------------------------------------------------
/** \brief Function converts a screen pixel to an image point.

  \param  screen  a screen pixel.
  \param  point   out: the destination image point.
  \return         the pixel mapped to the image coordinates. */
//-------------------------------------------------------------------------------------------------
void s2p( const POINT & screen, POINT & point )
{
  if (m_bReady)
  {
    point.x = ((2*(screen.x - m_rect.left)+1) * m_imgW) / (2*(m_rect.right - m_rect.left));
    point.y = ((2*(screen.y - m_rect.top)+1) * m_imgH) / (2*(m_rect.bottom - m_rect.top));
    if (!DOWNWARD_AXIS_Y) point.y = m_imgH-1-point.y;
  }
  else point.x = (point.y = 0);
}

//-------------------------------------------------------------------------------------------------
/** \brief Function converts a screen pixel to a point in image coordinates.

  \param  screen  a screen pixel.
  \param  point   out: the destination image point.
  \return         the pixel mapped to the image coordinates. */
//-------------------------------------------------------------------------------------------------
void s2p( const POINT & screen, Vec2f & point )
{
  if (m_bReady)
  {
    point.x = (float)(((screen.x - m_rect.left) * m_imgW) / (double)(m_rect.right - m_rect.left));
    point.y = (float)(((screen.y - m_rect.top) * m_imgH) / (double)(m_rect.bottom - m_rect.top));
    if (!DOWNWARD_AXIS_Y) point.y = (float)(m_imgH-1.0f-point.y);
  }
  else point.x = (point.y = 0);
}

//-------------------------------------------------------------------------------------------------
/** \brief Function returns horizontal size of a pixel in screen coordinates. */
//-------------------------------------------------------------------------------------------------
int pixel_dx() const
{
  return (m_bReady ? (std::max<int>( 1, (m_rect.right-m_rect.left)/m_imgW )) : 1);
}

//-------------------------------------------------------------------------------------------------
/** \brief Function returns vertical size of a pixel in screen coordinates. */
//-------------------------------------------------------------------------------------------------
int pixel_dy() const
{
  return (m_bReady ? (std::max<int>( 1, (m_rect.bottom-m_rect.top)/m_imgH )) : 1);
}

//-------------------------------------------------------------------------------------------------
/** \brief Function returns non-zero if the layout is valid. */
//-------------------------------------------------------------------------------------------------
bool ready() const
{
  return m_bReady;
}

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
OneImageLayout() : m_bKeepAspectRatio( true )
{
  clear();
}

};

