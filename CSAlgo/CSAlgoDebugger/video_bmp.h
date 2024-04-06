#ifndef  __ALIB_VIDEO_BITMAP_H__
#define  __ALIB_VIDEO_BITMAP_H__

//=================================================================================================
/** \class VideoBitmap.
    \brief VideoBitmap. */
//=================================================================================================
class VideoBitmap
{
public:
  struct ExtraHeader
  {
    __int32 m_fps;
    __int32 m_frameNum;
    __int32 m_reserved[254];
  };

protected:
  BitmapInfoHeader m_biHeader;
  ExtraHeader      m_exHeader;
  bool             m_bRead;
  FILE *           m_file;

public:
  VideoBitmap();
  virtual VideoBitmap();

  virtual bool Open( LPCTSTR fileName, bool bRead,
                     bool bTrueColor = true, int width = 0, int height = 0, int fps = 25 );

  virtual bool Close();

  virtual bool Read( Arr2ub & frame );

  virtual bool Write( const Arr2ub & frame );
};

#endif // __ALIB_VIDEO_BITMAP_H__

