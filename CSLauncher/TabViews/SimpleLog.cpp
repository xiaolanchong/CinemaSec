// SimpleLog.cpp : implementation file
//

#include "stdafx.h"
#include "../CSLauncher.h"
#include "SimpleLog.h"
#include "../LauncherDoc.h"

// CSimpleLog

IMPLEMENT_DYNCREATE(CSimpleLog, CView)

CSimpleLog::CSimpleLog() : m_bInit(false), m_dwShowFlags ( SHOW_ALL )
{
}

CSimpleLog::~CSimpleLog()
{
}

BEGIN_MESSAGE_MAP(CSimpleLog, CView)
	ON_WM_ERASEBKGND()
	ON_NOTIFY( NM_RCLICK, IDC_LIST_DEBUGMESSAGE, OnNotifyRClick ) 
END_MESSAGE_MAP()


// CSimpleLog drawing

void CSimpleLog::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CSimpleLog diagnostics

#ifdef _DEBUG
void CSimpleLog::AssertValid() const
{
	CView::AssertValid();
}

void CSimpleLog::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLauncherDoc* CSimpleLog::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLauncherDoc)));
	return (CLauncherDoc*)m_pDocument;
}
#endif //_DEBUG


// CSimpleLog message handlers

void CSimpleLog::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	if(  !m_bInit)
	{
		m_bInit = true;

		CRect rc(0, 0, 300, 300);
		GetClientRect(&rc);
		m_wndDebugMessage.Create(	LVS_REPORT | LVS_OWNERDRAWFIXED | WS_BORDER |WS_VISIBLE,
									rc, this, 
									IDC_LIST_DEBUGMESSAGE, 
									TVOXS_COLUMNHDR | TVOXS_ROWSEL | TVOXS_ITEMTIPS );
		m_LayoutMgr.Attach( this );
		m_LayoutMgr.AddAllChildren( TRUE );

		ModifyStyleEx( WS_EX_CLIENTEDGE, 0 );
		ModifyStyle( WS_BORDER, 0 );

		DebugOuputList::Attach( 0, this, this );

		m_LayoutMgr.SetConstraint( IDC_LIST_DEBUGMESSAGE, OX_LMS_RIGHT, OX_LMT_SAME, 0 );
		m_LayoutMgr.SetConstraint( IDC_LIST_DEBUGMESSAGE, OX_LMS_BOTTOM, OX_LMT_SAME, 0 );

		m_LayoutMgr.RedrawLayout();
	}
}

bool	CSimpleLog::OnReceiveMessage( int nSeverity, LPCWSTR szMessage )
{
	CLauncherDoc* pDoc = GetDocument();
#if 0
	return pDoc ? pDoc->ShowMessageType( nSeverity ) : true ;
#else
	return ShowMessageType( nSeverity );
#endif
}

int		CSimpleLog::GetMaxLineNumber()
{
	const int nCount = 100;
	CLauncherDoc* pDoc = GetDocument();
	return pDoc ? pDoc->GetMaxLineNumber() : nCount;
}

void CSimpleLog::OnNotifyRClick( NMHDR*, LRESULT * pResult )
{
	//__super::OnRButtonDown( nFlags, point );
	*pResult = 0;

	CMenu	menu;
	menu.CreatePopupMenu();
	AddString( menu, SHOW_DEBUG,	_T("Show debug info") );
	AddString( menu, SHOW_TRACE,	_T("Show trace info") );
	AddString( menu, SHOW_WARNING,	_T("Show warnings") );
	AddString( menu, SHOW_ERROR,	_T("Show errors") );

	CPoint pt;
	GetCursorPos(&pt);
	UINT nID = menu.TrackPopupMenuEx( TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this, NULL );
	if( nID )
	{
		m_dwShowFlags ^= nID;
	}
}

void	CSimpleLog::AddString( CMenu& menu, DWORD dwFlag,  LPCTSTR szItemName )
{
	menu.AppendMenu( MF_BYCOMMAND| (m_dwShowFlags & dwFlag ? MF_CHECKED : MF_UNCHECKED ), dwFlag, szItemName  );
}

bool	CSimpleLog::ShowMessageType( int nSeverity )
{
	switch( nSeverity )
	{
	case IDebugOutput::mt_debug_info	: return m_dwShowFlags & SHOW_DEBUG ? true : false;
	case IDebugOutput::mt_info			: return m_dwShowFlags & SHOW_TRACE ? true : false;
	case IDebugOutput::mt_warning		: return m_dwShowFlags & SHOW_WARNING ? true : false;
	case IDebugOutput::mt_error			: return m_dwShowFlags & SHOW_ERROR ? true : false;
	default								: return true;
	}
}