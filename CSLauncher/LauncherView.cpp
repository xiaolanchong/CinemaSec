// LauncherView.cpp : implementation of the CLauncherView class
//

#include "stdafx.h"
#include "CSLauncher.h"

#include "LauncherDoc.h"
#include "LauncherView.h"
#include ".\launcherview.h"
#include "MainFrm.h"

#include "../CSChair/syscfg_int.h"
#include <atldbcli.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLauncherView
#define WM_DBGMESSAGE WM_USER + 0x0F
#define WM_INITCONFIG WM_USER + 0x10

IMPLEMENT_DYNCREATE(CLauncherView, CFormView)

BEGIN_MESSAGE_MAP(CLauncherView, CFormView)
	ON_WM_DESTROY()
	ON_MESSAGE( WM_DBGMESSAGE, OnDbgMessage )
	ON_MESSAGE( WM_INITCONFIG, OnInitConfig)
END_MESSAGE_MAP()

//#define  USE_FILE_CFG

// CLauncherView construction/destruction

void CBrowseEdit :: OnDropButton()
{
//	CFileDialog dlg( TRUE, 0, 0, 0, m_strFilter );
	CDataSource ds;
	HRESULT hr = ds.Open();
	if( hr == S_OK )
	{
		//SetWindowText( dlg.GetPathName() );
		BSTR sCS;
		ds.GetInitializationString(&sCS);
		m_pWnd->SendMessage( WM_INITCONFIG, (WPARAM)sCS );
		SysFreeString(sCS);
	}
}

CLauncherView::CLauncherView()
	: CFormView(CLauncherView::IDD),
		m_bInit(false)
{
	// TODO: add construction code here

}

CLauncherView::~CLauncherView()
{
}

void CLauncherView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LINENUMBER, m_edLineNumber);
	DDX_Control(pDX, IDC_SPIN_LINENUMBER, m_spinLineNumber);
}

BOOL CLauncherView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CLauncherView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	if( !m_bInit )
	{
		m_bInit = true;
		m_LayoutMgr.Attach( this );
		m_LayoutMgr.AddAllChildren( TRUE );

		ModifyStyleEx( WS_EX_CLIENTEDGE, 0 );
		ModifyStyle( WS_BORDER, 0 );

		//m_LayoutMgr.SetConstraint( IDC_DEBUGMESSAGE,  )

		m_wndBrowseEdit.SubclassDlgItem( IDC_EDIT_CONFIG, this );
		m_wndBrowseEdit.SetFilter(_T("Server config files(*.xml)|*.xml||"), this);
		m_wndDebugMessage.SubclassDlgItem( IDC_LIST_DEBUGMESSAGE, this );

		m_LayoutMgr.SetConstraint( IDC_LIST_DEBUGMESSAGE, OX_LMS_RIGHT, OX_LMT_SAME, -2 );
		m_LayoutMgr.SetConstraint( IDC_LIST_DEBUGMESSAGE, OX_LMS_BOTTOM, OX_LMT_SAME, -2 );
		m_LayoutMgr.SetConstraint( IDC_EDIT_CONFIG, OX_LMS_RIGHT, OX_LMT_SAME, -2 );

		m_spinLineNumber.SetBuddy( &m_edLineNumber );
		m_spinLineNumber.SetRange32( 10, 1000 );
		m_spinLineNumber.SetPos( 100 );

		m_LayoutMgr.RedrawLayout();

		

		//FIXME: put to resources
		LVCOLUMN lvc;
		ZeroMemory( &lvc, sizeof(LVCOLUMN) );
		lvc.pszText = _T("Time");
		lvc.cx		= 100;
		lvc.mask	= LVCF_TEXT| LVCF_WIDTH;

		m_wndDebugMessage.SetColumn( 0, &lvc);
		//m_wndDebugMessage.InsertColumn( 0, _T("Time"), LVCFMT_LEFT, 100 );
		m_wndDebugMessage.InsertColumn( 1, _T("Message"), LVCFMT_LEFT, 400 );
		m_wndDebugMessage.ModifyExStyle( 0, TVOXS_ROWSEL );

		CLauncherDoc* pDoc = GetDocument();

		static bCreated = false;
		if( !bCreated )
		{
			bCreated = true;
			CCreateContext cc;
			memset(&cc, sizeof(cc), 0 );
			cc.m_pCurrentDoc	= pDoc;
			cc.m_pCurrentFrame	= GetParentFrame();
			//cc.m_pNewViewClass
			CameraMap_t cm;
			pDoc->CreateMainServer( cc, cm );
			CMainFrame * pFrame = STATIC_DOWNCAST( CMainFrame, GetParentFrame() );
			pFrame->SetTab( cm );
		}

		BOOL bRes = pDoc->InitServer( GetRoomID(), this ); 
		if( !bRes )
		{
			PrintW( IDebugOutput::mt_error, L"Failed to initialize server" );
		}
	}

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	SetScrollSizes( MM_TEXT, CSize(1,1) );
}


// CLauncherView diagnostics

#ifdef _DEBUG
void CLauncherView::AssertValid() const
{
	CFormView::AssertValid();
}

void CLauncherView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CLauncherDoc* CLauncherView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLauncherDoc)));
	return (CLauncherDoc*)m_pDocument;
}
#endif //_DEBUG


// CLauncherView message handlers

void CLauncherView::OnDestroy()
{
	// TODO: Add your message handler code here and/or call default
/*	if( m_pMainServer) 
	{
		m_pMainServer->Stop(true);
		m_pMainServer->Release();
		m_pMainServer = 0;
	}*/
	CLauncherDoc* pDoc = GetDocument();
	pDoc->ShutdownServer(GetRoomID() );
	__super::OnDestroy();
}

void	CLauncherView::PrintW( __int32 mt, LPCWSTR szMessage/*, LPCWSTR, LPCWSTR, LPCWSTR */)
{
	cs.Lock();
	m_MsgBuffer.push_back( std::make_pair( mt, std::wstring(szMessage)  )  );
	PostMessageW( WM_DBGMESSAGE );
	cs.Unlock();
}

void	CLauncherView::PrintA( __int32 mt, LPCSTR szMessage/*, LPCSTR, LPCSTR, LPCSTR */)
{
	//	cs.Lock();
	USES_CONVERSION;
	LPCWSTR szNewMsg = A2CW ( szMessage );
	PrintW( mt, szNewMsg );
	//	cs.Unlock();
}

LRESULT CLauncherView::OnDbgMessage( WPARAM, LPARAM )
{
	CLauncherDoc* pDoc = GetDocument();

	cs.Lock();
	int nCount = m_spinLineNumber.GetPos();
	if( !nCount)
	{
		m_wndDebugMessage.DeleteAllItems();
		m_MsgBuffer.clear();
		cs.Unlock();
		return TRUE;
	}
	if( m_wndDebugMessage.GetItemCount( ) > nCount )
	{
		for( UINT i = 0; i < m_wndDebugMessage.GetCount() - nCount; ++i )
		{
			HTREEITEM hti = m_wndDebugMessage.GetItemFromIndex( 0 );
			m_wndDebugMessage.DeleteItem( hti );
		}
	}
	
	MsgBuffer_t::iterator it = m_MsgBuffer.begin();
	for( ; it != m_MsgBuffer.end(); ++it )
	{
		unsigned int mt		= it->first;
		LPCWSTR szMessage	= it->second.c_str();

		COLORREF clMsg;
		CString s;
		LPCWSTR szSeverity;
		switch(mt)
		{
		case IDebugOutput::mt_debug_info :	clMsg = RGB(192, 192, 192); szSeverity = _T("dbg "); break;
		case IDebugOutput::mt_info :		clMsg = RGB(128, 128, 128); szSeverity = _T("info");break;
		case IDebugOutput::mt_warning :		clMsg = RGB(216, 216, 0); szSeverity = _T("warn");break;
		case IDebugOutput::mt_error :		clMsg = RGB(255, 0, 0); szSeverity = _T("err ");break;
		default :							clMsg = RGB( 0, 0, 255 ); szSeverity = _T("unk ");;
		}
		CString strTime = COleDateTime::GetCurrentTime().Format( _T("%b%d %H:%M:%S") );

		pDoc->LogMessage( GetRoomID(), mt, szMessage );

		HTREEITEM hItem = m_wndDebugMessage.InsertItem( strTime );
		m_wndDebugMessage.SetItemColor( hItem, clMsg, 0 );
		m_wndDebugMessage.SetSubItem( hItem, 1, OX_SUBITEM_COLOR | OX_SUBITEM_TEXT, szMessage, 0, 0,  clMsg );
	}
	m_MsgBuffer.clear();
	cs.Unlock();
	return TRUE;
}

LRESULT CLauncherView::OnInitConfig( WPARAM wp, LPARAM lp)
{
	return TRUE;
}

BOOL	CLauncherView::Create( CWnd* pParent, UINT nID, CCreateContext& cc )
{
	return CFormView::Create( 0, 0, WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), pParent, nID, &cc);
}