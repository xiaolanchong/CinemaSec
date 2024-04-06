// RoomView.cpp : implementation file
//

#include "stdafx.h"
#include "../CSLauncher.h"
#include "RoomView.h"
#include "../LauncherDoc.h"

// CRoomView

IMPLEMENT_DYNCREATE(CRoomView, CSimpleLog)

CRoomView::CRoomView():
	m_nRoomID(-1),
	m_bInit(false)
{
}

CRoomView::~CRoomView()
{
}

BEGIN_MESSAGE_MAP(CRoomView, CSimpleLog)
	ON_WM_DESTROY()
	ON_COMMAND( ID_BUTTON_RESTART, OnRoomRestart )
END_MESSAGE_MAP()

// CRoomView diagnostics

#ifdef _DEBUG
void CRoomView::AssertValid() const
{
	CSimpleLog::AssertValid();
}

void CRoomView::Dump(CDumpContext& dc) const
{
	CSimpleLog::Dump(dc);
}
#endif //_DEBUG


// CRoomView message handlers
void CRoomView::OnDestroy()
{
	// TODO: Add your message handler code here and/or call default
	CLauncherDoc* pDoc = GetDocument();
	pDoc->ShutdownServer(GetRoomID() );
	__super::OnDestroy();
}

bool	CRoomView::OnReceiveMessage( int nSeverity, LPCWSTR szMessage )
{
	CLauncherDoc* pDoc = GetDocument();
	pDoc->LogMessage( GetRoomID(), nSeverity, szMessage );
	return CSimpleLog::OnReceiveMessage( nSeverity, szMessage );
}

void CRoomView::OnInitialUpdate()
{
	CSimpleLog::OnInitialUpdate();
	if( !m_bInit)
	{
		m_bInit = true;
		CLauncherDoc* pDoc = GetDocument();
		BOOL bRes = pDoc->InitServer( GetRoomID(), this ); 
		if( !bRes )
		{
			PrintW( IDebugOutput::mt_error, L"Failed to initialize server" );
		}
	}
}

void CRoomView::OnRoomRestart()
{

}