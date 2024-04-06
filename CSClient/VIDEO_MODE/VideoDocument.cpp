// VideoDocument.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "csclient.h"
#include "VideoDocument.h"
#include "VideoView.h"
#include "VideoStatView.h"
#include "../../CSEngine/DBFacet/CinemaOleDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVideoDocument

IMPLEMENT_DYNCREATE(CVideoDocument, CArrangeDocument)

CVideoDocument::CVideoDocument()
{
}

CVideoDocument::~CVideoDocument()
{
}


BEGIN_MESSAGE_MAP(CVideoDocument, CArrangeDocument)
	//{{AFX_MSG_MAP(CVideoDocument)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoDocument diagnostics

#ifdef _DEBUG
void CVideoDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CVideoDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVideoDocument commands

void	CVideoDocument::LoadRoom( int nRoomID )
{
	CArrangeDocument::LoadRoom( nRoomID );
	// also load current video
	GetArrangeView()->SetAutoUpdateSource( true );
	LoadVideoRoomState( nRoomID );
}