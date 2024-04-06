// VideoFrame.cpp : implementation file
//

#include "stdafx.h"
#include "ReportFrame.h"
#include "ReportParamView.h"
#include "ReportView.h"
#include <afxpriv.h>
// CReportFrame

IMPLEMENT_DYNCREATE(CReportFrame, CChildFrame)

CReportFrame::CReportFrame()
{
}

CReportFrame::~CReportFrame()
{
}


BEGIN_MESSAGE_MAP(CReportFrame, CChildFrame)
//	ON_WM_SIZE()
END_MESSAGE_MAP()

// CReportFrame message handlers
static bool init = false;
BOOL CReportFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	UNREFERENCED_PARAMETER( lpcs );
	m_wndSplitter.CreateStatic( this, 1, 2 );
	m_wndSplitter.CreateView( 0, 1, RUNTIME_CLASS( CReportView ), CSize(600, 0), pContext );
	m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS( CReportParamView ), CSize(200, 0), pContext );
	return TRUE;//CChildFrame::OnCreateClient(lpcs, pContext);
}
