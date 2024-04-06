/****************************************************************************
  algo_debugger.h
  ---------------------
  begin     : Aug 2004
  modified  : 24 Aug 2005
  author(s) : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

const wchar_t CAMERA_ANALYZER_MODE_NAME[] = L"Camera Analyzer";

class CCamAnalyzerDoc;

//=================================================================================================
/** \class CAlgoDebuggerApp.
    \brief The main application class. */
//=================================================================================================
class CAlgoDebuggerApp : public CWinApp
{
public:
  friend class CMainFrame;
  typedef  csalgocommon::TCameraStatistics  CameraStatistics;
  typedef  std::pair<StdStr,int>            AlgoNameAndId;
  typedef  std::vector<AlgoNameAndId>       AlgoNameAndIdArr;

public:
  CAlgoDebuggerApp();
  ~CAlgoDebuggerApp();

public:
  virtual BOOL InitInstance();
  virtual BOOL OnIdle( LONG lCount );

private:
  CDocTemplate *   m_pCamAnalyzerDocTemplate; //!< pointer to the document template of a camera analyzer
  std::wstring     m_appPath;                 //!< full application's path with trailing backslash
  HMODULE          m_hAlgoLib;                //!< handle of currently loaded algorithm DLL
  LARGE_INTEGER    m_frequency;               //!< performance frequency, in counts per millisecond
  LARGE_INTEGER    m_clockStart;              //!< the start value of the high-resolution performance counter

  bool             m_bRunning;                //!< nonzero while surveillance process is under way
  StdStr           m_hallAnalyzerLogFile;     //!< the name of log-file of a hall analyzer
  IHallAnalyzerPtr m_pHallAnalyzer;           //!< pointer to an instance of a hall analyzer
  BaseChairArr     m_chairs;                  //!< all chairs of the current hall
  CameraStatistics m_camStatistics;           //!< data storage used to transfer statistics from a camera analyzer to a hall analyzer
  int              m_demoCounter;             //!< frame counter used to create demo

public:
  StdStr           m_city_address;            //!< label that contains city and address
  StdStr           m_city_address_hall;       //!< label that contains city, address and hall
  bool             m_bPlaylist;               //!< nonzero, if videofiles were loaded through playlist
  PlaylistItem     m_playListItem;            //!< temporal playlist item

  FrameGrabberParameters m_grabberParams;     //!< parameters of frame grabber
  __int32                m_spectatorNum;      //!< the current spectator number
  AlgoDebugOutput        m_output;            //!< message handler
  AppParameters          m_appParams;         //!< parameters of application
  StdStr                 m_appParamFileName;  //!< the name of file that contains parameters of application

  csalgocommon::PCreateBackgroundAccumulator m_CreateBackgroundAccumulator;  //!< pointer to CreateBackgroundAccumulator()
  csalgocommon::PCreateCameraAnalyzer        m_CreateCameraAnalyzer;         //!< pointer to CreateCameraAnalyzer()
  csalgocommon::PCreateHallAnalyzer          m_CreateHallAnalyzer;           //!< pointer to CreateHallAnalyzer()
  csalgocommon::PShowParameterDialog         m_ShowParameterDialog;          //!< pointer to ShowParameterDialog()
  csalgocommon::PSetDefaultParameters        m_SetDefaultParameters;         //!< pointer to SetDefaultParameters()
  csalgocommon::PShowParameterDialogEx       m_ShowParameterDialogEx;        //!< pointer to ShowParameterDialogEx()
  csalgocommon::PSetDefaultParametersEx      m_SetDefaultParametersEx;       //!< pointer to SetDefaultParametersEx()
  csalgocommon::PGetParameters               m_GetParameters;                //!< pointer to GetParameters()

public:
  void           Clear( bool bClearPlaylistItem = true );
  void           LaunchProcessing();
  void           StopProcessing();
  void           DeleteHallAnalyzer();
  __int64        Time();
  bool           AreDataReady() const;
  bool           IsRunning() const { return m_bRunning; }
  bool           TransferData( ICameraAnalyzer * pCamAnalyzer );
  void           DrawCurrentResult( CCamAnalyzerDoc * pDoc );
  CDocTemplate * GetCamAnalyzerDocTempl() { return m_pCamAnalyzerDocTemplate; }
  bool           ReloadAlgorithmLibrary();
  LPCWSTR        GetAppPath() { return m_appPath.c_str(); }

public:
  DECLARE_MESSAGE_MAP()
  afx_msg void OnAppAbout();
  afx_msg void OnFileNew();
  afx_msg void OnFileOpen();
  afx_msg void OnUpdateFileNew( CCmdUI * pCmdUI );
};

extern CAlgoDebuggerApp theApp;

