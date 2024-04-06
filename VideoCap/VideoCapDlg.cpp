// VideoCapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VideoCap.h"
#include "VideoCapDlg.h"
#include ".\videocapdlg.h"
#include "SettingsDialog.h"
//#include <boost/bind.hpp>
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const UINT Timer_Process	= 0xdf;
const UINT Period_Process	= 1000;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CVideoCapDlg dialog



CVideoCapDlg::CVideoCapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoCapDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoCapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CAMERA,	m_wndCameraTree);
	DDX_Control(pDX, IDC_STATIC_IMAGE,	m_wndImage);
#if 0
	DDX_Control(pDX, IDC_SPIN_FPS,		m_wndFPS);
	DDX_Control(pDX, IDC_COMBO_ENCODER,	m_wndEncoder);
#endif
}

BEGIN_MESSAGE_MAP(CVideoCapDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY( TVN_SELCHANGED, IDC_TREE_CAMERA, OnNotifyItemSel )
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_DIRECTORY, OnBnClickedButtonDirectory)
END_MESSAGE_MAP()


// CVideoCapDlg message handlers

BOOL CVideoCapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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
	try
	{
//		InitControls();
		CSettingsDialog dlg;
		int nFPS = CSettingsDialog::GetDefaultFPS();
		if( dlg.DoModal() == IDOK )
		{
			nFPS = dlg.GetFPS();
		}
		m_VideoCap.Create( nFPS);
		CreateTree();
		Initialize();
	}
	catch( CommonExceptionEx& ex )
	{
		m_wndCameraTree.DeleteAllItems();
		CString s( ex.whatW() );
		AfxMessageBox( s, MB_ICONERROR|MB_OK );
		return FALSE;
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVideoCapDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVideoCapDlg::OnPaint() 
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVideoCapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void	CVideoCapDlg::CreateTree()
{
	size_t nSize = m_VideoCap.GetCameraNumber();
	for( size_t i = 0; i < nSize; ++i )
	{
		CString s;
		s.Format( _T("camera %Id"), i + 1);

		HTREEITEM hti = m_wndCameraTree.InsertItem(s);
		m_wndCameraTree.SetItemData( hti, i );
	}
}

void	CVideoCapDlg::OnNotifyItemSel( NMHDR* pHdr, LRESULT * pResult )
{
	LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) pHdr; 
	if( pnmtv->itemNew.hItem )
	{
		size_t nIndex = pnmtv->itemNew.lParam;
		const BITMAPINFOHEADER* pbih;
		const BYTE* pByte;
		m_VideoCap.GetThumbView( nIndex, pbih, pByte );
		CString s; s.Format( _T("camera %Iu"), nIndex + 1);
		m_wndImage.SetImage( pbih, pByte, s );
	}
	else
	{
		m_wndImage.SetImage( 0, 0, CString());
	}
	m_wndImage.Invalidate();
	*pResult = 0;
}

void CVideoCapDlg::OnBnClickedButtonLoad()
try
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg( TRUE, _T("xml"), NULL, 
					 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING,
					 _T("XML files(*.xml)|*.xml|All files(*.*)|*.*||"));
	if( dlg.DoModal() == IDOK )
	{
		m_TimeLoader.Load( dlg.GetPathName() );
		SetDlgItemText( IDC_EDIT_FILE, dlg.GetPathName() );
	}
}
catch ( CommonExceptionEx& ex ) 
{
	AfxMessageBox( ex.whatW(), MB_OK|MB_ICONERROR );
};

void CVideoCapDlg::OnBnClickedButtonDirectory()
{
	// TODO: Add your control notification handler code here
}

void CVideoCapDlg::OnTimer( UINT_PTR nIDEvent )
{
	if( nIDEvent == Timer_Process )
	{
		HTREEITEM hti = m_wndCameraTree.GetNextItem( TVI_ROOT, TVGN_CHILD );
		for( size_t i = 0; i < m_CameraStates.size(); ++i, hti = m_wndCameraTree.GetNextItem( hti, TVGN_NEXT ) )
		{
			try
			{
				CameraState* pNewState = m_CameraStates[i]->Process( m_TimeLoader );
				if( pNewState )
				{
					m_CameraStates[i] = boost::shared_ptr<CameraState>( pNewState );
				}
				CString s = m_CameraStates[i]->GetState();
				m_wndCameraTree.SetItemText( hti, s );
			}
			catch( CommonExceptionEx& ex )
			{
				SetDlgItemText( IDC_EDIT_STATUS, ex.whatW() );
			}
		}
	}
	CDialog::OnTimer( nIDEvent );
}

void CVideoCapDlg::OnDestroy()
{
	for( size_t i = 0; i < m_CameraStates.size(); ++i )
	{
		m_CameraStates[i]->Terminate( );
	}	
	m_VideoCap.Destroy();
	CDialog::OnDestroy();
}

void CVideoCapDlg::Initialize()
{
	m_CameraStates.reserve( m_VideoCap.GetCameraNumber() );
	for( size_t i = 0; i < m_VideoCap.GetCameraNumber(); ++i )
	{
		m_CameraStates.push_back( boost::shared_ptr<CameraState> ( CameraState::GetDefaultState( i, &m_VideoCap ) ) );
	}
	SetTimer( Timer_Process, Period_Process, NULL );
}