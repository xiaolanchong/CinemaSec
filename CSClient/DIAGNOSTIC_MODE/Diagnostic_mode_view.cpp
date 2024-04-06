// DiagnosticView.cpp : implementation file
//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "CSClient.h"
#include "Diagnostic_mode_doc.h"
#include "Diagnostic_mode_view.h"

// CDiagnosticView

const UINT_PTR	Timer_Diagnostic	= 4952;
const DWORD		Period_Diagnostic	= 2 * 1000; // 2s

IMPLEMENT_DYNCREATE(CDiagnosticView, CFormView)

CDiagnosticView::CDiagnosticView()
	: CFormView(CDiagnosticView::IDD),
		m_bInit(false)
{
}

CDiagnosticView::~CDiagnosticView()
{
}

void CDiagnosticView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DIAGNOSTIC, m_wndDiagnostic);
}

BEGIN_MESSAGE_MAP(CDiagnosticView, CFormView)
	ON_WM_TIMER()
	ON_NOTIFY( NM_RCLICK, IDC_LIST_DIAGNOSTIC, OnRClkNotify )
END_MESSAGE_MAP()


// CDiagnosticView diagnostics

#ifdef _DEBUG
void CDiagnosticView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDiagnosticView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDiagnosticDoc * CDiagnosticView::GetDocument() // debug version
{
	ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CDiagnosticDoc ) ) );
	return (CDiagnosticDoc*)m_pDocument;
}

#endif //_DEBUG

#ifndef _DEBUG  
CDiagnosticDoc * CDiagnosticView::GetDocument() // non-debug version
{
	return reinterpret_cast<CDiagnosticDoc*>( m_pDocument );
}
#endif

#if 0
BOOL	CDiagnosticView::Create( CWnd* pParent, const CRect & rc, CCreateContext& cc , int i )
{
	return CFormView::Create( NULL,NULL,WS_CHILD | WS_VISIBLE /*| WS_BORDER*/, rc, pParent, AFX_IDW_PANE_FIRST+1 + i, &cc );
}
#endif

// CDiagnosticView message handlers

void CDiagnosticView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	if( !m_bInit )
	{
		m_bInit = true;
		m_wndDiagnostic.ModifyStyle( 0, TVS_HASBUTTONS | TVS_HASLINES|TVS_LINESATROOT);
		m_wndDiagnostic.ModifyExStyle( 0, TVOXS_ROWSEL|TVOXS_NOSORTHEADER );

		Init(IDC_TAB_CINEMA, IDC_TAB_ROOM, this, this);

		m_LayoutManager.Attach(this);
		m_LayoutManager.AddAllChildren();

		m_LayoutManager.SetConstraint(IDC_LIST_DIAGNOSTIC, OX_LMS_RIGHT, OX_LMT_SAME, -2);
		m_LayoutManager.SetConstraint(IDC_LIST_DIAGNOSTIC, OX_LMS_BOTTOM, OX_LMT_SAME, -2);

		m_LayoutManager.SetConstraint(IDC_TAB_CINEMA, OX_LMS_RIGHT, OX_LMT_SAME, -3);
		m_LayoutManager.SetConstraint(IDC_TAB_ROOM, OX_LMS_RIGHT, OX_LMT_SAME, -3);

		m_LayoutManager.RedrawLayout();

		BOOL res = m_ilDiagnostic.Create( 32, 32, ILC_COLOR24 |ILC_MASK, 4 , 4 );
		ASSERT(res);
		CBitmap bmp;
		res = bmp.LoadBitmap( IDB_DIAGNOSTIC );
		ASSERT(res);
		m_ilDiagnostic.Add( &bmp, RGB(255, 0, 255)  );
		m_wndDiagnostic.SetImageList( &m_ilDiagnostic, 0 );

		CString sDevice, sState;
		sDevice.LoadString( IDS_TITLE_DEVICE );
		sState.LoadString( IDS_TITLE_STATE);
		// FIXME resources
		LV_COLUMN lvc;
		lvc.fmt			= LVCFMT_LEFT;
		lvc.cx			= 150;
		lvc.pszText		= (LPTSTR)(LPCTSTR)sDevice;
		lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		m_wndDiagnostic.SetColumn( 0, &lvc );

		m_wndDiagnostic.InsertColumn(1, sState, LVCFMT_LEFT, 250);

		CDiagnosticDoc* pDoc= GetDocument();
		pDoc->CreateDiagnostic();

		int nRoomID = GetCurRoom();
		pDoc->CreateRoomDiagnostic( nRoomID, &m_wndDiagnostic);

		m_wndDiagnostic.SetItemHeight(40);

		SetTimer( Timer_Diagnostic, Period_Diagnostic, 0 );
	}

	ModifyStyleEx(  WS_EX_CLIENTEDGE, 0 );
	ModifyStyle(  WS_BORDER, 0 );
}

void CDiagnosticView::OnRoomChange(int nRoomID)
{
	CDiagnosticDoc* pDoc= GetDocument();
	pDoc->CreateRoomDiagnostic( nRoomID, &m_wndDiagnostic);
}

void CDiagnosticView::OnTimer( UINT_PTR nIDEvent )
{
	CFormView::OnTimer( nIDEvent );
	if( nIDEvent == Timer_Diagnostic )
	{
#if 1
		CDiagnosticDoc* pDoc= GetDocument();
		// FIXME
		int nRoomID = GetCurRoom();
		pDoc->RefreshDiagnostic(  nRoomID , &m_wndDiagnostic);
#endif
	}
}

void CDiagnosticView::OnRClkNotify( NMHDR* pNMHDR, LRESULT* pResult  )
{
	if( pNMHDR->hwndFrom == m_wndDiagnostic.GetSafeHwnd() &&
		pNMHDR->idFrom	== IDC_LIST_DIAGNOSTIC &&
		pNMHDR->code	== NM_RCLICK )
	{
		NMITEMACTIVATE * pHdr = (NMITEMACTIVATE*)pNMHDR;
		UINT flags;
		int nIndex = m_wndDiagnostic.HitTest( pHdr->ptAction, &flags );
		HTREEITEM hti = m_wndDiagnostic.GetItemFromIndex( nIndex );
		if(hti/* && (flags & TVHT_ONITEM )*/)
		{
			CDiagnosticDoc* pDoc= GetDocument();
			pDoc->OnRClickItem( &m_wndDiagnostic, hti, this );
		}
		*pResult = TRUE;
	}
	*pResult = FALSE;
}