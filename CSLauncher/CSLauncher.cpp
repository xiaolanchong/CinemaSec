// CSLauncher.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CSLauncher.h"
#include "MainFrm.h"

#include "LauncherDoc.h"
#include "TabViews/LauncherView.h"
#include "public/ModuleVersion.h"

#ifndef FreeAddrInfo 
#define FreeAddrInfo(x)
#endif

class CCSLauncherModule :
	public CAtlMfcModule
{
public:
	DECLARE_LIBID(LIBID_CSLauncherLib);
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_CSLAUNCHER, "{B55FE7EA-70C3-460F-ADF7-04731D8F16EE}");};

CCSLauncherModule _AtlModule;


#ifndef GetAddrInfo 
inline  GetAddrInfo( LPCTSTR , LPCTSTR , void* ,  void* )
{
	return 0;
}
#endif

#include <afxsock.h>
#include <initguid.h>
#include "CSLauncher_i.c"
#include ".\cslauncher.h"
#include "Exchange\Exchange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLauncherApp

BEGIN_MESSAGE_MAP(CLauncherApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CLauncherApp construction

CLauncherApp::CLauncherApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CLauncherApp object

CLauncherApp theApp;

// CLauncherApp initialization

BOOL CLauncherApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	AfxInitRichEdit2();
	if(!AfxSocketInit())
	{
		AfxMessageBox( _T("Failed to Initialize Sockets"),MB_OK| MB_ICONSTOP);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("ElVEES"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CLauncherDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CLauncherView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// run create interface
	// ME
	CreateExchange();
	//
	// Register class factories via CoRegisterClassObject().
#if 1
	if (FAILED(_AtlModule.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE)))
		return FALSE;
#endif
	// App was launched with /Embedding or /Automation switch.
	// Run app as automation server.
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Don't show the main window
		return TRUE;
	}
	// App was launched with /Unregserver or /Unregister switch.
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		_AtlModule.UpdateRegistryAppId(FALSE);
		_AtlModule.UnregisterServer(TRUE);
		return FALSE;
	}
	// App was launched with /Register or /Regserver switch.
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister)
	{
		_AtlModule.UpdateRegistryAppId(TRUE);
		_AtlModule.RegisterServer(TRUE);
		return FALSE;
	}
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void	SetExeVersion(UINT nResID);
	void	SetDllVersion(LPCTSTR szName, UINT nResID);
// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	SetExeVersion(IDC_STATIC_LAUNCHER_VERSION);	
	SetDllVersion( _T("cschair.dll"), IDC_STATIC_SERVER_VERSION);	
	SetDllVersion( _T("csalgo.dll"), IDC_STATIC_ALGO_VERSION);
	SetDllVersion( _T("csutility.dll"), IDC_STATIC_UTILS_VERSION);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void	CAboutDlg::SetExeVersion(UINT nResID)
{
	CModuleVersion Ver;
	TCHAR buf[MAX_PATH];
	GetModuleFileName( AfxGetInstanceHandle(), buf, MAX_PATH ) ;

	BOOL Res = Ver.GetFileVersionInfo(buf);
	ASSERT(Res);
	CString strVersion = Ver.GetValue(  _T("FileVersion"));

	strVersion.Replace(',', '.');
	strVersion.Remove(' ');
	SetDlgItemText(nResID, strVersion);	
}

void	CAboutDlg::SetDllVersion(LPCTSTR szName, UINT nResID)
{
	CModuleVersion Ver;
	BOOL Res = Ver.GetFileVersionInfo(szName);
	if(Res)
	{
		CString strVersion = Ver.GetValue(  _T("FileVersion"));
		strVersion.Replace(',', '.');
		strVersion.Remove(' ');
		SetDlgItemText(nResID, strVersion);	
	}
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CLauncherApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CLauncherApp message handlers


BOOL CLauncherApp::ExitInstance(void)
{
	_AtlModule.RevokeClassObjects();
	DestroyExchange();
#if 0	// hungup test
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if( !cmdInfo.m_bRunAutomated	&& 
		!cmdInfo.m_bRunEmbedded		&& 
		cmdInfo.m_nShellCommand != CCommandLineInfo::AppUnregister && 
		cmdInfo.m_nShellCommand != CCommandLineInfo::AppRegister)
		Sleep( 1000 * 60 * 60 );
#endif
	return CWinApp::ExitInstance();
}
