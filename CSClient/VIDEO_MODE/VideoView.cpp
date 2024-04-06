// VideoView.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "VideoView.h"
#include "VideoDocument.h"
#include "DBFacet/DBSerializer.h"
//#include "DBFacet/CfgDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVideoView

IMPLEMENT_DYNCREATE(CVideoView, CArrangeView)

CVideoView::CVideoView()
{
}

CVideoView::~CVideoView()
{
}

BEGIN_MESSAGE_MAP(CVideoView, CArrangeView)
	//{{AFX_MSG_MAP(CVideoView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoView diagnostics

#ifdef _DEBUG
void CVideoView::AssertValid() const
{
	CArrangeView::AssertValid();
}

void CVideoView::Dump(CDumpContext& dc) const
{
	CArrangeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVideoView message handlers

CVideoDocument* CVideoView::GetDocument()
{
	return dynamic_cast<CVideoDocument*>( m_pDocument );
}

void CVideoView::OnInitialUpdate() 
{
	CArrangeView::OnInitialUpdate();
}

int CVideoView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CArrangeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}	