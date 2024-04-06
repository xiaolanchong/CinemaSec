// VIDEO_MODE\VideoStatView.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "ArrangeDocument.h"
#include "ArrangeStatView.h"

#include "../DBFacet/CfgDB.h"
#include "../../CSEngine/DBFacet/CinemaOleDB.h"
#include "wv_message.h"


IMPLEMENT_DYNAMIC(CArrangeStatView, CFormView)

CArrangeStatView::CArrangeStatView(UINT nFormID):
	CFormView( nFormID ),
	m_bInit(false)
{
//	GetStatisticsDB().Register( this );
}

CArrangeStatView::~CArrangeStatView()
{
//	GetStatisticsDB().Unregister( this );
}


BEGIN_MESSAGE_MAP(CArrangeStatView, CFormView)
	ON_WM_CREATE()
	ON_NOTIFY( NM_DBLCLK, IDC_LIST_STAT, OnDblClkNotify )
	ON_NOTIFY( NM_RCLICK, IDC_LIST_STAT, OnRClkNotify )
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CArrangeStatView drawing

CArrangeDocument* CArrangeStatView::GetDocument()
{
	return dynamic_cast<CArrangeDocument*>( m_pDocument );
}

void CArrangeStatView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 1;
	SetScrollSizes(MM_TEXT, sizeTotal);

	if( !m_bInit )
	{
		m_bInit = true;

		ModifyStyleEx(  WS_EX_CLIENTEDGE, 0 );
		ModifyStyle(  WS_BORDER, 0 );
		m_wndStatistics.SubclassDlgItem( IDC_LIST_STAT, this );
		m_CtrlHelper.Attach( this, GetParentFrame() );

		CWnd*		pMainAppWnd = AfxGetMainWnd();
		CMainFrame* pFrm		= DYNAMIC_DOWNCAST ( CMainFrame, pMainAppWnd );
		pFrm->RegisterChangeTab( this );
	}
}

void CArrangeStatView::OnDraw(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
	// TODO: add draw code here
}

// CArrangeStatView diagnostics

#ifdef _DEBUG
void CArrangeStatView::AssertValid() const
{
	CFormView::AssertValid();
}

void CArrangeStatView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

// CArrangeStatView message handlers

int CArrangeStatView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  Add your specialized creation code here
	return 0;
}

BOOL CArrangeStatView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style &=  ~(WS_BORDER);
	cs.dwExStyle &=  ~(WS_EX_CLIENTEDGE);
	return CFormView::PreCreateWindow(cs);
}

void				CArrangeStatView::OnRoomChange(int nRoomID)	
{
	CArrangeDocument* pDoc = GetDocument();
	pDoc->LoadRoom( nRoomID );
}

void CArrangeStatView::OnDblClkNotify( NMHDR* pNMHDR, LRESULT* pResult  )
{
	if( pNMHDR->hwndFrom == m_wndStatistics.GetSafeHwnd() &&
		pNMHDR->idFrom	== IDC_LIST_STAT &&
		pNMHDR->code	== NM_DBLCLK)
	{
		NMITEMACTIVATE * pHdr = (NMITEMACTIVATE*)pNMHDR;
		UINT flags;
		int nIndex = m_wndStatistics.HitTest( pHdr->ptAction, &flags );
		HTREEITEM hti = m_wndStatistics.GetItemFromIndex( nIndex );
		if(hti && (flags & TVHT_ONITEM ))
		{
			OnDblClickItem( hti,CPoint(pHdr->ptAction) );
		}
		*pResult = TRUE;
	}
	*pResult = FALSE;
}

void CArrangeStatView::OnRClkNotify( NMHDR* pNMHDR, LRESULT* pResult  )
{
	if( pNMHDR->hwndFrom == m_wndStatistics.GetSafeHwnd() &&
		pNMHDR->idFrom	== IDC_LIST_STAT &&
		pNMHDR->code	== NM_RCLICK )
	{
		NMITEMACTIVATE * pHdr = (NMITEMACTIVATE*)pNMHDR;
		UINT flags;
		int nIndex = m_wndStatistics.HitTest( pHdr->ptAction, &flags );
		HTREEITEM hti = m_wndStatistics.GetItemFromIndex( nIndex );
		if(hti && (flags & TVHT_ONITEM ))
		{
			OnRClickItem( hti );
		}
		*pResult = TRUE;
	}
	*pResult = FALSE;
}

void CArrangeStatView::OnDestroy()
{
	CWnd*		pMainAppWnd = AfxGetMainWnd();
	CMainFrame* pFrm		= DYNAMIC_DOWNCAST ( CMainFrame, pMainAppWnd );
	pFrm->UnregisterChangeTab( this );

	__super::OnDestroy();
}

void CArrangeStatView::OnActivateTab( HWND hWnd )
{
	CArrangeDocument* pDoc = GetDocument();
	pDoc->DisableAction( hWnd != GetParentFrame()->GetSafeHwnd()  ); 
	pDoc->OnActivateMode( hWnd == GetParentFrame()->GetSafeHwnd() );
}