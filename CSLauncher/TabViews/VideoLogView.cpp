// TabViews\VideoLogView.cpp : implementation file
//

#include "stdafx.h"
#include "../CSLauncher.h"
#include "../LauncherDoc.h"
#include "VideoLogView.h"


// CVideoLogView

IMPLEMENT_DYNCREATE(CVideoLogView, CSimpleLog)

CVideoLogView::CVideoLogView()
{
}

CVideoLogView::~CVideoLogView()
{
}

BEGIN_MESSAGE_MAP(CVideoLogView, CSimpleLog)
END_MESSAGE_MAP()


// CVideoLogView drawing


// CVideoLogView diagnostics

#ifdef _DEBUG
void CVideoLogView::AssertValid() const
{
	CSimpleLog::AssertValid();
}

void CVideoLogView::Dump(CDumpContext& dc) const
{
	CSimpleLog::Dump(dc);
}
#endif //_DEBUG

// CVideoLogView message handlers

bool	CVideoLogView::OnReceiveMessage( int nSeverity, LPCWSTR szMessage )
{
	CLauncherDoc* pDoc = GetDocument();
	pDoc->LogVideoMessage( nSeverity, szMessage );
	return CSimpleLog::OnReceiveMessage( nSeverity, szMessage );
}