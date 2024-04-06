/****************************************************************************
                          avi_io.h
                             -------------------
    begin                : October 2004
    author               : Albert Akhriev
    email                : aaahaaah@hotmail.com, aaahaaah@yandex.ru
****************************************************************************/

#ifndef MY_AVI_IO_H
#define MY_AVI_IO_H

//=================================================================================================
/** \class InitAviLibrary.
    \brief Simple initializer of AVI library. */
//=================================================================================================
struct InitAviLibrary
{
  InitAviLibrary()  { ::AVIFileInit(); }
  ~InitAviLibrary() { ::AVIFileExit(); }
};


//=================================================================================================
/** \class AviIO.
    \brief Class is designed to ease manipulation with AVI streams. */
//=================================================================================================
class AviIO
{
private:
  bool               m_ok;             //!< state of AVI stream
  PAVIFILE           m_pAVIFile;       //!< 
  PAVISTREAM         m_pAVIStream;     //!< 
  PGETFRAME          m_pAVIGetFrame;   //!< 
  int                m_streamTime;     //!< 
  int                m_streamDuration; //!< 
  AVISTREAMINFO      m_aviStreamInfo;  //!<
  bool               m_bReadMode;      //!< if true, then the stream has been opened for reading, otherwise for writing
  int                m_frameSize;      //!< the size of frame in bytes
  float              m_frameRate;      //!<
  int                m_frameDuration;  //!< duration of a single frame in milliseconds
  int                m_frameNumber;    //!<
  int                m_frameIndex;     //!<
  BITMAPINFOHEADER * m_pDib;           //!< pointer to the current frame

  union
  {
    char       buffer[sizeof(BITMAPINFOHEADER)+(1<<8)*sizeof(RGBQUAD)];
    BITMAPINFO bi;
  }
  m_header;                            //!< DIB's header

private:
  void CreateAviStream( bool bColor, int width, int height, int fps );
  void OpenAviFile( LPCTSTR filename );
  void OpenAviStream();
  void Clear();

public:
  AviIO();
  ~AviIO();

  bool OpenRead( LPCTSTR fileName );
  bool OpenWrite( LPCTSTR fileName, bool bColor, int width, int height, int fps );
  void Close();

  bool PutNextFrame( const void * data, int size );
  bool GetFrame( int index );
  bool GetNextFrame();

  const AVISTREAMINFO & GetAviInfo() const { return m_aviStreamInfo; }
  const BITMAPINFO    & GetDibHeader() const { return m_header.bi; }

  bool IsOk()  const { return m_ok;   }
  bool IsEnd() const { return ((m_frameIndex+1) >= m_frameNumber); }

  float GetFrameRate()     const { return m_frameRate;     }
  int   GetFrameDuration() const { return m_frameDuration; }
  int   GetFrameNumber()   const { return m_frameNumber;   }
  int   GetFrameIndex()    const { return m_frameIndex;    }

  const BITMAPINFOHEADER * GetDIB() const { return (m_ok ? m_pDib : 0); }
};

typedef  std::auto_ptr<AviIO>  AviIOAutoPtr;

#endif // MY_AVI_IO_H

