/****************************************************************************
  mainfrm.cpp
  ---------------------
  begin     : Aug 2004
  modified  : 24 Aug 2005
  author(s) : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "algo_debugger.h"
#include "mainfrm.h"
#include "cam_analyzer_mode/cam_analyzer_doc.h"
#include "cam_analyzer_mode/cam_analyzer_view.h"
#include "cam_analyzer_mode/cam_analyzer_frm.h"
#include "utility/document_iterator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
  ON_WM_CREATE()
  ON_WM_SYSCOMMAND()
  ON_COMMAND(IDM_File_OpenPlayList, OnFileOpenPlaylist)
END_MESSAGE_MAP()

static UINT indicators[] =
{
  ID_SEPARATOR,           // status line indicator
  INDICATOR_HUMAN_NUM
};

const wchar_t WANT_TO_SET_DEFAULT[] = L"Are you really going to set default parameters?";
const wchar_t NO_PARAM_FILE[] = L"Parameter file does not exist";

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CMainFrame::CMainFrame()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CMainFrame::~CMainFrame()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Called before the creation of the Windows window attached to this CWnd object. */
//-------------------------------------------------------------------------------------------------
BOOL CMainFrame::PreCreateWindow( CREATESTRUCT & cs )
{
  if (!CMDIFrameWnd::PreCreateWindow( cs ))
    return FALSE;

  cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE |
             WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

  return TRUE;
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when an application requests that the Windows
           window be created by calling the Create() or CreateEx() member function. */
//-------------------------------------------------------------------------------------------------
int CMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
  if (CMDIFrameWnd::OnCreate( lpCreateStruct ) == -1)
    return -1;

  // Create statusbar.
  {
    if (!m_wndStatusBar.Create( this ) ||
        !m_wndStatusBar.SetIndicators( indicators, sizeof(indicators)/sizeof(UINT) ))
    {
      TRACE( L"Failed to create status bar\n" );
      return -1;
    }

    CClientDC dc( &m_wndStatusBar );
    CRect     rect;
    UINT      flags = DT_CALCRECT | DT_LEFT | DT_NOCLIP | DT_SINGLELINE | DT_INTERNAL;

    rect.SetRect( 0, 0, 1, 1 );
    dc.DrawText( L"human: 0000", -1, &rect, flags );
    m_wndStatusBar.SetPaneInfo( 1, INDICATOR_HUMAN_NUM, SBPS_NORMAL, rect.Width() );
  }

  // Create toolbar.
  {
    if (!(m_wndToolBar.CreateEx( this, TBSTYLE_AUTOSIZE/*TBSTYLE_FLAT*/, WS_CHILD | WS_VISIBLE | CBRS_TOP |
                                 CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)) ||
        !(m_wndToolBar.LoadToolBar( IDR_MAINFRAME )) )
    {
      TRACE( L"Failed to create toolbar\n" );
      return -1;
    }

    // Delete these three lines if you don't want the toolbar to be dockable.
    m_wndToolBar.EnableDocking( CBRS_ALIGN_ANY );
    EnableDocking( CBRS_ALIGN_ANY );
    DockControlBar( &m_wndToolBar );
  }

  return 0;
}


//-------------------------------------------------------------------------------------------------
// CMainFrame diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
  CMDIFrameWnd::AssertValid();
}
void CMainFrame::Dump( CDumpContext & dc ) const
{
  CMDIFrameWnd::Dump( dc );
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief Implements IDM_File_OpenPlayList command. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::OnFileOpenPlaylist()
{
  CFileDialog dlg( TRUE, (WILDCARD_XML+2), 0, MY_OFN_FLAGS, XML_FILTER, this );
  if (dlg.DoModal() == IDOK)
  {
    CSPlayListParser parser;
    if (FAILED(parser.LoadPlayList( (LPCWSTR)(dlg.GetPathName()) )))
    {
      theApp.m_output.PrintW( IDebugOutput::mt_error, L"Failed to load playlist" );
      return;
    }

    CWaitCursor wait;
    theApp.CloseAllDocuments( FALSE );
    theApp.Clear();
    for (size_t i = 0; i < parser.size(); i++)
    {
      theApp.m_playListItem = parser[i];
/*
      // Remove camera index from chair file name.
      uint p1 = (uint)(theApp.m_playListItem.chairs.find( L"_cam" ));
      if (p1 != std::wstring::npos)
      {
        uint p2 = (uint)(theApp.m_playListItem.chairs.find_first_of( L'_', p1+1 ));
        if (p2 != std::wstring::npos)
          theApp.m_playListItem.chairs.erase( p1, p2-p1 );
        else
          theApp.m_output.PrintW( IDebugOutput::mt_error, L"Wrong chair file name" );
      }
*/
      theApp.OnFileNew();
    }
    theApp.m_playListItem = PlaylistItem();
    theApp.m_bPlaylist = true;
    MDITile( MDITILE_HORIZONTAL );
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief This method is called by the framework
           when the user selects a command from the Control menu. */
//-------------------------------------------------------------------------------------------------
void CMainFrame::OnSysCommand( UINT nID, LPARAM lParam )
{
  if ((nID == SC_CLOSE) && theApp.IsRunning())
    theApp.StopProcessing();
  CMDIFrameWnd::OnSysCommand( nID, lParam );
}


//-------------------------------------------------------------------------------------------------
/** \brief The framework calls this member function when the user selects an item from a menu,
           when a child control sends a notification message, or when an accelerator keystroke
           is translated. */
//-------------------------------------------------------------------------------------------------
BOOL CMainFrame::OnCommand( WPARAM wParam, LPARAM lParam )
{
  if (/*(lParam == 0) &&*/ (HIWORD(wParam) <= 1))
  {
    switch (LOWORD(wParam))
    {
      case IDM_Param_AlgoDialog:
      {
        theApp.m_ShowParameterDialogEx( GetSafeHwnd(), 0, true, &(theApp.m_output) );
        return TRUE;
      }

      case IDM_Param_AlgoDefault:
      {
        if (::AfxMessageBox( WANT_TO_SET_DEFAULT, MB_YESNO | MB_ICONQUESTION ) == IDYES)
          theApp.m_SetDefaultParametersEx( 0, true, &(theApp.m_output) );
        return TRUE;
      }

      case IDM_Param_AppDialog:
      {
        int algo = theApp.m_appParams.s_app.p_algoVersion();
        if (CParameterDialog( theApp.m_appParams, this, 320, 100 ).DoModal() == IDOK)
          csutility::SaveParameters( theApp.m_appParamFileName.c_str(), theApp.m_appParams, &(theApp.m_output) );
        if (algo != theApp.m_appParams.s_app.p_algoVersion())
          theApp.ReloadAlgorithmLibrary();
        return TRUE;
      }

      case IDM_Param_AppDefault:
      {
        int algo = theApp.m_appParams.s_app.p_algoVersion();
        if (::AfxMessageBox( WANT_TO_SET_DEFAULT, MB_YESNO | MB_ICONQUESTION ) == IDYES)
        {
          theApp.m_appParams.set_default();
          csutility::SaveParameters( theApp.m_appParamFileName.c_str(), theApp.m_appParams, &(theApp.m_output) );
        }
        if (algo != theApp.m_appParams.s_app.p_algoVersion())
          theApp.ReloadAlgorithmLibrary();
        return TRUE;
      }

      case IDM_Run_StopProcessing:
      {
        theApp.StopProcessing();
        return TRUE;
      }

      case IDM_Run_LaunchProcessing:
      {
        theApp.LaunchProcessing();
        return TRUE;
      }

      case IDM_Window_CloseAll:
      {
        theApp.CloseAllDocuments( FALSE );
        return TRUE;
      }
    }
  }
  return CMDIFrameWnd::OnCommand( wParam, lParam );
}


//-------------------------------------------------------------------------------------------------
/** \brief Called by the framework to route and dispatch command messages
           and to handle the update of command user-interface objects. */
//-------------------------------------------------------------------------------------------------
BOOL CMainFrame::OnCmdMsg( UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo )
{
  if ((nCode == CN_UPDATE_COMMAND_UI) && (pExtra != 0))
  {
    CCmdUI * pCmdUI = reinterpret_cast<CCmdUI*>( pExtra );

    switch (nID)
    {
      case IDM_Param_AlgoDialog:
      case IDM_Param_AlgoDefault:
      {
        pCmdUI->Enable( theApp.AreDataReady() && !(theApp.IsRunning()) );
        return TRUE;
      }

      case IDM_Param_AppDialog:
      case IDM_Param_AppDefault:
      case ID_FILE_OPEN:
      case IDM_File_OpenPlayList:
      {
        pCmdUI->Enable( !(theApp.IsRunning()) );
        return TRUE;
      }

      case IDM_Run_StopProcessing:
      {
        pCmdUI->Enable( theApp.AreDataReady() && theApp.IsRunning() );
        return TRUE;
      }

      case IDM_Run_LaunchProcessing:
      {
        pCmdUI->Enable( theApp.AreDataReady() && !(theApp.IsRunning()) );
        return TRUE;
      }

      case IDM_Window_CloseAll:
      {
        DocumentIterator<CCamAnalyzerDoc> iter( theApp.GetCamAnalyzerDocTempl() );
        pCmdUI->Enable( !(theApp.IsRunning()) && (iter.GetFirst() != 0) );
        return TRUE;
      }
    }
  }
  return CMDIFrameWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}


