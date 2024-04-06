// CSGuard.cpp : 定義應用程式的類別行為。
//

#include "stdafx.h"
#include "CSGuard.h"
#include "CSGuardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCSGuardApp

BEGIN_MESSAGE_MAP(CCSGuardApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCSGuardApp 建構

CCSGuardApp::CCSGuardApp()
{
	// TODO: 在此加入建構程式碼，
	// 將所有重要的初始設定加入 InitInstance 中
}


// 僅有的一個 CCSGuardApp 物件

CCSGuardApp theApp;


// CCSGuardApp 初始設定

BOOL CCSGuardApp::InitInstance()
{
	// 假如應用程式資訊清單指定使用 ComCtl32.dll 6.0 (含) 以後版本
	// 以啟用視覺化樣式，則 Windows XP 需要 InitCommonControls()。否則的話，
	// 任何視窗的建立將失敗。
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

#if 0
	CCSGuardDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置於使用 [確定] 來停止使用對話方塊時
		// 處理的程式碼
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置於使用 [取消] 來停止使用對話方塊時
		// 處理的程式碼
	}
	return FALSE;
#else
	m_pdlg = std::auto_ptr<CCSGuardDlg>( new CCSGuardDlg() );
	m_pdlg->Create( CCSGuardDlg::IDD );
	m_pMainWnd = m_pdlg.get();
	//m_pMainWnd->ShowWindow( SW_SHOW );
	return TRUE;
/*	CCSGuardDlg* pdlg = new CCSGuardDlg() ;
	pdlg->Create( CCSGuardDlg::IDD );
	m_pMainWnd = pdlg;*/
	return TRUE;
#endif
	// 因為已經關閉對話方塊，傳回 FALSE，所以我們會結束應用程式，
	// 而非提示開始應用程式的訊息。
	
}
