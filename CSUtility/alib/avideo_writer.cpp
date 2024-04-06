///////////////////////////////////////////////////////////////////////////////
//  avideo_writer.cpp
//  ---------------------
//  begin     : Apr 2005
//  modified  : 24 Jul 2005
//  author(s) : Albert Akhriev
//  copyright : Albert Akhriev
//  email     : aaah@mail.ru, Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "avideo_header.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace avideolib
{

//=================================================================================================
/** \class AVideoWriter.
    \brief Class sequentially writes images into a video file. */
//=================================================================================================
class AVideoWriter : public avideolib::IVideoWriter
{
private:
  VideoHeader      m_videoHeader; //!< header of video file
  FrameHeader      m_frameHeader; //!< header of each frame in a file
  bool             m_bGood;       //!< state of processing
  bool             m_bInvertY;    //!< if nonzero, then axis Y will be inverted
  alib::IMessage * m_pOutput;     //!< pointer to a message handler
  alib::BinaryFile m_file;        //!< file wrapper
  UByteArr         m_scan;        //!< temporal scan buffer
  int              m_count;       //!< frame counter
  unsigned __int32 m_startTime;   //!< the time of the first frame
  CCriticalSection m_locker;      //!< data protector in multi-threaded application

protected:
virtual void Clear()
{
  m_videoHeader = VideoHeader();
  m_frameHeader = FrameHeader();
  m_bGood = false;
  m_bInvertY = false;
  m_pOutput;
  m_file;
  m_scan.clear();
  m_count = 0;
  m_startTime = 0;
  m_locker;
}


virtual bool Finalize()
{
  try
  {
    ALIB_ASSERT( m_bGood );
    m_videoHeader.frameNum = m_count;
    m_videoHeader.fps1000 = (m_frameHeader.time > 0)
      ? (__int32)(floor( (1000.0*1000.0*std::max( m_count-1, 0 )) / m_frameHeader.time ) + 0.5) : 0;
    m_file.seek( 0L, SEEK_SET );
    m_file.write( &m_videoHeader, sizeof(VideoHeader) );  // write correct file header
  }
  catch (alib::GeneralError & e)
  {
    m_bGood = false;
    PrintErrorMessage( e );
  }
  return m_bGood;
}


__int32 GetTime( unsigned __int32 milliTime )
{
  if (m_count == 0)
    m_startTime = milliTime;

  __int64 t = (__int64(1) << 32);
  return (__int32)((t += (__int64)milliTime) -= (__int64)m_startTime);
}


void PrintErrorMessage( alib::GeneralError & e ) const
{
  if (m_pOutput != 0)
    m_pOutput->PrintA( alib::IMessage::mt_error, e.what() );
}


public:
AVideoWriter( alib::IMessage * pOutput )
{
  m_pOutput = pOutput;
  Clear();
}


virtual ~AVideoWriter()
{
  CSingleLock lock( &m_locker, TRUE );
  Finalize();
  Clear();
}


virtual bool Initialize( LPCWSTR name, bool bInvertY, avideolib::IBaseVideo::ColorScheme scheme )
{
  CSingleLock lock( &m_locker, TRUE );
  try
  {
    Clear();
    m_file.open( name, false );
    m_file.write( &m_videoHeader, sizeof(VideoHeader) );  // initially write empty file header
    m_videoHeader.version = avideolib::AVIDEO_VERSION;
    m_videoHeader.colorScheme = (__int32)scheme;
    m_bInvertY = bInvertY;
    m_bGood = true;
  }
  catch (alib::GeneralError & e)
  {
    Clear();
    PrintErrorMessage( e );
  }
  return m_bGood;
}


virtual bool WriteFrame( const Arr2ub * pImage, unsigned __int32 milliTime )
{
  CSingleLock lock( &m_locker, TRUE );
  try
  {
    ALIB_ASSERT( m_bGood && (pImage != 0) && !(pImage->empty()) );

    int nByte = m_videoHeader.BytesPerPixel();
    int W = (m_count == 0) ? (m_videoHeader.width  = pImage->width())  : m_videoHeader.width;
    int H = (m_count == 0) ? (m_videoHeader.height = pImage->height()) : m_videoHeader.height;
    int scanSize = nByte * W;

    ALIB_VERIFY( ((W == pImage->width()) && (H == pImage->height())), DIFFERENT_SIZES );
    if ((int)(m_scan.size()) < scanSize)
      m_scan.resize( scanSize );

    // Write frame header.
    m_frameHeader.index = m_count;
    m_frameHeader.time = this->GetTime( milliTime );
    m_file.write( &m_frameHeader, sizeof(FrameHeader) );

    // Write file scan by scan.
    for (int y = 0; y < H; y++)
    {
      const ubyte * src = pImage->row_begin( m_bInvertY ? (H-1-y) : y );
      ubyte       * dst = &(*(m_scan.begin()));

      if (nByte >= 3)                  // 24-bits image
      {
        for (int k = 0; k < W; k++)
        {
          dst[0] = (dst[1] = (dst[2] = src[k]));
          dst += nByte;
        }
      }
      else                             // 8-bits image
      {
        memcpy( dst, src, W*sizeof(ubyte) );
      }

      m_file.write( &(*(m_scan.begin())), scanSize );
    }

    ++m_count;
  }
  catch (alib::GeneralError & e)
  {
    m_bGood = false;
    PrintErrorMessage( e );
  }
  return m_bGood;
}


virtual bool WriteFrame( const AImage * pImage, unsigned __int32 milliTime )
{
  CSingleLock lock( &m_locker, TRUE );
  try
  {
    ALIB_ASSERT( m_bGood && (pImage != 0) && !(pImage->empty()) && (sizeof(AImage::value_type) == 4) );

    int nByte = m_videoHeader.BytesPerPixel();
    int W = (m_count == 0) ? (m_videoHeader.width  = pImage->width())  : m_videoHeader.width;
    int H = (m_count == 0) ? (m_videoHeader.height = pImage->height()) : m_videoHeader.height;
    int scanSize = nByte * W;

    ALIB_VERIFY( ((W == pImage->width()) && (H == pImage->height())), DIFFERENT_SIZES );
    if ((int)(m_scan.size()) < scanSize)
      m_scan.resize( scanSize );

    // Write frame header.
    m_frameHeader.index = m_count;
    m_frameHeader.time = this->GetTime( milliTime );
    m_file.write( &m_frameHeader, sizeof(FrameHeader) );

    // Write file scan by scan.
    for (int y = 0; y < H; y++)
    {
      const ubyte * src = reinterpret_cast<const ubyte*>( pImage->row_begin( m_bInvertY ? (H-1-y) : y ) );
      ubyte       * dst = &(*(m_scan.begin()));

      if (nByte >= 3)                  // 24-bits image
      {
        for (int k = 0; k < W; k++)
        {
          dst[0] = src[0];
          dst[1] = src[1];
          dst[2] = src[2];
          dst += nByte;
          src += sizeof(AImage::value_type);
        }
      }
      else                             // 8-bits image
      {
        for (int k = 0; k < W; k++)
        {
          dst[k] = (ubyte)(((uint)(src[2])*306 + (uint)(src[1])*601 + (uint)(src[0])*117 + 512) >> 10);
          src += sizeof(AImage::value_type);
        }
      }

      m_file.write( &(*(m_scan.begin())), scanSize );
    }

    ++m_count;
  }
  catch (alib::GeneralError & e)
  {
    m_bGood = false;
    PrintErrorMessage( e );
  }
  return m_bGood;
}


virtual bool SetData( const alib::IDataType * pData )
{
  CSingleLock lock( &m_locker, TRUE );
  try
  {
    ALIB_ASSERT( pData != 0 );
    switch (pData->type())
    {
      case alib::DATAID_STATE_FLAG:
      {
        const alib::TStateFlag * pState = dynamic_cast<const alib::TStateFlag*>( pData );
        ALIB_ASSERT( pState != 0 );
        pState;
      }
      break;
    }
  }
  catch (alib::GeneralError & e)
  {
    PrintErrorMessage( e );
    return false;
  }
  return true;
}


virtual bool GetData( alib::IDataType * pData ) const
{
  CSingleLock lock( &((const_cast<AVideoWriter*>(this))->m_locker), TRUE );
  try
  {
    ALIB_ASSERT( pData != 0 );
    switch (pData->type())
    {
      case alib::DATAID_STATE_FLAG:
      {
        alib::TStateFlag * pState = dynamic_cast<alib::TStateFlag*>( pData );
        ALIB_ASSERT( pState != 0 );
        pState;
      }
      break;
    }
  }
  catch (alib::GeneralError & e)
  {
    PrintErrorMessage( e );
    return false;
  }
  return true;
}

};


//-------------------------------------------------------------------------------------------------
/** \brief Function creates an instance of video writer.

  \param  name        file name.
  \param  pOutput     pointer to a message handler.
  \param  bTrueColor  nonzero means 24-bits frame format, otherwise in 8-bits format will be used.
  \param  bInvertY    if nonzero, then axis Y will be inverted.
  \return             Ok = pointer to the newly created instance, otherwise 0. */
//-------------------------------------------------------------------------------------------------
avideolib::IVideoWriter * CreateAVideoWriter( const WCHAR    * name,
                                              alib::IMessage * pOutput,
                                              bool             bTrueColor,
                                              bool             bInvertY )
{
  AVideoWriter * p = new AVideoWriter( pOutput );

  if (p != 0)
  {
    avideolib::IBaseVideo::ColorScheme scheme = bTrueColor ? avideolib::IBaseVideo::TRUECOLOR
                                                           : avideolib::IBaseVideo::GRAYSCALED;
    if (p->Initialize( name, bInvertY, scheme ))
      return p;
    else
      delete p;
  }

  if (pOutput != 0)
    pOutput->PrintW( alib::IMessage::mt_error, _T("Failed to create video writer") );
  return 0;
}

} // namespace avideolib

