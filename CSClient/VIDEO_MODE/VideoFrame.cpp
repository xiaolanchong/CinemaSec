// VideoFrame.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "VideoFrame.h"
#include ".\videoframe.h"
#include "VideoStatView.h"
#include "VideoView.h"
#include <afxpriv.h>
// CVideoFrame

IMPLEMENT_DYNCREATE(CVideoFrame, CChildFrame)

CVideoFrame::CVideoFrame()
{
}

CVideoFrame::~CVideoFrame()
{
}


BEGIN_MESSAGE_MAP(CVideoFrame, CChildFrame)
	ON_WM_SIZE()
END_MESSAGE_MAP()

inline UINT IdFromRowCol( int x, int y)	
{
	return (0xd0000 | MAKEWORD(x&0xFF , y&0xFF) );
}

CView* CreateViewForSplitter( CWnd* pParent, int row, int col,  CRuntimeClass* pViewClass, CCreateContext* pContext)
{
	CSize sizeInit(100,100);
#ifdef _DEBUG
//	ASSERT_VALID(this);
	ASSERT(row >= 0 );
	ASSERT(col >= 0 );
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));

	if (pParent->GetDlgItem(IdFromRowCol(row, col)) != NULL)
	{
		TRACE(traceAppMsg, 0, "Error: CreateView - pane already exists for row %d, col %d.\n",
			row, col);
		ASSERT(FALSE);
		return FALSE;
	}

#endif

	// set the initial size for that pane

	BOOL bSendInitialUpdate = FALSE;

	CCreateContext contextT;
	if (pContext == NULL)
	{
		// if no context specified, generate one from the currently selected
		//  client if possible
#if 0
		CView* pOldView = (CView*)GetActivePane();
		if (pOldView != NULL && pOldView->IsKindOf(RUNTIME_CLASS(CView)))
		{
			// set info about last pane
			ASSERT(contextT.m_pCurrentFrame == NULL);
			contextT.m_pLastView = pOldView;
			contextT.m_pCurrentDoc = pOldView->GetDocument();
			if (contextT.m_pCurrentDoc != NULL)
				contextT.m_pNewDocTemplate =
				contextT.m_pCurrentDoc->GetDocTemplate();
		}
#endif
		pContext = &contextT;
		bSendInitialUpdate = TRUE;
	}

	CView* pWnd;
	TRY
	{
		pWnd = (CView*)pViewClass->CreateObject();
		if (pWnd == NULL)
			AfxThrowMemoryException();
	}
	CATCH_ALL(e)
	{
		TRACE(traceAppMsg, 0, "Out of memory creating a splitter pane.\n");
		// Note: DELETE_EXCEPTION(e) not required
		return FALSE;
	}
	END_CATCH_ALL

		ASSERT_KINDOF(CWnd, pWnd);
	ASSERT(pWnd->m_hWnd == NULL);       // not yet created

	DWORD dwStyle = AFX_WS_DEFAULT_VIEW & ~WS_BORDER;

	// Create with the right size (wrong position)
	CRect rect(CPoint(0,0), sizeInit);
	if (!pWnd->Create(NULL, NULL, dwStyle,
		rect, pParent, IdFromRowCol(row, col), pContext))
	{
		TRACE(traceAppMsg, 0, "Warning: couldn't create client pane for splitter.\n");
		// pWnd will be cleaned up by PostNcDestroy
		return FALSE;
	}
	ASSERT( (UINT) GetDlgCtrlID(pWnd->m_hWnd) == IdFromRowCol(row, col));

	// send initial notification message
	if (bSendInitialUpdate)
		pWnd->SendMessage(WM_INITIALUPDATE);

	return pWnd;
}

// CVideoFrame message handlers
static bool init = false;
BOOL CVideoFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	UNREFERENCED_PARAMETER( lpcs );
#ifdef USE_NEW_SPLITTER
	CRect rect(0, 0, 200, 200);
	CView* pLeft	= CreateViewForSplitter( this, 0, 0, RUNTIME_CLASS( CVideoView ), pContext );
	CView* pRight	= CreateViewForSplitter( this, 0, 1, RUNTIME_CLASS( CVideoStatView ), pContext );
	m_wndSplitter.Create(
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|SS_VERT, 
		this,              // the parent of the splitter pane
		pLeft,       // left pane
		pRight,       // right pane
		2, // this ID is used for saving/restoring splitter
		// position and therefore it must be unique 
		// within your application
		rect,              // dimensions of the splitter pane
		90,                // left constraint for splitter position
		110                // right constraint for splitter position
		);
#else
	m_wndSplitter.CreateStatic( this, 1, 2 );
	m_wndSplitter.CreateView( 0, 1, RUNTIME_CLASS( CVideoView ), CSize(600, 0), pContext );
	m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS( CVideoStatView ), CSize(200, 0), pContext );
	init = true;
#endif
	return TRUE;//CChildFrame::OnCreateClient(lpcs, pContext);
}

void CVideoFrame::OnSize(UINT nType, int cx, int cy)
{
	CChildFrame::OnSize(nType, cx, cy);
#ifdef USE_NEW_SPLITTER
	if( m_wndSplitter.GetSafeHwnd() )
	m_wndSplitter.SetWindowPos( 0, 0, 0, cx, cy, SWP_NOZORDER|SWP_NOMOVE | SWP_NOACTIVATE );
	//	m_wndSplitter.SendMessage( WM_SIZE, nType, MAKELONG(cx, cy));
#else
/*	if( m_wndSplitter.GetSafeHwnd() && init )
	{
		m_wndSplitter.SetColumnInfo( 1, 100, 00 );
		m_wndSplitter.RecalcLayout();
	}*/
#endif
}
