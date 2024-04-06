#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "log_mode_doc.h"
#include "log_mode_view.h"
#include "ClientLogManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT_PTR	Timer_Log	= 0x84371;
const UINT		Period_Log	= 500; //ms

IMPLEMENT_DYNCREATE(CLogView, CFormView)

BEGIN_MESSAGE_MAP(CLogView, CFormView)
  //{{AFX_MSG_MAP(CLogView)
  ON_WM_TIMER()
  ON_NOTIFY( TCN_SELCHANGE, IDC_TAB_LOGWINDOW, OnTabSelChange )
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CLogView::CLogView():
	CFormView(CLogView::IDD),
	m_bInitialized(false)
{
}

//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CLogView::~CLogView()
{
}

//-------------------------------------------------------------------------------------------------
// CLogView diagnostics.
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CLogView::AssertValid() const
{
  CFormView::AssertValid();
}
void CLogView::Dump( CDumpContext & dc ) const
{
  CFormView::Dump( dc );
}
CLogDoc * CLogView::GetDocument() // debug version
{
  ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CLogDoc ) ) );
  return (CLogDoc*)m_pDocument;
}
#endif //_DEBUG

void CLogView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogView)
	//}}AFX_DATA_MAP
}

void CLogView::OnInitialUpdate( )
{
	CFormView::OnInitialUpdate();
	if(!m_bInitialized )
	{
		m_bInitialized = true;
	
		m_LayoutManager.Attach(this);
		m_LayoutManager.AddAllChildren();

		m_LayoutManager.SetConstraint(IDC_TAB_LOGWINDOW,		OX_LMS_RIGHT,	OX_LMT_SAME, 0);
		m_LayoutManager.SetConstraint(IDC_LIST_LOG_MESSAGE,		OX_LMS_RIGHT,	OX_LMT_SAME, 0);
		m_LayoutManager.SetConstraint(IDC_LIST_LOG_MESSAGE,		OX_LMS_BOTTOM,	OX_LMT_SAME, 0);

		m_LayoutManager.RedrawLayout();

		m_wndDebugSelector.SubclassDlgItem( IDC_TAB_LOGWINDOW, this );

		m_DebugWindowArr.reserve( 10 );
		m_DebugWindowArr.push_back( boost::shared_ptr< COXTreeCtrl >( new COXTreeCtrl ) );
		COXTreeCtrl* pTree = m_DebugWindowArr.back().get();
		pTree->SubclassDlgItem(IDC_LIST_LOG_MESSAGE, this);
		InitTree( pTree, true );
		m_nActiveTree = 0;

		SetTimer( Timer_Log, Period_Log, 0 );

	}
	// i prefer rubber windows, so switch off default CFormView layout (w/ scroll bar)
	SetScrollSizes( MM_TEXT, CSize(1, 1) );
}

void	CLogView::InitTree( COXTreeCtrl* pTree, bool bFirst )
{
	LVCOLUMN lvc;
	ZeroMemory( &lvc, sizeof(LVCOLUMN) );
	lvc.pszText = _T("Time");
	lvc.cx		= 100;
	lvc.mask	= LVCF_TEXT| LVCF_WIDTH;

	if( bFirst )
		pTree->SetColumn( 0, &lvc);
	else
		pTree->InsertColumn( 0, &lvc );
	pTree->InsertColumn( 1, _T("Message"), LVCFMT_LEFT, 500 );
	if( bFirst )
		pTree->ModifyExStyle( 0, TVOXS_ROWSEL );
}

void  CLogView::OnTimer( UINT_PTR nIDEvent )
{
	CFormView::OnTimer( nIDEvent );

	if( nIDEvent == Timer_Log )
	{
		std::queue<IClientLogManager::Message_t>& MsgQueue = GetLogManager().LockBuffer();
		while( !MsgQueue.empty() )
		{
			const IClientLogManager::Message_t& msg = MsgQueue.front();
			COXTreeCtrl* pTree = GetTree( boost::get<0>(msg).c_str() );
			InsertMessage( pTree, boost::get<1>(msg), boost::get<2>(msg), boost::get<3>(msg) );
			GetLogManager().WriteMessage( msg );
			MsgQueue.pop();
		}
		GetLogManager().UnlockBuffer();
	}
}

void	CLogView::InsertMessage( COXTreeCtrl* pTree, CTime timeMsg, int nType, const std::wstring& sMsg )
{
	CStringW sTrimmedMsg ( sMsg.c_str() );
	sTrimmedMsg.TrimRight( L"\r\n" );

	const int c_nDefCount = 1000;
	int nCount = c_nDefCount;

	if( !nCount)
	{
		pTree->DeleteAllItems();
	}
	if( pTree->GetItemCount( ) > nCount )
	{
		for( UINT i = 0; i < pTree->GetCount() - nCount; ++i )
		{
			HTREEITEM hti = pTree->GetItemFromIndex( 0 );
			pTree->DeleteItem( hti );
		}
	}


	COLORREF clMsg;
	CString s;
	switch( nType )
	{
	case IDebugOutput::mt_debug_info :	clMsg = RGB(192, 192, 192);  break;
	case IDebugOutput::mt_info :		clMsg = RGB(128, 128, 128); break;
	case IDebugOutput::mt_warning :		clMsg = RGB(216, 216, 0); break;
	case IDebugOutput::mt_error :		clMsg = RGB(255, 0, 0); break;
	default :							clMsg = RGB( 0, 0, 255 ); break;
	}
	CString strTime = timeMsg.Format( _T("%b%d %H:%M:%S") );

	HTREEITEM hItem = pTree->InsertItem( strTime );
	pTree->SetItemColor( hItem, clMsg, 0 );
	pTree->SetSubItem( hItem, 1, OX_SUBITEM_COLOR | OX_SUBITEM_TEXT, sTrimmedMsg, 0, 0,  clMsg );
}

COXTreeCtrl*	CLogView::GetTree( LPCWSTR szSystem )
{
	COXTreeCtrl* pTree = NULL;
	int nCount = m_wndDebugSelector.GetItemCount();
	if( m_DebugWindowArr.size() == 1 && nCount == 0)
	{
		pTree = m_DebugWindowArr.back().get();
		m_wndDebugSelector.InsertItem( nCount, szSystem );
		m_DebugSystemArr.push_back( std::make_pair( std::wstring(szSystem), m_DebugWindowArr.size() - 1) ) ;
//		GetTree(L"Test"); // for test
	}
	else
	{
		for( size_t i = 0; i < m_DebugSystemArr.size(); ++i )
		{
			if( m_DebugSystemArr[i].first == szSystem )
			{
				size_t nIndex = m_DebugSystemArr[i].second;
				pTree = m_DebugWindowArr[ nIndex ].get();
				ASSERT(pTree);
				return pTree;
			}
		}
		m_wndDebugSelector.InsertItem( nCount, szSystem );

		m_DebugWindowArr.push_back( boost::shared_ptr< COXTreeCtrl >( new COXTreeCtrl ) );
		m_DebugSystemArr.push_back( std::make_pair( std::wstring(szSystem), m_DebugWindowArr.size() - 1) ) ;
		pTree = m_DebugWindowArr.back().get();
		pTree->Create( LVS_REPORT|LVS_OWNERDRAWFIXED|WS_BORDER, CRect(0,0,0,0), this, 1, TVOXS_ROWSEL|TVOXS_COLUMNHDR );
		InitTree( pTree, false );
	}
	ASSERT(pTree);
	return pTree;
}

void	CLogView::OnTabSelChange( NMHDR*, LRESULT* pRes )
{
	int nCurSel = m_wndDebugSelector.GetCurSel();
	size_t nTrueCurSel = nCurSel;
	if( nTrueCurSel		< m_DebugSystemArr.size() && 
		m_nActiveTree	< m_DebugSystemArr.size() &&
		m_nActiveTree != nTrueCurSel )
	{
		CRect rc;
		m_DebugWindowArr[ m_nActiveTree ]->GetWindowRect(&rc);
		ScreenToClient(&rc);
		m_DebugWindowArr[ m_nActiveTree ]->ShowWindow( SW_HIDE );
		m_DebugWindowArr[ m_nActiveTree ]->SetDlgCtrlID( 1 );
		m_DebugWindowArr[ nTrueCurSel ]->SetDlgCtrlID( IDC_LIST_LOG_MESSAGE );
		m_DebugWindowArr[ nTrueCurSel ]->MoveWindow( &rc );
		m_DebugWindowArr[ nTrueCurSel ]->ShowWindow( SW_SHOW );
		m_nActiveTree = nTrueCurSel;
	}
	*pRes = 0;
}