#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "../ChildFrm.h"
#include "boxoffice_mode_doc.h"
#include "boxoffice_mode_frm.h"
#include "boxoffice_mode_view.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CBoxOfficeFrame, CChildFrame)

BEGIN_MESSAGE_MAP(CBoxOfficeFrame, CChildFrame)
  //{{AFX_MSG_MAP(CAlgoDebugFrame)
  ON_WM_CREATE()
  ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CBoxOfficeFrame::CBoxOfficeFrame() : m_bSplitterCreated(false)
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CBoxOfficeFrame::~CBoxOfficeFrame()
{
}


//-------------------------------------------------------------------------------------------------
// CBoxOfficeFrame diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CBoxOfficeFrame::AssertValid() const
{
  CChildFrame::AssertValid();
}
void CBoxOfficeFrame::Dump( CDumpContext & dc ) const
{
  CChildFrame::Dump( dc );
}
#endif //_DEBUG


int CBoxOfficeFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
  if (CChildFrame::OnCreate( lpCreateStruct ) == -1)
    return -1;
  return 0;
}

BOOL CBoxOfficeFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	UNREFERENCED_PARAMETER( lpcs );
	CChildFrame::OnCreateClient(lpcs, pContext);
	return TRUE;//CChildFrame::OnCreateClient(lpcs, pContext);
}

void CBoxOfficeFrame::OnSize( UINT nType, int cx, int cy  )
{
	CChildFrame::OnSize(nType, cx, cy);

	
	if( m_bSplitterCreated )  // m_bSplitterCreated set in OnCreateClient
	{
	//	m_wndSplitter.GetPane(0, 1)->GetWindowRect(rect);
	//	m_wndSplitter.SetColumnInfo(0, rcOwn.Width() - rect.Width(), 100);
	//	m_wndSplitter.SetColumnInfo(1, rect.Width(), 100);
	//	m_wndSplitter.RecalcLayout();
	}

}