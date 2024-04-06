/********************************************************************************
  application.cpp
  ---------------------
  begin     : May 2003
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  e-mail    : aaah@mail.ru, aaahaaah@hotmail.com
********************************************************************************/

#include "stdafx.h"
#include "..\resource.h"
#include "view.h"
#include "toolbar.h"
#include "mainfrm.h"
#include "application.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTrackApp, CWinApp)
  ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()

CTrackApp theApp;

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CTrackApp::CTrackApp()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Initializes each new instance of your application running under Windows. */
//-------------------------------------------------------------------------------------------------
BOOL CTrackApp::InitInstance()
{
  // InitCommonControls() is required on Windows XP if an application manifest specifies use of
  // ComCtl32.dll version >= 6 to enable visual styles. Otherwise, any window creation will fail.
  InitCommonControls();

  CWinApp::InitInstance();

  // Standard initialization. If you are not using these features and wish to reduce the size
  // of your final executable, you should remove from the following the specific initialization
  // routines you do not need. Change the registry key under which our settings are stored
  // TODO: You should modify this string to be something appropriate such as the name of your
  // company or organization.
  //SetRegistryKey( _T("Type the name of this application") );

  // To create the main window, this code creates a new frame window
  // object and then sets it as the application's main window object.
  CMainFrame * pFrame = new CMainFrame;
  if (pFrame == 0)
    return FALSE;
  m_pMainWnd = pFrame;

  // Create and load the frame with its resources.
  pFrame->LoadFrame( IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL );

  // The one and only window has been initialized, so show and update it.
  pFrame->ShowWindow( SW_SHOW );
  pFrame->UpdateWindow();

  // Set icon.
  HICON hIcon = LoadIcon( IDR_MAINFRAME );
  if (hIcon != 0)
    pFrame->SetIcon( hIcon, FALSE );

  // Call DragAcceptFiles only if there's a suffix. In an SDI app, this should occur after ProcessShellCommand.
  pFrame->DragAcceptFiles( TRUE );
  return TRUE;
}


//-------------------------------------------------------------------------------------------------
/** \brief Application command to run 'About' dialog. */
//-------------------------------------------------------------------------------------------------
void CTrackApp::OnAppAbout()
{
  AfxMessageBox( _T("This program was dedicated to play a sequence of\n"
                    "bitmap files or video-files stored in AVD format.\n\n"
                    "The program has been written\n"
                    "by Albert Akhriev, Aug 2005\n"
                    "aaah@mail.ru, aaahaaah@hotmail.com"), MB_OK | MB_ICONINFORMATION );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function performs idle-time processing. */
//-------------------------------------------------------------------------------------------------
BOOL CTrackApp::OnIdle( LONG lCount )
{
  BOOL bStopWrite = FALSE;
  BOOL retValue = CWinApp::OnIdle( lCount );

  if (m_pMainWnd == 0)
    return retValue;

  CClientView & view = (reinterpret_cast<CMainFrame*>( m_pMainWnd ))->m_view;
  if (!(view.m_bPlay) || (view.m_reader.get() == 0))
    return retValue;

  // Read next frame.
  int res = view.m_reader->ReadFrame( &(view.m_image), -1, &(view.m_frameNo), &(view.m_frameTime) );

  if ((res == avideolib::IVideoReader::NORMAL) || (res == avideolib::IVideoReader::END_IS_REACHED))
  {
    if (view.m_writer.get() != 0)
    {
      if (alib::IsLimited( view.m_frameNo, view.m_headFrame, view.m_lastFrame ))
      {
        __int32 time = (__int32)(floor( 1000.0*(view.m_frameNo-view.m_headFrame) /
                                        std::max<double>( view.m_writeFPS, 1.0 ) ) + 0.5);

        if (!(view.m_writer->WriteFrame( &(view.m_image), time )))
          view.m_writer.reset();
      }
      else if (view.m_frameNo > view.m_lastFrame) bStopWrite = TRUE;
    }
  }

  if (res == avideolib::IVideoReader::NORMAL)
  {
    retValue = TRUE;
  }
  else if (res != avideolib::IVideoReader::NO_FRESH_FRAME)
  {
    view.m_reader.reset();
    view.m_writer.reset();
  }

  if (((view.m_frameNo+1) >= view.m_frameNum) || bStopWrite ||
      (res == avideolib::IVideoReader::END_IS_REACHED))
  {
    (reinterpret_cast<CMainFrame*>( m_pMainWnd ))->OnPlayCommand( IDM_Stop_Sequence );
  }
  (reinterpret_cast<CMainFrame*>( m_pMainWnd ))->FrameHasChanged( FALSE );
  return retValue;
}

