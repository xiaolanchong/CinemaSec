///////////////////////////////////////////////////////////////////////////////////////////////////
// avideo_reader.cpp
// ---------------------
// begin     : Apr 2005
// modified  : 24 Jul 2005
// author(s) : Albert Akhriev
// copyright : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "avideo_header.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace avideolib
{

//=================================================================================================
/** \class AVideoGrabber.
    \brief Class sequentially reads images from a video file. */
//=================================================================================================
class AVideoReader : public avideolib::IVideoReader
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
  bool             m_bLoopback;   //!< if nonzero, then a sequence will be infinitely played from the beginning
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
  m_bLoopback = false;
  m_locker;
}


void PrintErrorMessage( alib::GeneralError & e ) const
{
  if (m_pOutput != 0)
    m_pOutput->PrintA( alib::IMessage::mt_error, e.what() );
}


public:
AVideoReader( alib::IMessage * pOutput )
{
  m_pOutput = pOutput;
  Clear();
}


virtual ~AVideoReader()
{
  CSingleLock lock( &m_locker, TRUE );
  Clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function fulfils some common operations before frame reading.

  \return  nonzero means normal return, zero means that the end of a sequence has been reached. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual bool PreReadFrame( __int32 desiredNo, __int32 * pFrameNo, __int32 * pMilliTime )
{
  ALIB_ASSERT( m_bGood );
  ALIB_VERIFY( m_videoHeader.frameNum > 0, CORRUPTED_FILE );

  bool retValue = true;
  int  scanSize = m_videoHeader.BytesPerPixel() * m_videoHeader.width;

  if ((int)(m_scan.size()) < scanSize)
    m_scan.resize( scanSize );

  // Decide what to do next.
  if (desiredNo >= 0)
  {
    ALIB_ASSERT( Seek( m_count = alib::Limit( desiredNo, 0, m_videoHeader.frameNum-1 ) ) );
  }
  else if (m_bLoopback)
  {
    if (m_count >= m_videoHeader.frameNum)
      ALIB_ASSERT( Seek( m_count = (m_count % m_videoHeader.frameNum) ) );
  }
  else if (m_count >= (m_videoHeader.frameNum-1))
  {
    ALIB_ASSERT( Seek( m_count = (m_videoHeader.frameNum-1) ) );
    retValue = false;
  }

  // Read frame header.
  m_file.read( &m_frameHeader, sizeof(FrameHeader) );
  ALIB_VERIFY( (m_frameHeader.thisSize == sizeof(FrameHeader)), WRONG_HEADER );
  ALIB_VERIFY( (m_frameHeader.index == m_count), CORRUPTED_FILE );

  if (pFrameNo != 0) (*pFrameNo) = m_frameHeader.index;
  if (pMilliTime != 0) (*pMilliTime) = m_frameHeader.time;
  return retValue;
}


virtual bool Initialize( LPCWSTR name, bool bInvertY )
{
  CSingleLock lock( &m_locker, TRUE );
  try
  {
    Clear();
    m_file.open( name, true );
    m_file.read( &m_videoHeader, sizeof(m_videoHeader) );
    ALIB_VERIFY( (m_videoHeader.thisSize == sizeof(VideoHeader)), WRONG_HEADER );
    ALIB_VERIFY( (m_videoHeader.version == avideolib::AVIDEO_VERSION), WRONG_VERSION );
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


virtual bool Seek( __int32 frameNo )
{
  __int64 videoHeaderSize = sizeof(VideoHeader);
  __int64 frameHeaderSize = sizeof(FrameHeader);
  __int64 frameContentSize = m_videoHeader.width * m_videoHeader.height * m_videoHeader.BytesPerPixel();

  if (m_videoHeader.frameNum < 1)
    return false;
  frameNo = alib::Limit( frameNo, 0, m_videoHeader.frameNum-1 );
  m_file.seek( videoHeaderSize + (__int64)frameNo * (frameHeaderSize + frameContentSize), SEEK_SET );
  m_count = alib::Limit( frameNo, 0, m_videoHeader.frameNum-1 );
  return true;
}


virtual ReturnCode ReadFrame( Arr2ub * pImage, __int32 desiredNo, __int32 * pFrameNo, __int32 * pMilliTime )
{
  CSingleLock lock( &m_locker, TRUE );
  bool        bNormal = true;

  try
  {
    int nByte = m_videoHeader.BytesPerPixel();
    int W = m_videoHeader.width;
    int H = m_videoHeader.height;

    ALIB_ASSERT( pImage != 0 );
    bNormal = PreReadFrame( desiredNo, pFrameNo, pMilliTime );
    pImage->resize( W, H, Arr2ub::value_type(), false );

    // Read file scan by scan.
    for (int y = 0; y < H; y++)
    {
      m_file.read( &(*(m_scan.begin())), (int)(m_scan.size()) );

      ubyte * dst = pImage->row_begin( m_bInvertY ? (H-1-y) : y );
      ubyte * src = &(*(m_scan.begin()));

      if (nByte == 3)                    // 24-bits image
      {
        for (int k = 0; k < W; k++)
        {
          dst[k] = (ubyte)(((uint)(src[2])*306 + (uint)(src[1])*601 + (uint)(src[0])*117 + 512) >> 10);
          src += nByte;
        }
      }
      else                               // 8-bits image
      {
        memcpy( dst, src, W*sizeof(ubyte) );
      }
    }

    ++m_count;
  }
  catch (alib::GeneralError & e)
  {
    m_bGood = false;
    PrintErrorMessage( e );
    return avideolib::IVideoReader::GENERAL_ERROR;
  }
  return (bNormal ? (avideolib::IVideoReader::NORMAL) : (avideolib::IVideoReader::END_IS_REACHED));
}


virtual ReturnCode ReadFrame( AImage * pImage, __int32 desiredNo, __int32 * pFrameNo, __int32 * pMilliTime )
{
  CSingleLock lock( &m_locker, TRUE );
  bool        bNormal = true;

  try
  {
    int nByte = m_videoHeader.BytesPerPixel();
    int W = m_videoHeader.width;
    int H = m_videoHeader.height;

    ALIB_ASSERT( pImage != 0 );
    bNormal = PreReadFrame( desiredNo, pFrameNo, pMilliTime );
    pImage->resize( W, H, AImage::value_type(), false );

    // Read file scan by scan.
    for (int y = 0; y < H; y++)
    {
      m_file.read( &(*(m_scan.begin())), (int)(m_scan.size()) );

      ubyte * dst = reinterpret_cast<ubyte*>( pImage->row_begin( m_bInvertY ? (H-1-y) : y ) );
      ubyte * src = &(*(m_scan.begin()));

      if (nByte == 3)                    // 24-bits image
      {
        for (int k = 0; k < W; k++)
        {
          dst[0] = src[0];
          dst[1] = src[1];
          dst[2] = src[2];
          dst[3] = 0;
          dst += sizeof(AImage::value_type);
          src += nByte;
        }
      }
      else                               // 8-bits image
      {
        for (int k = 0; k < W; k++)
        {
          dst[0] = src[k];
          dst[1] = src[k];
          dst[2] = src[k];
          dst[3] = 0;
          dst += sizeof(AImage::value_type);
        }
      }
    }

    ++m_count;
  }
  catch (alib::GeneralError & e)
  {
    m_bGood = false;
    PrintErrorMessage( e );
    return avideolib::IVideoReader::GENERAL_ERROR;
  }
  return (bNormal ? (avideolib::IVideoReader::NORMAL) : (avideolib::IVideoReader::END_IS_REACHED));
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
  CSingleLock lock( &((const_cast<AVideoReader*>(this))->m_locker), TRUE );
  try
  {
    ALIB_ASSERT( pData != 0 );
    switch (pData->type())
    {
      case alib::DATAID_STATE_FLAG:
      {
        alib::TStateFlag * pState = dynamic_cast<alib::TStateFlag*>( pData );
        ALIB_ASSERT( pState != 0 );
        switch (pState->data)
        {
          case avideolib::IBaseVideo::VERSION     : pState->data = m_videoHeader.version;      break;
          case avideolib::IBaseVideo::WIDTH       : pState->data = m_videoHeader.width;        break;
          case avideolib::IBaseVideo::HEIGHT      : pState->data = m_videoHeader.height;       break;
          case avideolib::IBaseVideo::FRAME_NUMBER: pState->data = m_videoHeader.frameNum;     break;
          case avideolib::IBaseVideo::COLORSCHEME : pState->data = m_videoHeader.colorScheme;  break;
          case avideolib::IBaseVideo::FPS_1000    : pState->data = m_videoHeader.fps1000;      break;
          default                                 : return false;
        }
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


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function creates an instance of video reader.

  \param  name      file name.
  \param  pOutput   pointer to a message handler.
  \param  bInvertY  if nonzero, then axis Y will be inverted.
  \return           Ok = pointer to the newly created instance, otherwise 0. */
///////////////////////////////////////////////////////////////////////////////////////////////////
avideolib::IVideoReader * CreateAVideoReader( const WCHAR    * name,
                                              alib::IMessage * pOutput,
                                              bool             bInvertY )
{
  AVideoReader * p = new AVideoReader( pOutput );

  if (p != 0)
  {
    if (p->Initialize( name, bInvertY ))
      return p;
    else
      delete p;
  }

  if (pOutput != 0)
    pOutput->PrintW( alib::IMessage::mt_error, _T("Failed to create video reader") );
  return 0;
}

} // namespace avideolib

