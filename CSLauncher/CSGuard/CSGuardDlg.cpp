// CSGuardDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "CSGuard.h"
#include "CSGuardDlg.h"
#include "Inspector\Launcher.h"
#include "../../CSClient/ModuleVersion.h"
#include <shlwapi.h>
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const UINT WM_TASKBARCREATED	= ::RegisterWindowMessage(_T("TaskbarCreated"));
const UINT WM_EXIT_GUARD		= ::RegisterWindowMessage(_T("ExitGuard"));


// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 對話方塊資料
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	virtual BOOL OnInitDialog();

// 程式碼實作
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

static LPCTSTR	c_szModuleName[] = 
{
	_T("chcsva.dll"),
	_T("csutility.dll"),
	_T("csalgo.dll"),
	_T("cschair.dll"),
	_T("csengine.dll"),
	_T("CSBackUp.dll"),
	_T("CVMCadComponent.dll"),
	_T("csclient.exe"),
	_T("cslauncher.exe"),
	_T("csguard.exe")
};

BOOL CAboutDlg::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();
	
	const int Col_Name		= 0;
	const int Col_Version	= 1;
	const int Col_Modified	= 2;
	CListCtrl* m_pwndVersion = (CListCtrl*)GetDlgItem( IDC_LIST_VERSION );
	ASSERT(m_pwndVersion);

	CString sNameHdr, sVersionHdr, sModifiedHdr;
	sNameHdr.LoadString(  IDS_MODULE );
	sVersionHdr.LoadString( IDS_VERSION  );
	sModifiedHdr.LoadString( IDS_MODIFIED  );

	LV_COLUMN lvc;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx	= 150;
	lvc.pszText = (LPTSTR)(LPCTSTR)sNameHdr;
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	m_pwndVersion->InsertColumn( Col_Name, &lvc );

	m_pwndVersion->InsertColumn( Col_Version,	sVersionHdr, LVCFMT_LEFT, 100 );
	m_pwndVersion->InsertColumn( Col_Modified,	sModifiedHdr, LVCFMT_LEFT, 100 );

	TCHAR szDirectory[MAX_PATH];
	GetModuleFileName(  AfxGetInstanceHandle() , szDirectory, MAX_PATH );
	PathRemoveFileSpec( szDirectory );

	for( size_t i = 0; i < sizeof(c_szModuleName)/sizeof(c_szModuleName[0]) ; ++i)
	{
		int nCount = m_pwndVersion->GetItemCount();
		int nIndex = m_pwndVersion->InsertItem( nCount, c_szModuleName[i] );
		CString sVersion;
		sVersion.LoadString( IDS_NOT_FOUND );
		TCHAR szFullPath[MAX_PATH];
		PathCombine( szFullPath, szDirectory, c_szModuleName[i] );
		CModuleVersion Ver;
		BOOL Res = Ver.GetFileVersionInfoFromFullPath( szFullPath );
		if( Res )
		{
			sVersion = Ver.GetValue(  _T("FileVersion"));
			sVersion.Replace(',', '.');
			sVersion.Remove(' ');
			m_pwndVersion->SetItemText(nIndex, Col_Version, sVersion );
		}
		
		WIN32_FILE_ATTRIBUTE_DATA fa;
		Res = GetFileAttributesEx( szFullPath, GetFileExInfoStandard, &fa );
		if( Res )
		{
#if 1
			CTime time( fa.ftLastWriteTime );
			CString sModified = time.Format( _T("%d.%m.%y %H:%M") );
#else
			CString sModified;
			TCHAR szBuf[MAX_PATH];
			DWORD dwFlags = FDTF_SHORTDATE | FDTF_SHORTTIME | FDTF_RELATIVE;
			if( SHFormatDateTime( fa.ftLastWriteTime, &dwFlags, szBuf, 255 ) )
			{	
				sModified = szBuf;
			}
#endif
			m_pwndVersion->SetItemText(nIndex, Col_Modified, sModified );
		}
	}
	
	return res;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCSGuardDlg 對話方塊



CCSGuardDlg::CCSGuardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCSGuardDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCSGuardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCSGuardDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE( WM_NOTIFY_TASKBAR_ICON, OnTaskbarMsg )
	ON_REGISTERED_MESSAGE(WM_TASKBARCREATED,	OnTaskBarCreated)
	ON_REGISTERED_MESSAGE(WM_EXIT_GUARD,		OnExitGuard	)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CCSGuardDlg

BOOL CCSGuardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// 
	SetIcon(m_hIcon, FALSE);		// 

	InitTaskbar();

//	HANDLE hSyncEvent = m_SyncEvent.m_hObject;
	m_hThread = (HANDLE)::_beginthreadex( 0, 0, LauncherProc, NULL, 0, NULL  );

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void	CCSGuardDlg::InitTaskbar()
{
	m_pwndTaskbar = std::auto_ptr<COXTaskbarIcon>( new COXTaskbarIcon );

	m_pwndTaskbar->Create();
	m_pwndTaskbar->SetTooltipText(_T("CSGuard"));
	m_pwndTaskbar->SetIcon(IDR_MAINFRAME);
	m_pwndTaskbar->SetOwner(this);
	m_pwndTaskbar->Show();
}

void CCSGuardDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，以便繪製圖示。
// 對於使用文件/檢視模式的 MFC 應用程式，框架會自動完成此作業。

void CCSGuardDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//當使用者拖曳最小化視窗時，系統呼叫這個功能取得游標顯示。
HCURSOR CCSGuardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT	CCSGuardDlg::OnTaskbarMsg( WPARAM , LPARAM msg )
{
	switch( msg )
	{
	case WM_RBUTTONDOWN:
		HandleMenu();
		break;
	}
	return 0;
}

void	CCSGuardDlg::HandleMenu()
{
	const UINT	c_Exit		= 1;
	const UINT	c_Show		= 2;
	const UINT	c_Hide		= 3;
	const UINT	c_Restart	= 4;
	const UINT	c_About		= 5;
	CMenu menu;
	CString sItem;
	menu.CreatePopupMenu();
	sItem.LoadString( IDS_CMD_RESTART );
	menu.AppendMenu( MF_STRING|MF_BYCOMMAND, c_Restart,	sItem );
	sItem.LoadString( IDS_CMD_SHOW );
	menu.AppendMenu( MF_STRING|MF_BYCOMMAND, c_Show,	sItem );
	sItem.LoadString( IDS_CMD_HIDE );
	menu.AppendMenu( MF_STRING|MF_BYCOMMAND, c_Hide,	sItem );
	sItem.LoadString( IDS_CMD_ABOUT );
	menu.AppendMenu( MF_STRING|MF_BYCOMMAND, c_About,	sItem );
	sItem.LoadString( IDS_CMD_EXIT );
	menu.AppendMenu( MF_STRING|MF_BYCOMMAND, c_Exit,	sItem );
	CPoint pt;
	GetCursorPos(&pt);
	SetForegroundWindow();
	BOOL bRes = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON|	TPM_RETURNCMD |TPM_NONOTIFY, pt.x, pt.y,  this );

	switch(bRes )
	{
	case c_About:	
		{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
		}
		break;
	case c_Exit:
		OnExit();
		break;
	case c_Show:
		GetMessagePipe().SendMessage( MessagePipe::MsgShow ); break;
	case c_Hide:
		GetMessagePipe().SendMessage( MessagePipe::MsgHide ); break;
	case c_Restart:
		GetMessagePipe().SendMessage( MessagePipe::MsgRestart ); break;
	}
}

LRESULT CCSGuardDlg::OnTaskBarCreated(WPARAM wp, LPARAM lp)
{
	InitTaskbar();
	return 0;
}


void	CCSGuardDlg::OnExit()
{
	const DWORD c_WaitWorkingThread = 20 * 1000; // > c_WaitOrKillProcess
	GetMessagePipe().SendMessage( MessagePipe::MsgExit );
	DWORD dwRes  = WaitForSingleObject( m_hThread, c_WaitWorkingThread );
	if( dwRes == WAIT_OBJECT_0 )
	{
		// normal
		TRACE0( "Thread exits normally" );
	}
	else
	{
		TerminateThread( m_hThread, DWORD(-1) );
		TRACE0( "Thread was terminated" );
	}
	try
	{
	CloseHandle( m_hThread );
	}
	catch(...)
	{
	}

	PostQuitMessage(0);
}

LRESULT CCSGuardDlg::OnExitGuard(WPARAM wp, LPARAM lp)
{
	OnExit();
	return 0;
}