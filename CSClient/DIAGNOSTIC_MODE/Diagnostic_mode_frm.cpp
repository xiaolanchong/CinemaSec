#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "../ChildFrm.h"
#include "diagnostic_mode_doc.h"
#include "diagnostic_mode_frm.h"
#include "diagnostic_mode_view.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CDiagnosticFrame, CChildFrame)

BEGIN_MESSAGE_MAP(CDiagnosticFrame, CChildFrame)
  //{{AFX_MSG_MAP(CDiagnosticFrame)
  ON_WM_CREATE()
  ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CDiagnosticFrame::CDiagnosticFrame()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CDiagnosticFrame::~CDiagnosticFrame()
{
}


//-------------------------------------------------------------------------------------------------
// CBoxOfficeFrame diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CDiagnosticFrame::AssertValid() const
{
  CChildFrame::AssertValid();
}
void CDiagnosticFrame::Dump( CDumpContext & dc ) const
{
  CChildFrame::Dump( dc );
}
#endif //_DEBUG


int CDiagnosticFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
  if (CChildFrame::OnCreate( lpCreateStruct ) == -1)
    return -1;
  return 0;
}

BOOL CDiagnosticFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	UNREFERENCED_PARAMETER( lpcs );
	CChildFrame::OnCreateClient(lpcs, pContext);
	return TRUE;//CChildFrame::OnCreateClient(lpcs, pContext);
}

void CDiagnosticFrame::OnSize( UINT nType, int cx, int cy  )
{
	CChildFrame::OnSize(nType, cx, cy);
}