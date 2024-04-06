#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "archive_mode_doc.h"
#include "archive_mode_view.h"
#include "../res/resource.h"
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CArchiveView, CArrangeView)

BEGIN_MESSAGE_MAP(CArchiveView, CArrangeView)
  //{{AFX_MSG_MAP(CArchiveView)
  ON_WM_SIZE()
  ON_WM_DESTROY()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CArchiveView::CArchiveView():m_bInitialized(false)
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CArchiveView::~CArchiveView()
{
}

//-------------------------------------------------------------------------------------------------
// CArchiveView diagnostics.
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CArchiveView::AssertValid() const
{
  CArrangeView::AssertValid();
}
void CArchiveView::Dump( CDumpContext & dc ) const
{
  CArrangeView::Dump( dc );
}
CArchiveDoc * CArchiveView::GetDocument() // debug version
{
  ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CArchiveDoc ) ) );
  return (CArchiveDoc*)m_pDocument;
}
#endif //_DEBUG

void CArchiveView::OnInitialUpdate( )
{
	CArrangeView::OnInitialUpdate( );
	if(!m_bInitialized )
	{
		m_bInitialized = true;

		m_pdlgTransfer = std::auto_ptr<CFileTransferDialog>( new CFileTransferDialog() );
		m_pdlgTransfer->Create( CFileTransferDialog::IDD, AfxGetMainWnd() );
	}
}

void	CArchiveView::PostLoadRoom()
{
	// if ArrayRender is enabled then save the render state
	ClearImage();
}

void	CArchiveView::LoadRoom( int nRoomID, const WindowArray_t& wa )
{
	__super::LoadRoom( nRoomID, wa );
	std::vector<int>	CameraArr;
	for( size_t i = 0; i < wa.size(); ++i )
	{
		CameraArr.push_back( wa[i].m_nID );
	}
	CArchiveDoc* pDoc = GetDocument();
	pDoc->LoadArchive( CameraArr );

	m_nCachedRoomID = nRoomID;
}

const UINT_PTR Item_ShowTransferWnd	=	5;
const UINT_PTR Item_Transfer		=	6;

void	CArchiveView::AddContextMenuItem( CMenu& menu )
{
#ifdef SWITCH_ON_TRANSFER
	CString sItem;
	sItem.LoadString(IDS_SHOW_TRANSFER_WINDOW);
	UINT nCheck = m_pdlgTransfer->IsWindowVisible()? ( MF_GRAYED|MF_CHECKED ) : 0 ;
	menu.AppendMenu( MF_STRING|nCheck,	Item_ShowTransferWnd, sItem );
#ifdef DEBUG_TRANSFER
	menu.AppendMenu( MF_STRING,			Item_Transfer, _T("Transfer") );
#endif
#endif
}

void	CArchiveView::ProcessContextMenuItem( UINT_PTR nID )
{
#ifdef SWITCH_ON_TRANSFER
	switch( nID )
	{
#ifdef DEBUG_TRANSFER
	case Item_Transfer:
		{
			if( m_pdlgTransfer->IsTransfered() )
			{
				AfxMessageBox( IDS_ERROR_TRANSFER_EXISTS );
				break;
			}
			CArchiveDoc* pDoc = GetDocument();
			static IDebugOutput* pDbgInt = GetLogManager().CreateDebugOutput( L"File transfer" );
			TCHAR szBuf[MAX_PATH];
			GetModuleFileName( GetModuleHandle(NULL), szBuf, MAX_PATH );
			PathRemoveFileSpec( szBuf );
			CString sBasePath = szBuf;
			CTime timeBegin = 1, timeEnd = 2;

			m_pdlgTransfer->ShowWindow(SW_SHOW);
			std::vector< std::pair<int, CString > > Cams = pDoc->GetRoomCamera( m_nCachedRoomID );
			m_pdlgTransfer->Transfer(pDbgInt, sBasePath, Cams, timeBegin, timeEnd );
		}
		break;
#endif
	case Item_ShowTransferWnd:
		{
			m_pdlgTransfer->ShowWindow( SW_SHOW );
		}
		break;
	}
#endif
}

void	CArchiveView::OnDestroy()
{
	m_pdlgTransfer = std::auto_ptr<CFileTransferDialog>();

	__super::OnDestroy();
}