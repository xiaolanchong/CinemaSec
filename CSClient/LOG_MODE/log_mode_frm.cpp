#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "../ChildFrm.h"
#include "log_mode_frm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CLogFrame, CChildFrame)

BEGIN_MESSAGE_MAP(CLogFrame, CChildFrame)
  //{{AFX_MSG_MAP(CLogFrame)
  ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//-------------------------------------------------------------------------------------------------
// CLogFrame diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CLogFrame::AssertValid() const
{
  CChildFrame::AssertValid();
}
void CLogFrame::Dump( CDumpContext & dc ) const
{
  CChildFrame::Dump( dc );
}
#endif //_DEBUG

int CLogFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CChildFrame::OnCreate( lpCreateStruct ) == -1)
		return -1;
	return 0;
}

BOOL CLogFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	UNREFERENCED_PARAMETER( lpcs );
	CChildFrame::OnCreateClient(lpcs, pContext);
	return TRUE;//CChildFrame::OnCreateClient(lpcs, pContext);
}