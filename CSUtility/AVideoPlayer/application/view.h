/********************************************************************************
  view.h
  ---------------------
  begin     : May 2003
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  e-mail    : aaah@mail.ru, aaahaaah@hotmail.com
********************************************************************************/

#pragma once

enum GrabberTypeId
{
  GRABBER_UNKNOWN = 0,
  GRABBER_BITMAP,
  GRABBER_AVIDEO
};

static const TCHAR * APP_TITLE = _T("AVideo Player");
static const TCHAR * VIDEO_FILE_EXTENSION[] = { _T("*"), _T("bmp"), _T("avd") };

const float PLAY_SPEED[] = { 0.1f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f, 8.0f };

//=================================================================================================
/** \class CClientView.
    \brief CClientView. */
//=================================================================================================
class CClientView : public CWnd
{
public:
  typedef  std::auto_ptr< avideolib::IVideoReader >  ReaderPtr;
  typedef  std::auto_ptr< avideolib::IVideoWriter >  WriterPtr;
  typedef  OneImageLayout< DOWNWARD_AXIS_Y >         Layout;

  DebugOutput   m_output;      //!< message handler
  ReaderPtr     m_reader;      //!< pointer to a video sequence reader
  WriterPtr     m_writer;      //!< pointer to a video sequence writer
  AImage        m_image;       //!< the current frame
  Layout        m_layout;      //!< image layout
  StdStr        m_file;        //!< name of the input file
  GrabberTypeId m_grabberId;   //!< identifier (type) of video sequence reader
  StdStr        m_videoInfo;   //!< string contains information about currently opened video sequence
  __int32       m_colorScheme; //!< color scheme of an input videofile
  __int32       m_width;       //!< frame width
  __int32       m_height;      //!< frame height
  __int32       m_frameNum;    //!< the number of frames in a sequence
  __int32       m_frameNo;     //!< index of the current frame
  __int32       m_frameTime;   //!< timestamp of the current frame
  bool          m_bPlay;       //!< the flag indicates that a sequence is being played now
  DWORD         m_sleepTime;   //!< the time to sleep between successive frames
  float         m_fps;         //!< frames per second (frame rate)
  int           m_speedNo;     //!< the index of playing speed of a videosequence
  int           m_headFrame;   //!< the first frame to be written
  int           m_lastFrame;   //!< the last frame to be written
  float         m_writeFPS;    //!< FPS of an output videofile

public:
  CClientView();
  virtual ~CClientView();
  virtual BOOL PreCreateWindow( CREATESTRUCT & cs );

  void Clear();
  bool CreateFrameGrabber();
  bool GetVideoInfo();

public:
  DECLARE_MESSAGE_MAP()
  afx_msg void OnSize( UINT nType, int cx, int cy );
  afx_msg void OnPaint();
};

