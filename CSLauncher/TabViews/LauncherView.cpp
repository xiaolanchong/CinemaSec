// LauncherView.cpp : implementation of the CLauncherView class
//

#include "stdafx.h"
#include "../CSLauncher.h"

#include "../LauncherDoc.h"
#include "LauncherView.h"
#include "../MainFrm.h"

#include <atldbcli.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLauncherView


IMPLEMENT_DYNCREATE(CLauncherView, CSimpleLog)

BEGIN_MESSAGE_MAP(CLauncherView, CSimpleLog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

CLauncherView::CLauncherView():
		m_bInit(false)
{
	// TODO: add construction code here

}

CLauncherView::~CLauncherView()
{
}

BOOL CLauncherView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CSimpleLog::PreCreateWindow(cs);
}

void CLauncherView::OnInitialUpdate()
{
	CSimpleLog::OnInitialUpdate();
	if( !m_bInit )
	{
		m_bInit = true;

		ModifyStyle( WS_BORDER, 0 );
		CLauncherDoc* pDoc = GetDocument();
		static bCreated = false;
		if( !bCreated )
		{
			bCreated = true;
			CCreateContext cc;
			memset(&cc, 0, sizeof(cc) );
			cc.m_pCurrentDoc	= pDoc;
			cc.m_pCurrentFrame	= GetParentFrame();
			//cc.m_pNewViewClass
			pDoc->CreateMainServer( cc );
		}
	}

	GetParentFrame()->RecalcLayout();
//	ResizeParentToFit();

//	SetScrollSizes( MM_TEXT, CSize(1,1) );
}

// CLauncherView diagnostics

#ifdef _DEBUG
void CLauncherView::AssertValid() const
{
	CSimpleLog::AssertValid();
}

void CLauncherView::Dump(CDumpContext& dc) const
{
	CSimpleLog::Dump(dc);
}
#endif //_DEBUG

// CLauncherView message handlers

BOOL	CLauncherView::Create( CWnd* pParent, UINT nID, CCreateContext& cc )
{
	return CSimpleLog::Create( 0, 0, WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), pParent, nID, &cc);
}

void CLauncherView::OnDestroy()
{
	// TODO: Add your message handler code here and/or call default
	__super::OnDestroy();
}

bool	CLauncherView::OnReceiveMessage( int nSeverity, LPCWSTR szMessage )
{
	CLauncherDoc* pDoc = GetDocument();
	pDoc->LogMainMessage( nSeverity, szMessage );

	return CSimpleLog::OnReceiveMessage(  nSeverity,  szMessage );
}