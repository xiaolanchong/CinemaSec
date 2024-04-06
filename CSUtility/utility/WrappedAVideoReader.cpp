///////////////////////////////////////////////////////////////////////////////
// WrappedAVideoReader.cpp
// ---------------------
// begin     : Jul 2005
// modified  : 1 Aug 2005
// author(s) : Albert Akhriev.
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "base_frame_grabber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csutility
{

//=================================================================================================
/** \class WrappedAVideoReader.
    \brief Wrapped version of avideolib::AVideoReadGrabber. */
//=================================================================================================
struct WrappedAVideoReader : public BaseFrameGrabberEx
{
  avideolib::IVideoReader * m_pGrabber;
  bool                      m_bEnd;
  Arr2ub                    m_frame;
  ubyte                     m_buffer[sizeof(BITMAPINFOHEADER)+(1<<8)*sizeof(RGBQUAD)];
  CCriticalSection          m_locker;
  __int32                   m_version;
  __int32                   m_width;
  __int32                   m_height;
  __int32                   m_frameNum;
  __int32                   m_frameNo;

  WrappedAVideoReader()
  {
    m_pGrabber = 0;
    m_bEnd = false;
    m_frame;
    memset( m_buffer, 0, sizeof(m_buffer) );
    m_locker;
    m_version = 0;
    m_width = 0;
    m_height = 0;
    m_frameNum = 0;
    m_frameNo = 0;
  }

  ~WrappedAVideoReader()
  {
    delete m_pGrabber;
  }

  virtual bool Initialize( LPCWSTR name, alib::IMessage * pOutput )
  {
    BITMAPINFO * p = reinterpret_cast<BITMAPINFO*>( m_buffer );
    for (int i = 0; i < (1<<8); i++)
      p->bmiColors[i].rgbBlue = (p->bmiColors[i].rgbGreen = (p->bmiColors[i].rgbRed = (ubyte)i));

    m_pGrabber = avideolib::CreateAVideoReader( name, pOutput, DOWNWARD_AXIS_Y );
    m_bEnd = false;
    m_version = 0;
    m_width = 0;
    m_height = 0;
    m_frameNum = 0;
    m_frameNo = 0;
    if (m_pGrabber != 0)
    {
      alib::TStateFlag flag;
      flag.data = avideolib::IBaseVideo::VERSION;
      if (m_pGrabber->GetData( &flag )) m_version = flag.data;
      flag.data = avideolib::IBaseVideo::WIDTH;
      if (m_pGrabber->GetData( &flag )) m_width = flag.data;
      flag.data = avideolib::IBaseVideo::HEIGHT;
      if (m_pGrabber->GetData( &flag )) m_height = flag.data;
      flag.data = avideolib::IBaseVideo::FRAME_NUMBER;
      if (m_pGrabber->GetData( &flag )) m_frameNum = flag.data;
    }
    return (m_pGrabber != 0);
  }

  virtual bool Start( LPCTSTR, const FrameGrabberParameters & )
  {
    m_frameNo = 0;
    return (m_pGrabber != 0);
  }

  virtual bool Stop()
  {
    return (m_pGrabber != 0);
  }

  virtual bool IsOk() const
  {
    return (m_pGrabber != 0);
  }

  virtual bool IsEnd() const
  {
    return m_bEnd;
  }

  virtual bool HasFreshFrame() const
  {
    return true;
  }

  virtual bool LockLatestFrame( LPCBITMAPINFO & pHeader, LPCUBYTE & pImage, int & imgSize )
  {
    CSingleLock lock( &m_locker, TRUE );

    if (m_pGrabber == 0)
      return false;

    switch (m_pGrabber->ReadFrame( &m_frame, -1, &m_frameNo, 0 ))
    {
      case avideolib::IVideoReader::NORMAL        : break;
      case avideolib::IVideoReader::NO_FRESH_FRAME:
      case avideolib::IVideoReader::END_IS_REACHED: m_bEnd = true;  break;
      case avideolib::IVideoReader::GENERAL_ERROR : return false;
    }

    BITMAPINFO * pbi = reinterpret_cast<BITMAPINFO*>( m_buffer );
    pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbi->bmiHeader.biWidth = m_frame.width();
    pbi->bmiHeader.biHeight = DOWNWARD_AXIS_Y ? -(m_frame.height()) : +(m_frame.height());
    pbi->bmiHeader.biPlanes = 1;
    pbi->bmiHeader.biBitCount = 8;
    pbi->bmiHeader.biCompression = 0;
    pbi->bmiHeader.biSizeImage = m_frame.size() * sizeof(Arr2ub::value_type);
    pHeader = pbi;
    pImage = m_frame.begin();
    imgSize = pbi->bmiHeader.biSizeImage;
    return true;
  }

  virtual bool UnlockProcessedFrame()
  {
    CSingleLock lock( &m_locker, TRUE );
    return true;
  }

  virtual DWORD GetVersion()
  {
    return (DWORD)m_version;
  }

  virtual DWORD GetSize( DWORD & x, DWORD & y )
  {
    x = (DWORD)m_width;
    y = (DWORD)m_height;
    return (((m_width > 0) && (m_height > 0)) ? 0 : 1);
  }

  virtual DWORD GetPos( float & pos )
  {
    pos = (float)((double)m_frameNo/(double)(std::max<__int32>( m_frameNum-1, 1 )));
    return ((m_frameNum > 0) ? 0 : 1);
  }
};


//-------------------------------------------------------------------------------------------------
/** \brief Function creates an instance of wrapped reading video frame-grabber.

  \param  name     file name.
  \param  pOutput  pointer to the message handler.
  \return          Ok = pointer to the newly created instance, otherwise 0. */
//-------------------------------------------------------------------------------------------------
BaseFrameGrabberEx * CreateWrappedAVideoReader( LPCWSTR name, alib::IMessage * pOutput )
{
  WrappedAVideoReader * p = new WrappedAVideoReader();

  if (p != 0)
  {
    if (p->Initialize( name, pOutput ))
      return p;
    else
      delete p;
  }

  if (pOutput != 0)
    pOutput->PrintW( alib::IMessage::mt_error,
                     _T("Failed to create wrapped reading video frame-grabber") );
  return 0;
}

} // namespace csutility

