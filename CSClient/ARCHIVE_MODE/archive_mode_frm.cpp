#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "../ChildFrm.h"
#include "archive_mode_doc.h"
#include "archive_mode_frm.h"
#include "archive_mode_view.h"
#include "ArchiveStatView.h"
#include "ArchiveContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CArchiveFrame, CChildFrame)

BEGIN_MESSAGE_MAP(CArchiveFrame, CChildFrame)
  //{{AFX_MSG_MAP(CAlgoDebugFrame)
  ON_WM_CREATE()
  ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CArchiveFrame::CArchiveFrame() : m_bSplitterCreated(false)
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CArchiveFrame::~CArchiveFrame()
{
}


//-------------------------------------------------------------------------------------------------
// CArchiveFrame diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CArchiveFrame::AssertValid() const
{
  CChildFrame::AssertValid();
}
void CArchiveFrame::Dump( CDumpContext & dc ) const
{
  CChildFrame::Dump( dc );
}
#endif //_DEBUG


int CArchiveFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
  if (CChildFrame::OnCreate( lpCreateStruct ) == -1)
    return -1;
  return 0;
}

BOOL CArchiveFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	UNREFERENCED_PARAMETER( lpcs );
	m_wndSplitter.CreateStatic( this, 1, 2, WS_CHILD | WS_VISIBLE/* | WS_CLIPCHILDREN*/ );
	m_wndSplitter.CreateView( 0, 1, RUNTIME_CLASS( CArchiveContainerView ), CSize(600, 0), pContext );
	m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS( CArchiveStatView ), CSize(200, 0), pContext );
	m_bSplitterCreated = true;
//	ModifyStyle( 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS );
	return TRUE;//CChildFrame::OnCreateClient(lpcs, pContext);
}

void CArchiveFrame::OnSize( UINT nType, int cx, int cy  )
{
	CChildFrame::OnSize(nType, cx, cy);

	CRect rect, rcOwn;
	GetClientRect( &rcOwn );
	
	if( m_bSplitterCreated )  // m_bSplitterCreated set in OnCreateClient
	{
	//	m_wndSplitter.GetPane(0, 1)->GetWindowRect(rect);
	//	m_wndSplitter.SetColumnInfo(0, rcOwn.Width() - rect.Width(), 100);
	//	m_wndSplitter.SetColumnInfo(1, rect.Width(), 100);
	//	m_wndSplitter.RecalcLayout();
	}

}