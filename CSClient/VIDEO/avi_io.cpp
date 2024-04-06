/****************************************************************************
                          avi_io.cpp
                             -------------------
    begin                : October 2004
    author               : Albert Akhriev
    email                : aaahaaah@hotmail.com, aaahaaah@yandex.ru
****************************************************************************/

#include "stdafx.h"
#include "avi_io.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//static  InitAviLibrary  init_avi_library; // reliable initialization for a SINGLE thread application

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
AviIO::AviIO()
{
  ASSERT( sizeof(m_header) == (sizeof(BITMAPINFOHEADER)+(1<<8)*sizeof(RGBQUAD)) );
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
AviIO::~AviIO()
{
  Close();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function opens specified AVI file for reading.

  \param  fileName  the name of the file to be opened.
  \return           Ok = true. */
//-------------------------------------------------------------------------------------------------
bool AviIO::OpenRead( LPCTSTR fileName )
{
  try
  {
    Close();
    if (fileName != 0)
    {
      m_bReadMode = true;
      OpenAviFile( fileName );
      m_ok = true;
      OpenAviStream();
      m_ok = (GetFrame(0) != 0);
    }
  }
  catch (std::runtime_error & e)
  {
    alib::ErrorMessage( _T("Failed to open AVI file for reading:"), fileName,
                        _T("reason:"), CString( e.what() ) );
    Close();
  }
  catch (...)
  {
    alib::ErrorMessage( ALIB_UNSUPPORTED_EXCEPTION );
    Close();
  }
  return m_ok;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function opens specified AVI file for writing.

  \param  fileName  the name of the file to be opened for writing.
  \param  bColor    nonzero for true-color (24 bits) stream and zero for grayscaled (8 bits) one.
  \param  width     the width of a frame.
  \param  height    the height of a frame.
  \param  fps       desired frames-per-second rate.
  \return           Ok = true. */
//-------------------------------------------------------------------------------------------------
bool AviIO::OpenWrite( LPCTSTR fileName, bool bColor, int width, int height, int fps )
{
  try
  {
    Close();
    if (fileName != 0)
    {
      m_bReadMode = false;
      OpenAviFile( fileName );
      CreateAviStream( bColor, width, height, fps );
      m_ok = true;
    }
  }
  catch (std::runtime_error & e)
  {
    alib::ErrorMessage( _T("Failed to open AVI file for writing:"), fileName,
                        _T("reason:"), CString( e.what() ) );
    Close();
  }
  catch (...)
  {
    alib::ErrorMessage( ALIB_UNSUPPORTED_EXCEPTION );
    Close();
  }
  return m_ok;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object. */
//-------------------------------------------------------------------------------------------------
void AviIO::Clear()
{
  m_pAVIFile = 0;
  m_pAVIStream = 0;
  m_pAVIGetFrame = 0;
  m_streamTime = 0;
  m_streamDuration = 0;
  m_bReadMode = true;
  memset( &m_aviStreamInfo, 0, sizeof(m_aviStreamInfo) );
  m_frameSize = 0;
  m_frameRate = 0.0f;
  m_frameDuration = 0;
  m_frameNumber = 0;
  m_frameIndex = 0;
  m_pDib = 0;
  m_ok = false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function closes the opened AVI stream. */
//-------------------------------------------------------------------------------------------------
void AviIO::Close()
{
  // Release resources used to decompress video frames.
  if (m_pAVIGetFrame != 0)
    ::AVIStreamGetFrameClose( m_pAVIGetFrame );

  // Decrement the reference count of an AVI stream and closes it if the count reaches zero.
  if (m_pAVIStream != 0)
    ::AVIStreamRelease( m_pAVIStream );

  // Decrement the reference count of an AVI file and closes the file if the count reaches zero.
  if (m_pAVIFile != 0)
    ::AVIFileRelease( m_pAVIFile );

  // Clear this object.
  Clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function opens AVI file.

  \param  fileName      the name of the file to be opened.
  \param  pWriteHeader  the pointer to the header for writing mode and zero for reading mode. */
//-------------------------------------------------------------------------------------------------
void AviIO::OpenAviFile( LPCTSTR filename )
{
  UINT flags = (m_bReadMode ? (OF_READ/* | OF_SHARE_DENY_WRITE*/) : (OF_CREATE | OF_WRITE));

  switch (::AVIFileOpen( &m_pAVIFile, filename, flags, 0 ))
  {
    case AVIERR_OK:
      break;

    case AVIERR_BADFORMAT:
      throw std::runtime_error( "The file couldn't be read,\n"
                                "indicating a corrupt file or an unrecognized format." );
      break;

    case AVIERR_MEMORY:
      throw std::runtime_error( "The file could not be opened because of insufficient memory." );
      break;

    case AVIERR_FILEREAD:
      throw std::runtime_error( "A disk error occurred while reading the file." );
      break;

    case AVIERR_FILEOPEN:
      throw std::runtime_error( "A disk error occurred while opening the file." );
      break;

    case REGDB_E_CLASSNOTREG:
      throw std::runtime_error( "According to the registry, the type of file specified in\n"
                                "AVIFileOpen does not have a handler to process it." );
      break;

    default:
      throw std::runtime_error( "Unknown error of AVIFileOpen()." );
      break;
  }

  if (m_pAVIFile == 0)
    throw std::runtime_error( "!AVIFileOpen()" );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function opens the first video-stream of the current AVI file. */
//-------------------------------------------------------------------------------------------------
void AviIO::OpenAviStream()
{
  const WORD BIT_COUNT[] = { 24, 16, 8, 24, 0 };

  if (m_pAVIFile == 0)
    throw std::runtime_error( "Zero AVI file pointer" );

  // Open the first video-stream.
  switch (::AVIFileGetStream( m_pAVIFile, &m_pAVIStream, streamtypeVIDEO, 0 ))
  {
    case AVIERR_OK:
      break;

    case AVIERR_NODATA:
      throw std::runtime_error( "The file does not contain a stream\n"
                                "corresponding to the values of fccType and lParam." );
      break;

    case AVIERR_MEMORY:
      throw std::runtime_error( "Not enough memory." );

    default:
      throw std::runtime_error( "Unknown error of AVIFileGetStream()." );
      break;
  }

  if (m_pAVIStream == 0)
    throw std::runtime_error( "!AVIFileGetStream()" );

  // Get video source information.
  if (::AVIStreamInfo( m_pAVIStream, &m_aviStreamInfo, sizeof(m_aviStreamInfo) ))
    throw std::runtime_error( "!AVIStreamInfo()" );

  // Try several color schemes to open frame.
  m_pAVIGetFrame = 0;
  for (int attempt = 0; (attempt < ALIB_LengOf(BIT_COUNT)) && (m_pAVIGetFrame == 0); attempt++)
  {
    BITMAPINFOHEADER * pbi = &(m_header.bi.bmiHeader);

    // Set bitmap info for the AVIStreamGetFrameOpen call.
    memset( pbi, 0, sizeof(BITMAPINFOHEADER) );
    pbi->biSize = sizeof(BITMAPINFOHEADER);
    pbi->biWidth = abs( m_aviStreamInfo.rcFrame.right - m_aviStreamInfo.rcFrame.left );
    pbi->biHeight = abs( m_aviStreamInfo.rcFrame.bottom - m_aviStreamInfo.rcFrame.top );
    pbi->biPlanes = 1;
    pbi->biBitCount = BIT_COUNT[attempt];
    pbi->biCompression = (BIT_COUNT[attempt] == 16) ? MY_BI_YUY2 : BI_RGB;
    pbi->biSizeImage = (DWORD)(m_frameSize = MyGetDibContentsSizeInBytes( pbi ));

    // Prepare to decompress video frames from the specified video stream.
    m_pAVIGetFrame = ::AVIStreamGetFrameOpen( m_pAVIStream, (BIT_COUNT[attempt] == 0) ? 0 : pbi );
  }
  if (m_pAVIGetFrame == 0)
    throw std::runtime_error( "!AVIStreamGetFrameOpen()" );

  m_frameNumber = ::AVIStreamLength( m_pAVIStream );
  if (m_frameNumber <= 0)
    throw std::runtime_error( "!AVIStreamLength()" );

  if ((m_aviStreamInfo.dwRate > 0) && (m_aviStreamInfo.dwScale > 0))
  {
    m_frameRate = (float)(m_aviStreamInfo.dwRate)/(float)(m_aviStreamInfo.dwScale);

    m_frameDuration = (int)ceil( (1000.0 * m_aviStreamInfo.dwScale)/
                                    (float)(m_aviStreamInfo.dwRate) );

    m_streamDuration = (int)ceil( (1000.0 * m_frameNumber * m_aviStreamInfo.dwScale)/
                                                     (float)(m_aviStreamInfo.dwRate) );
  }
  else
  {
    m_frameRate = 0.0f;
    m_frameDuration = 0;
    m_streamDuration = 0;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function creates the first video-stream of the current AVI file.

  \param  bColor  nonzero for true-color (24 bits) stream and zero for grayscaled (8 bits) one.
  \param  width   the width of a frame.
  \param  height  the height of a frame.
  \param  fps     desired frames-per-second rate. */
//-------------------------------------------------------------------------------------------------
void AviIO::CreateAviStream( bool bColor, int width, int height, int fps )
{
  BITMAPINFO * pbi = &(m_header.bi);
  int          bitCount = (bColor ? 24 : 8);

  ASSERT( !m_bReadMode && (width > 0) && (height > 0) && ALIB_IS_LIMITED( fps, 1, 50 ) );
  memset( pbi, 0, sizeof(m_header) );
  m_frameSize = ((((width*bitCount)+31)&(~31))/8)*height;

  // Fill BITMAPINFOHEADER structure.
  pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  pbi->bmiHeader.biWidth = width;
  pbi->bmiHeader.biHeight = height;
  pbi->bmiHeader.biPlanes = 1;
  pbi->bmiHeader.biBitCount = (WORD)bitCount;
  pbi->bmiHeader.biCompression = BI_RGB;
  pbi->bmiHeader.biSizeImage = m_frameSize;
  pbi->bmiHeader.biClrUsed = (bColor ? 0 : (1<<8));

  // Fill color palette (meaningful only for grayscaled images).
  for (int i = 0; i < 256; i++)
  {
    pbi->bmiColors[i].rgbRed   = (BYTE)i;
    pbi->bmiColors[i].rgbGreen = (BYTE)i;
    pbi->bmiColors[i].rgbBlue  = (BYTE)i;
  }

  // Set parameters for the new stream. 
  memset( &m_aviStreamInfo, 0, sizeof(m_aviStreamInfo) );
  m_aviStreamInfo.fccType = streamtypeVIDEO;                  // video stream type
  m_aviStreamInfo.fccHandler = mmioFOURCC('D','I','B',' ');   // uncompressed DIB
  m_aviStreamInfo.dwScale = 1;                                // should be one for video
  m_aviStreamInfo.dwRate = fps;                               // fps
  m_aviStreamInfo.dwSuggestedBufferSize = m_frameSize;        // recommended buffer size, in bytes, for the stream
  SetRect( &(m_aviStreamInfo.rcFrame), 0, 0, width, height );
#if 0
  _tcscpy( m_aviStreamInfo.szName, (bColor ? _T("24-bits AVI") : _T("8-bits AVI")) );
#else
  StringCchCopy( m_aviStreamInfo.szName, 64, (bColor ? _T("24-bits AVI") : _T("8-bits AVI")) );
#endif

  // Create a stream.
  if (::AVIFileCreateStream( m_pAVIFile, &m_pAVIStream, &m_aviStreamInfo ) != 0)
    throw std::runtime_error( "!AVIFileCreateStream()" );

  // Set format of new stream.
  void * lpFormat = (bColor ? reinterpret_cast<void*>( &(pbi->bmiHeader) )
                            : reinterpret_cast<void*>( pbi ));
  long   cbFormat = (bColor ? sizeof(BITMAPINFOHEADER) : sizeof(m_header));

  if (::AVIStreamSetFormat( m_pAVIStream, 0, lpFormat, cbFormat ) != 0)
    throw std::runtime_error( "!AVIStreamSetFormat()" );

  m_frameRate = 0.0f;
  m_frameDuration = 0;
  m_streamDuration = 0;
  if ((m_aviStreamInfo.dwRate > 0) && (m_aviStreamInfo.dwScale > 0))
  {
    m_frameRate = (float)(m_aviStreamInfo.dwRate)/(float)(m_aviStreamInfo.dwScale);

    m_frameDuration = (int)ceil( (1000.0 * m_aviStreamInfo.dwScale)/
                                    (float)(m_aviStreamInfo.dwRate) );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function puts the next frame in a sequence and increments frame counter.

  \param  data  pointer to the frame's content.
  \param  size  the size of content in bytes.
  \return       Ok = true. */
//-------------------------------------------------------------------------------------------------
bool AviIO::PutNextFrame( const void * data, int size )
{
  if (!m_ok || m_bReadMode || (m_frameSize != size))
    return false;

  return (::AVIStreamWrite( m_pAVIStream, m_frameNumber++, 1,
                            const_cast<void*>( data ), size, AVIIF_KEYFRAME, 0, 0 ) == 0);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function gets the next frame of a sequence and increments frame index.

  \return Ok = true. */
//-------------------------------------------------------------------------------------------------
bool AviIO::GetNextFrame()
{
  if (!m_ok || !m_bReadMode || (m_pDib == 0))
    return false;

  if ((m_frameIndex+1) < m_frameNumber)
    return GetFrame( m_frameIndex+1 );

  return false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function gets the frame of a sequence with specified index and
           sets the internal frame index by the new value.

  \param  index  the index of the frame of interest.
  \return        Ok = true. */
//-------------------------------------------------------------------------------------------------
bool AviIO::GetFrame( int index )
{
  if (!m_ok || !m_bReadMode || !ALIB_IS_RANGE( index, 0, m_frameNumber ))
    return false;

  if ((index != m_frameIndex) || (m_pDib == 0))
  {
    m_pDib = reinterpret_cast<BITMAPINFOHEADER*>( 
               ::AVIStreamGetFrame( m_pAVIGetFrame, index+m_aviStreamInfo.dwStart ) );

    if (m_pDib != 0)
    {
      m_frameIndex = index;
      m_streamTime = static_cast<int>( ::AVIStreamSampleToTime( m_pAVIStream, index ) );
    }
    else if (index != 0)
    {
      for (int attempt = 0; (attempt < 100) && (m_pDib == 0) && (index < m_frameNumber); attempt++)
      {
        ++index;
        m_pDib = reinterpret_cast<BITMAPINFOHEADER*>( 
                   ::AVIStreamGetFrame( m_pAVIGetFrame, index+m_aviStreamInfo.dwStart ) );
      }

      if (m_pDib != 0)
      {
        m_frameIndex = index;
        m_streamTime = static_cast<int>( ::AVIStreamSampleToTime( m_pAVIStream, index ) );
      }
    }

    if (m_pDib == 0)
    {
      Close();
    }
    else if (m_pDib->biCompression == MY_BI_YUY2) // invert axis Y in this special case
    {
      int     scanWidth = MyGetDibScanSizeInBytes( m_pDib );
      int     height = abs( m_pDib->biHeight );
      ubyte * row1 = reinterpret_cast<ubyte*>( m_pDib+1 ) + scanWidth * 0;
      ubyte * row2 = reinterpret_cast<ubyte*>( m_pDib+1 ) + scanWidth * (height-1);

      for (; row1 < row2; row1 += scanWidth, row2 -= scanWidth)
      {
        for (int x = 0; x < scanWidth; x++)
          std::swap( row1[x], row2[x] );
      }
    }
  }
  return (m_pDib != 0);
}

