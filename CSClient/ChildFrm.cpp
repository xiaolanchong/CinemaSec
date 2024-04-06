// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "CSClient.h"

#include "ChildFrm.h"
#include "MainFrm.h"

#include "afxpriv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

//	cs.style &= ~(WS_MAXIMIZEBOX|WS_MINIMIZEBOX|WS_CAPTION);
//	cs.style |= WS_MAXIMIZE;
	cs.style &= ~FWS_ADDTOTITLE;
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	MDIMaximize();

//	CWnd* pWnd = GetParent();
	
/*	ASSERT_KINDOF  ( CMainFrame, pWnd  );

	CMainFrame* pFrame = (CMainFrame*)pWnd;
	pFrame->AddChildFrame( this );*/
	::SendMessage( AfxGetMainWnd()->m_hWnd, WM_ADDCHILD, (WPARAM)GetSafeHwnd(), 0 );
	return 0;
}

void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
/*	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle)
	{
		TCHAR szText[256+_MAX_PATH];
		if (pDocument == NULL)
			lstrcpy(szText, m_strTitle);
		else
			lstrcpy(szText, pDocument->GetTitle());
		CString str(szText );
		str.Delete( str.GetLength() - 1, 1 );
		AfxSetWindowText(m_hWnd, str);
	}*/
	__super::OnUpdateFrameTitle(bAddToTitle);
}