// ArchiveContainer.cpp : implementation file
//

#include "stdafx.h"
#include "CSClient.h"
#include "ArchiveContainer.h"
#include "archive_mode_doc.h"
#include "archive_mode_view.h"
#include "VideoControlView.h"
#include <afxpriv.h>

// CArchiveContainerView
#define ID_DIALOG_BAR	0x100

IMPLEMENT_DYNCREATE(CArchiveContainerView, CView)

CArchiveContainerView::CArchiveContainerView() : m_bInitialized(false)
{

}

CArchiveContainerView::~CArchiveContainerView()
{
}

BEGIN_MESSAGE_MAP(CArchiveContainerView, CView)
	//{{AFX_MSG_MAP(CArchiveView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_UPDATE_COMMAND_UI(ID_DIALOG_BAR, OnBarUpdate)
	ON_UPDATE_COMMAND_UI(IDC_STATIC_BAR, OnBarUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CArchiveContainerView drawing

void CArchiveContainerView::OnDraw(CDC* pDC)
{
	//CDocument* pDoc = GetDocument();
	// TODO: add draw code here
	UNREFERENCED_PARAMETER(pDC);
}


// CArchiveContainerView diagnostics

#ifdef _DEBUG
void CArchiveContainerView::AssertValid() const
{
	CView::AssertValid();
}

void CArchiveContainerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CArchiveContainerView message handlers

void CArchiveContainerView::OnInitialUpdate( )
{
	if(!m_bInitialized )
	{
		m_bInitialized = true;

		ModifyStyleEx(  WS_EX_CLIENTEDGE, 0 );
		ModifyStyle(  WS_BORDER, WS_CLIPCHILDREN|WS_CLIPSIBLINGS );

		CCreateContext cc;
		memset( &cc, 0, sizeof(cc));
		cc.m_pCurrentFrame	= GetParentFrame();
		cc.m_pCurrentDoc	= GetDocument();
		cc.m_pNewViewClass	= RUNTIME_CLASS(  CArchiveView );
		m_pView = (CArchiveView*)RUNTIME_CLASS(CArchiveView)->CreateObject();
		m_pView->Create( NULL,NULL,WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN, CRect(0,0,0,0), this,  1 /*AFX_IDW_PANE_FIRST*/, &cc);

		m_pView->SendMessage(WM_INITIALUPDATE);

		memset( &cc, 0, sizeof(cc));
		cc.m_pCurrentFrame	= GetParentFrame();
		cc.m_pCurrentDoc	= GetDocument();
		cc.m_pNewViewClass	= RUNTIME_CLASS(  CVideoControlView );
		m_pControlView = (CVideoControlView*)RUNTIME_CLASS(CVideoControlView)->CreateObject();
		m_pControlView->Create(  this, CRect(0,0,0,0), 2 /*AFX_IDW_PANE_FIRST*/, cc);

		m_pControlView->SendMessage(WM_INITIALUPDATE);

		m_LayoutManager.Attach( this);
		m_LayoutManager.AddAllChildren();

		m_LayoutManager.SetConstraint(m_pControlView,		OX_LMS_RIGHT, OX_LMT_SAME, 0);
		m_LayoutManager.SetConstraint(m_pControlView,		OX_LMS_BOTTOM,OX_LMT_SAME, 0);

		CSize size = m_pControlView->GetTotalSize();
		m_pControlView->SetScrollSizes( MM_TEXT, CSize(1, 1));
		m_LayoutManager.SetConstraint( m_pControlView, OX_LMS_TOP, OX_LMT_OPPOSITE, - ( size.cy + 1) );
		
		m_LayoutManager.SetConstraint(m_pView,		OX_LMS_TOP, OX_LMT_SAME, 0);
		m_LayoutManager.SetConstraint(m_pView,		OX_LMS_LEFT, OX_LMT_SAME, 0);
		m_LayoutManager.SetConstraint(m_pView,		OX_LMS_RIGHT, OX_LMT_SAME, 0);
		m_LayoutManager.SetConstraint(m_pView,		OX_LMS_BOTTOM,OX_LMT_OPPOSITE, 0, m_pControlView);
		
		m_LayoutManager.RedrawLayout();
	}
	CView::OnInitialUpdate();
}

void CArchiveContainerView::OnSize( UINT nType, int cx, int cy )
{
#if 0
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, AFX_IDW_PANE_FIRST);
	if( IsWindow(m_wndVideoBar.GetSafeHwnd()))
	{
		m_wndVideoBar.SizeSeeker(cx, 10);
		CRect rcSlider;
		m_wndVideoBar.GetSlider()->GetWindowRect(&rcSlider);
		rcSlider.InflateRect( 10, 0, 10, 0 );
		m_wndDialogBar.ScreenToClient( &rcSlider );
		CRect rcRange;
		m_wndRange.GetWindowRect(&rcRange);
		m_wndDialogBar.ScreenToClient( &rcRange );
		CWnd* pWnd = m_wndDialogBar.GetDlgItem( IDC_BUTTON_SAVE );
		ASSERT(pWnd);
		
		m_wndRange.SetWindowPos( 0, rcSlider.left, rcRange.top, rcSlider.Width(), rcRange.Height(), SWP_NOZORDER );
		pWnd->SetWindowPos( 0, 3, rcRange.top, rcSlider.left - 6 , rcRange.Height() , SWP_NOZORDER);
	}
#endif
	CView::OnSize(  nType, cx, cy);
}

BOOL CArchiveContainerView::OnEraseBkgnd(CDC* pDC) 
{ 
	UNREFERENCED_PARAMETER(pDC);
	return TRUE;
};

void CArchiveContainerView::OnBarUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}