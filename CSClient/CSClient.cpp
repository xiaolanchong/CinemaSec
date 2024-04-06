// CSClient.cpp : Defines the class behaviors for the application.
//

#pragma warning(  disable : 4097 )
#pragma	  warning( disable : 4995 )		//deprecated functions

//#define TURN_ON_MODE

#include "stdafx.h"
#include "CSClient.h"

#include "MainFrm.h"
#include "CSClientDoc.h"
#include "CSClientView.h"
#include "ModuleVersion.h"

#include "DBFacet/CfgDB.h"

#define max _cpp_max
#define min _cpp_min

#include "Video_mode\VideoView.h"
#include "Video_mode\VideoDocument.h"
#include "Video_mode\VideoFrame.h"

#include "gauge_hall\gauge_hall_doc.h"
#include "gauge_hall\gauge_hall_view.h"
#include "gauge_hall\gauge_hall_frm.h"

#include "archive_mode/archive_mode_doc.h"
#include "archive_mode/archive_mode_view.h"
#include "archive_mode/archive_mode_frm.h"

#include "boxoffice_mode/boxoffice_mode_doc.h"
#include "boxoffice_mode/boxoffice_mode_view.h"
#include "boxoffice_mode/boxoffice_mode_frm.h"

#include "traindata_mode/train_data_frm.h"
#include "traindata_mode/train_data_doc.h"
#include "traindata_mode/train_data_view.h"

#include "Report_mode\ReportView.h"
#include "Report_mode\ReportDocument.h"
#include "Report_mode\ReportFrame.h"

#include "Diagnostic_mode\Diagnostic_mode_View.h"
#include "Diagnostic_mode\Diagnostic_mode_doc.h"
#include "Diagnostic_mode\Diagnostic_mode_frm.h"

#include "Timetable_mode\Timetable_mode_View.h"
#include "Timetable_mode\Timetable_mode_doc.h"
#include "Timetable_mode\Timetable_mode_frm.h"

#include "Settings_mode\Settings_mode_View.h"
#include "Settings_mode\Settings_mode_doc.h"
#include "Settings_mode\Settings_mode_frm.h"
#ifndef TURN_ON_MODE
#include "Log_mode\log_mode_View.h"
#include "Log_mode\log_mode_doc.h"
#include "Log_mode\log_mode_frm.h"
#endif

#include "DBFacet/DBSerializer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCSClientApp

BEGIN_MESSAGE_MAP(CCSClientApp, CWinApp)
	//{{AFX_MSG_MAP(CCSClientApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCSClientApp construction

CCSClientApp::CCSClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCSClientApp object

CCSClientApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CCSClientApp initialization

BOOL CCSClientApp::InitInstance()
{
	//AfxEnableControlContainer();

	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED, MB_OK|MB_ICONERROR);
		return FALSE;
	}
	AfxEnableControlContainer();

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("ElVEES"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

//	m_pDocManager = new CDocManagerEx;

	CMultiDocTemplate* pDocTemplate;

	// 1
	pDocTemplate = new CMultiDocTemplate(
		IDR_VIDEO_VIEW,
		RUNTIME_CLASS(CVideoDocument),
		RUNTIME_CLASS(CVideoFrame),      
		RUNTIME_CLASS(CVideoView));
	AddDocTemplate(pDocTemplate);
	// 2
	pDocTemplate = new CMultiDocTemplate(
		IDR_ArchiveMode,
		RUNTIME_CLASS(CArchiveDoc),
		RUNTIME_CLASS(CArchiveFrame),      
		RUNTIME_CLASS(CArchiveView));
	AddDocTemplate(pDocTemplate);
	// 3
	pDocTemplate = new CMultiDocTemplate(
		IDR_DIAGNOSTIC_VIEW,
		RUNTIME_CLASS(CDiagnosticDoc),
		RUNTIME_CLASS(CDiagnosticFrame),      
		RUNTIME_CLASS(CDiagnosticView));
	AddDocTemplate(pDocTemplate);
	// 4
	pDocTemplate = new CMultiDocTemplate(
		IDR_TIMETABLE_VIEW,
		RUNTIME_CLASS(CTimeTableDoc),
		RUNTIME_CLASS(CTimeTableFrame),      
		RUNTIME_CLASS(CTimeTableView));
	AddDocTemplate(pDocTemplate);
	// 5
	pDocTemplate = new CMultiDocTemplate(
		IDR_REPORT_VIEW,
		RUNTIME_CLASS(CReportDocument),
		RUNTIME_CLASS(CReportFrame),      
		RUNTIME_CLASS(CReportView));
	AddDocTemplate(pDocTemplate);
	// 6
	pDocTemplate = new CMultiDocTemplate(
		IDR_GrabMode,
		RUNTIME_CLASS(CSettingsDoc),
		RUNTIME_CLASS(CSettingsFrame),      
		RUNTIME_CLASS(CSettingsView));
	AddDocTemplate(pDocTemplate);
	// 7
	pDocTemplate = new CMultiDocTemplate(
		IDR_GaugeHallTYPE,
		RUNTIME_CLASS(CGaugeHallDoc),
		RUNTIME_CLASS(CGaugeHallFrame),      
		RUNTIME_CLASS(CGaugeHallView));
	AddDocTemplate(pDocTemplate);
	// 8
	pDocTemplate = new CMultiDocTemplate(
		IDR_BOXOFFICE_VIEW,
		RUNTIME_CLASS(CBoxOfficeDoc),
		RUNTIME_CLASS(CBoxOfficeFrame),      
		RUNTIME_CLASS(CBoxOfficeView));
	AddDocTemplate(pDocTemplate);
	// 9
	pDocTemplate = new CMultiDocTemplate(
		IDR_TrainData,
		RUNTIME_CLASS(CTrainDataDoc),
		RUNTIME_CLASS(CTrainDataFrame),      
		RUNTIME_CLASS(CTrainDataView));
	AddDocTemplate(pDocTemplate);
	// 10
#ifndef TURN_ON_MODE
	pDocTemplate = new CMultiDocTemplate(
		IDR_LOG_VIEW,
		RUNTIME_CLASS(CLogDoc),
		RUNTIME_CLASS(CLogFrame),      
		RUNTIME_CLASS(CLogView));
	AddDocTemplate(pDocTemplate);
#endif
	try
	{
		if( GetDB().IsConnected() )
		{
			GetDB().ReadData();
		}
		else
		{
			throw DataBaseException("Error");
		}
	}
	catch( CommonException )
	{
		if( GetCfgDB().UseDB() )
		{
			// FIXME : to resource
			AfxMessageBox( IDS_ERROR_CONNECTION, MB_OK|MB_ICONERROR );
			return FALSE;
		}
	}
	  // create main Frame window.
	  CMainFrame * pMainFrame = new CMainFrame;
	  m_pMainWnd = pMainFrame;
	  if (!pMainFrame || !(pMainFrame->LoadFrame( IDR_MAINFRAME )))
		return FALSE;

	  

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow(); 

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CCSClientApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CCSClientApp message handlers


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here


	CModuleVersion Ver;
	TCHAR buf[MAX_PATH];
	GetModuleFileName( AfxGetInstanceHandle(), buf, MAX_PATH ) ;

	BOOL Res = Ver.GetFileVersionInfo(buf);
	ASSERT(Res);
	CString strVersion = Ver.GetValue(  _T("FileVersion"));
	
	strVersion.Replace(',', '.');
	strVersion.Remove(' ');
	SetDlgItemText(IDC_STATIC_CSCLIENT_VERSION, strVersion);	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCSClientApp::OnFileNew() 
{
	// TODO: Add your command handler code here
	
}

//-------------------------------------------------------------------------------------------------
/** \brief Function performs idle-time processing. */
//-------------------------------------------------------------------------------------------------
BOOL CCSClientApp::OnIdle( LONG lCount )
{
  BOOL result = CWinApp::OnIdle( lCount );

  if (m_pMainWnd != 0)
  {
    ASSERT( m_pMainWnd->IsKindOf( RUNTIME_CLASS( CMainFrame ) ) );

    CMDIChildWnd * pChild = (reinterpret_cast<CMainFrame*>( m_pMainWnd ))->MDIGetActive();
    if (pChild != 0)
    {
      CDocument * pDoc = pChild->GetActiveDocument();
      if (pDoc != 0)
      {
        for (POSITION pos = pDoc->GetFirstViewPosition(); pos != 0;)
        {
          CView * pView = pDoc->GetNextView( pos );
          if ((pView != 0) && pView->IsKindOf( RUNTIME_CLASS( BaseIdleView ) ))
          {
            result = (result || (reinterpret_cast<BaseIdleView*>( pView ))->OnIdle());
          }
        }
      }
    }
  }
  return result;
}
