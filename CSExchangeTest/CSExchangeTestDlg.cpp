// CSExchangeTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CSExchangeTest.h"
#include "CSExchangeTestDlg.h"
#include ".\csexchangetestdlg.h"
#include "..\CSLauncher\CSLauncher_i.h"
#include "..\CSLauncher\CSLauncher_i.c"

#include "BrowseForFolder/XSBrowseFolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public ParentClass
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : ParentClass(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	ParentClass::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, ParentClass)
END_MESSAGE_MAP()


// CCSExchangeTestDlg dialog



CCSExchangeTestDlg::CCSExchangeTestDlg(CWnd* pParent /*=NULL*/)
	: ParentClass(CCSExchangeTestDlg::IDD, IDS_OUTPUT_MESSAGE ),
		m_Desc( NULL, 0 )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCSExchangeTestDlg::DoDataExchange(CDataExchange* pDX)
{
	ParentClass::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCSExchangeTestDlg, ParentClass)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnLoad)
	ON_BN_CLICKED(IDC_BUTTON_INPUTPATH,		OnBnInputPath)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUTPATH,	OnBnOutputPath)
END_MESSAGE_MAP()


// CCSExchangeTestDlg message handlers

BOOL CCSExchangeTestDlg::OnInitDialog()
{
	ParentClass::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CString s;
	s = AfxGetApp()->GetProfileString( _T("Settings"), _T("InputPath") );
	SetDlgItemText( IDC_EDIT_INPUTPATH, s );

	s = AfxGetApp()->GetProfileString( _T("Settings"), _T("InputName"), _T("query") );
	SetDlgItemText( IDC_EDIT_INPUTNAME, s );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCSExchangeTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		ParentClass::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCSExchangeTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		ParentClass::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCSExchangeTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#define AfxMessageBoxErr(x ) AfxMessageBox( x, MB_OK|MB_ICONERROR );

void CCSExchangeTestDlg::OnBnClickedButtonSend()
{
	// TODO: Add your control notification handler code here
	CString sText;
	GetDlgItemText( IDC_EDIT_MESSAGE, sText );

	CComPtr<ICCSRequest> pRequest;
	HRESULT hr = pRequest.CoCreateInstance( CLSID_CCSRequest );
	if( hr != S_OK )
	{
		AfxMessageBoxErr( _T("Failed to get ICCSRequest") );
		return;
	}
	CComBSTR sMsg( sText );
	hr = pRequest->Request( sMsg );
	std::wstring sDesc = m_Desc.GetErrorDesc( hr );
	SetDlgItemText( IDC_EDIT_RESULT, sDesc.c_str() );
	pRequest.Release();
}

void CCSExchangeTestDlg::OnBnSave()
{
	TCHAR szNewFile[MAX_PATH], szFullPath[MAX_PATH];
	CString s, sQueryName;
	GetDlgItemText( IDC_EDIT_INPUTPATH, s );
	GetDlgItemText( IDC_EDIT_INPUTNAME, sQueryName );

	AfxGetApp()->WriteProfileString( _T("Settings"), _T("InputName"), sQueryName );

	sQueryName += _T(".xml");
	PathMakeUniqueName( szNewFile, MAX_PATH, 0 , (LPCTSTR)sQueryName, s );
	PathCombine( szFullPath, s, szNewFile );
	LRESULT nSize = GetDlgItem( IDC_EDIT_MESSAGE )->SendMessage( WM_GETTEXTLENGTH );
	std::vector<char> Buf( nSize + 1, 0 );
	GetDlgItemTextA( GetSafeHwnd(), IDC_EDIT_MESSAGE, &Buf[0], nSize+1 );
	CFile XmlFile( szFullPath, CFile::modeCreate | CFile::modeWrite );
	XmlFile.Write( &Buf[0], (size_t)nSize );
}

void CCSExchangeTestDlg::OnBnLoad()
{

	LPCTSTR szFilter = _T("Query message files(*.xml)|*.xml|All files(*.*)|*.*||");
	CFileDialog dlg( TRUE, _T("xml"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING, szFilter, this  );
	if( dlg.DoModal() == IDOK )
	{
		CFile XmlFile( dlg.GetPathName(), CFile::modeRead );
		if( XmlFile.m_hFile == INVALID_HANDLE_VALUE )
		{
			AfxMessageBox( _T("Failed to open file"), MB_OK|MB_ICONERROR );
			return;
		}
		ULONGLONG nSize = XmlFile.GetLength();
		std::vector<char>	Buf( (size_t)nSize + 1, 0 );
		XmlFile.Read( &Buf[0], (size_t)nSize );
		SetDlgItemTextA( GetSafeHwnd(), IDC_EDIT_MESSAGE, &Buf[0] );
	}
}

void CCSExchangeTestDlg::OnBnInputPath()
{
	TCHAR Buf[MAX_PATH];
	CXSBrowseFolder dlg(BIF_NEWDIALOGSTYLE);
	CXSBrowseFolder::retCode ret = dlg.Show ( GetSafeHwnd(), Buf );
	if( CXSBrowseFolder::RET_OK == ret)
	{
		SetDlgItemText( IDC_EDIT_INPUTPATH, Buf );
		AfxGetApp()->WriteProfileString( _T("Settings"), _T("InputPath"), CString(Buf) );
	}
}

void CCSExchangeTestDlg::OnBnOutputPath()
{
	TCHAR Buf[MAX_PATH];
	CXSBrowseFolder dlg(BIF_NEWDIALOGSTYLE);
	CXSBrowseFolder::retCode ret = dlg.Show ( GetSafeHwnd(), Buf );
	if( CXSBrowseFolder::RET_OK == ret)
	{
		SetDlgItemText( IDC_EDIT_OUTPUTPATH, Buf );
	}
}