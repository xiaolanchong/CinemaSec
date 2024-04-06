// CSPlayList.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CSPlayList.h"
//#include "CSPlayListDlg.h"

#include "AlbertsAlgorithmsPage.h"
#include "SaveForDBViewerPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCSPlayListApp

BEGIN_MESSAGE_MAP(CCSPlayListApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCSPlayListApp construction

CCSPlayListApp::CCSPlayListApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CCSPlayListApp object

CCSPlayListApp theApp;


// CCSPlayListApp initialization

class mySheet : public CPropertySheet
{
	BOOL OnInitDialog()
	{
		CPropertySheet::OnInitDialog();
		CWnd* pWnd = GetDlgItem( IDOK );
		pWnd->ShowWindow(SW_HIDE);
		pWnd->EnableWindow( FALSE );
		return TRUE;

	}
};

BOOL CCSPlayListApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	class mySheet mySheet;


	mySheet.SetTitle(_T("Сохранение данных для обучения"));


	AlbertsAlgorithmsPage albertsPage;
	SaveForDBViewerPage saveForDBpage;

	mySheet.AddPage(&albertsPage);
	mySheet.AddPage(&saveForDBpage);

	mySheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	m_pMainWnd = &mySheet;
	INT_PTR nResponse = mySheet.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
