// TabViews\VideoLogView.cpp : implementation file
//

#include "stdafx.h"
#include "../CSLauncher.h"
#include "../LauncherDoc.h"
#include "ExchangeLogView.h"


// CVideoLogView

IMPLEMENT_DYNCREATE(CExchangeLogView, CSimpleLog)

CExchangeLogView::CExchangeLogView()
{
}

CExchangeLogView::~CExchangeLogView()
{
}

BEGIN_MESSAGE_MAP(CExchangeLogView, CSimpleLog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CExchangeLogView drawing


// CExchangeLogView diagnostics

#ifdef _DEBUG
void CExchangeLogView::AssertValid() const
{
	CSimpleLog::AssertValid();
}

void CExchangeLogView::Dump(CDumpContext& dc) const
{
	CSimpleLog::Dump(dc);
}
#endif //_DEBUG

// CVideoLogView message handlers

bool	CExchangeLogView::OnReceiveMessage( int nSeverity, LPCWSTR szMessage )
{
	CLauncherDoc* pDoc = GetDocument();
	pDoc->LogExchangeMessage( nSeverity, szMessage );
	return CSimpleLog::OnReceiveMessage( nSeverity, szMessage );
}

void CExchangeLogView::OnDestroy()
{
	CLauncherDoc* pDoc = GetDocument();
	pDoc->ShutdownExchange();
	__super::OnDestroy();
}