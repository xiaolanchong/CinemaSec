#pragma once

class CCamAnalyzerView;
class CCamAnalyzerFrame;

//=================================================================================================
/** \class CCamAnalyzerDoc.
    \brief Document class of test mode. */
//=================================================================================================
class CCamAnalyzerDoc : public CDocument
{
public:
  friend class CCamAnalyzerFrame;
  typedef  csalgocommon::TFrameCounters  FrameCounters;
  typedef  csalgocommon::TDemoImage      DemoImage;

private:
public:
  bool    m_bEmptyDoc;                //!< non-zero if document is empty
  bool    m_bCamAnReady;              //!< non-zero if camera analyzer has been properly initialized
  bool    m_bCamAnRunning;            //!< non-zero if camera analyzer is running now
  __int64 m_prevTime;                 //!< the previous time when the last number of passed frames has been stored
  int     m_prevFrameNo;              //!< the lastest stored index of frame
  float   m_fps;                      //!< frames-per-second
  FOURCC  m_videoFormat;              //!< format of video-file
  StdStr  m_city_address_hall_camera; //!< label that contains city, address, hall and camera
  StdStr  m_dataPath;                 //!< full path of data to be loaded with trailing backslash

public:
  OneImageLayout     m_layout;       //!< layout of a single image on the screen
  BaseFrameGrabber * m_pGrabber;     //!< pointer to the instance of frame grabber
  GrabberType        m_grabberType;  //!< the type of frame grabber currently in use
  ICamAnalyzerPtr    m_pCamAnalyzer; //!< pointer to the instance of camera analyzer
  BaseChairArr       m_chairs;       //!< chairs covered by this camera
  StdStr             m_source;       //!< the short name of video file or the indicator of camera device.
  int                m_cameraNo;     //!< the index of a control zone (index of a camera).
  StdStr             m_statusText;   //!< statusbar's text
  FrameCounters      m_frmCount;     //!< frame counters
  Arr2f              m_background;   //!< precomputed static background (empty hall)
  DemoImage          m_demoImg;      //!< demo image and some additional information

protected:
  DECLARE_DYNCREATE(CCamAnalyzerDoc)
  CCamAnalyzerDoc();
  virtual ~CCamAnalyzerDoc();
  virtual bool OnIdleProcessing();

protected:
  bool LoadFirstFrame();
  void ParseInputFileName( LPCWSTR fileName );
  bool LoadPrecomputedData();
  void PrintException( const std::runtime_error * e = 0, LPCWSTR szMessage = 0 );
  void UpdateStatusbar( bool bStatusText, bool bIndicators );
  bool CreateFrameGrabber();
  bool DestroyFrameGrabber();

public:
  bool InitializeCameraAnalyzer();
  bool StartCameraAnalyzer();
  bool StopCameraAnalyzer();

  bool IsEmpty() const { return m_bEmptyDoc; }
  bool IsReady() const { return m_bCamAnReady; }
  bool IsRunning() const { return m_bCamAnRunning; }

  virtual void DeleteContents();
  virtual void OnCloseDocument();

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

public:
  DECLARE_MESSAGE_MAP()
  virtual BOOL OnOpenDocument( LPCWSTR lpszPathName );
  virtual BOOL CanCloseFrame( CFrameWnd * pFrame );
  virtual BOOL OnNewDocument();
};

