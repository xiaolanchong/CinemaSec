// CSExchangeTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CSExchangeTest.h"
#include "CSExchangeTestDlg.h"
#include <initguid.h>
#include "CSExchangeTest_i.c"
#include ".\csexchangetest.h"

#include "InputMessageDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCSExchangeTestApp


class CCSExchangeTestModule :
	public CAtlMfcModule
{
public:
	DECLARE_LIBID(LIBID_CSExchangeTestLib);
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_CSEXCHANGETEST, "{1C963D24-1396-4D67-B503-50014501F644}");};

CCSExchangeTestModule _AtlModule;

BEGIN_MESSAGE_MAP(CCSExchangeTestApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCSExchangeTestApp construction

CCSExchangeTestApp::CCSExchangeTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CCSExchangeTestApp object

CCSExchangeTestApp theApp;


// CCSExchangeTestApp initialization

BOOL CCSExchangeTestApp::InitInstance()
{
	AfxOleInit();
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Register class factories via CoRegisterClassObject().
	if (FAILED(_AtlModule.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE)))
		return FALSE;
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

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CPropertySheet MainSheet;
	CCSExchangeTestDlg		dlgOutput;
	CInputMessageDialog		dlgInput;

	MainSheet.AddPage( &dlgOutput);
	MainSheet.AddPage( &dlgInput);
	MainSheet.m_psh.dwFlags		|=  PSH_USEHICON| PSH_PROPTITLE;
	MainSheet.m_psh.hIcon		= LoadIcon( IDR_MAINFRAME );
	MainSheet.m_psh.pszCaption	= MAKEINTRESOURCE( IDS_TITLE );
	INT_PTR nResponse = MainSheet.DoModal();	
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CCSExchangeTestApp::ExitInstance(void)
{
	_AtlModule.RevokeClassObjects();
	return CWinApp::ExitInstance();
}
