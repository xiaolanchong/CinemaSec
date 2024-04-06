#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "diagnostic_mode_doc.h"
#include "diagnostic_mode_view.h"
#include "../DBFacet/DBSerializer.h"
#include "../../CSEngine/DBFacet/CinemaOleDB.h"
#include "../Log_Mode/ClientLogManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CDiagnosticDoc, CDocument)

BEGIN_MESSAGE_MAP(CDiagnosticDoc, CDocument)
  //{{AFX_MSG_MAP(CDiagnosticView)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

const DWORD c_nExchangeID = 0xffff0101;

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CDiagnosticDoc::CDiagnosticDoc()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CDiagnosticDoc::~CDiagnosticDoc()
{
}


//-------------------------------------------------------------------------------------------------
// CDiagnosticView diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CDiagnosticDoc::AssertValid() const
{
  CDocument::AssertValid();
}
void CDiagnosticDoc::Dump( CDumpContext & dc ) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief Function returns pointer to CBoxOfficeView class. */
//-------------------------------------------------------------------------------------------------
CDiagnosticView * CDiagnosticDoc::GetDiagnosticView()
{
  for (POSITION pos = GetFirstViewPosition(); pos != 0;)
  {
    CView * p = GetNextView( pos );
    if ((p != 0) && p->IsKindOf( RUNTIME_CLASS( CDiagnosticDoc ) ))
    {
      return reinterpret_cast<CDiagnosticView*>( p );
    }
  }
  return 0;
}

void	CDiagnosticDoc::CreateDiagnostic()
{
	HRESULT hr;
	ISystemDiagnostic* pInt;
	DWORD dwRes = CreateEngineInterface( SYSTEM_DIAGNOSTIC_INTERFACE, (void**)&pInt );
	ASSERT( dwRes == 0 );
	IDebugOutput* pDbgInt = GetLogManager().CreateDebugOutput( L"Diagnostic" );
	hr = pInt->Initialize( pDbgInt );
	ASSERT( hr == S_OK);
	boost::shared_ptr<ISystemDiagnostic> pTmp( pInt, ::DestroyDiagnostic );
	m_pDiagnostic = pTmp;
}

void	CDiagnosticDoc::DestroyDiagnostic()
{

}

std::pair<UINT, int> GetStateAndImage( bool bServer, ISystemDiagnostic::DeviceState ds )
{
	const	int Server_Offline	= 4;
	const	int Server_Wait		= 2;
	const	int Server_Work		= 0;
	const	int Camera_Offline	= 5;
	const	int Camera_Wait		= 3;
	const	int Camera_Work		= 1;
	switch( ds )
	{
	case ISystemDiagnostic::ds_offline: 
			return std::make_pair( IDS_STATE_OFFLINE, bServer? Server_Offline : Camera_Offline ) ;
	case ISystemDiagnostic::ds_stopped:
			return std::make_pair( IDS_STATE_WAIT, bServer? Server_Wait : Camera_Wait) ;
	case ISystemDiagnostic::ds_working:
		return std::make_pair( IDS_STATE_WORK, bServer? Server_Work : Camera_Work ) ;
	case ISystemDiagnostic::ds_processing:
		return std::make_pair( IDS_STATE_WORK, bServer? Server_Work : Camera_Work );
	default:
		return std::make_pair( IDS_STATE_OFFLINE, bServer? Server_Offline : Camera_Offline ) ;
	}
}

std::pair<UINT, int> GetStateAndImageForExchange( ISystemDiagnostic::DeviceState ds )
{
	const	int Exchange_Offline	= 7;
	const	int Exchange_Work		= 6;
	switch( ds )
	{
	case ISystemDiagnostic::ds_offline: 
		return std::make_pair( IDS_STATE_OFFLINE, Exchange_Offline ) ;
	case ISystemDiagnostic::ds_stopped:
		return std::make_pair( IDS_STATE_DISCONNECTED, Exchange_Offline ) ;
	case ISystemDiagnostic::ds_working:
	case ISystemDiagnostic::ds_processing:
		return std::make_pair( IDS_STATE_WORK, Exchange_Work );
	default:
		return std::make_pair( IDS_STATE_OFFLINE, Exchange_Offline ) ;
	}
}

void	CDiagnosticDoc::CreateRoomDiagnostic(int nRoomID, COXTreeCtrl* pTree)
try
{
	pTree->DeleteAllItems();
	if( !m_pDiagnostic )
	{
		return;
	}

	std::set< std::tstring > ServerSet;
	CCinemaOleDB& db = dynamic_cast<CCinemaOleDB&>( GetDB() );
	const std::set<RoomRow>& Rooms = db.GetTableRoomFacet().GetRooms();
	std::set<RoomRow>::const_iterator itRoom = Rooms.find( nRoomID );
	ASSERT( itRoom != Rooms.end() ) ;// room has no server? oh, gosh
	ServerSet.insert( itRoom->m_sIP );
	const std::set<CameraRow>& Cameras = db.GetTableCameraFacet().GetCameras();
	std::set<CameraRow>::const_iterator itCam = Cameras.begin();
	for( ; itCam != Cameras.end() ; ++itCam )
	{
		if( itCam->m_nRoomID == nRoomID )
			ServerSet.insert( itCam->m_sIP );
	}
	std::set<int>	CamSet = db.GetTableCameraFacet().SelectCamerasFromRoom( nRoomID );

	std::set< std::tstring >::const_iterator itNewServer = ServerSet.begin();
	HTREEITEM hti;
	for( ; itNewServer != ServerSet.end(); ++itNewServer )
	{
		hti = InsertComputerItem( pTree, itNewServer->c_str(), nRoomID );
		std::set<CameraRow>::const_iterator itCam = Cameras.begin();
		for( ; itCam != Cameras.end() ; ++itCam )
		{
			if( itCam->m_sIP == *itNewServer && 
				itCam->m_nRoomID == nRoomID)
			{
				InsertCameraItem(pTree, hti, itCam->m_nID );
			}
		}
		InsertExchangeItem ( pTree, itNewServer->c_str(), hti );
	}

	pTree->Invalidate();
}
catch( DataBaseException )
{
	pTree->DeleteAllItems();
}
catch( std::bad_cast )
{
	pTree->DeleteAllItems();
};

void	CDiagnosticDoc::RefreshDiagnostic(int nRoomID, COXTreeCtrl* pTree)
try
{
	if( !m_pDiagnostic )
	{
		pTree->DeleteAllItems();
		return;
	}

	bool bNeedRedraw = false;
	HRESULT hr;
	HTREEITEM hti = pTree->GetChildItem( pTree->GetRootItem() );
	while (hti != NULL)
	{
		CString sName = pTree->GetItemText( hti );
		BOOL res;
		ISystemDiagnostic::DeviceState ds;
		hr = m_pDiagnostic->GetComputerState( sName, nRoomID, ds );
		if( hr != S_OK ) ds = ISystemDiagnostic::ds_offline;
#if 0
		static int n = 1;
		ds = ( n%2 )? ISystemDiagnostic::ds_working : ISystemDiagnostic::ds_stopped;
		++n;
#endif
		std::pair<UINT, int> p = GetStateAndImage( true, ds);
		CString sState;
		sState.LoadString( p.first );
		res = pTree->SetItemText( hti, sState, 1 );
		ASSERT(res);
		int nImage, nSel;
		pTree->GetItemImage( hti, nImage, nSel );
		if( nImage != p.second )
		{
			pTree->SetItemImage( hti, p.second, p.second );
			bNeedRedraw = true;
		}
		ASSERT(res);
		bool bCamRefresh = RefreshChildren( pTree,  sName, hti );
		bNeedRedraw = bCamRefresh ? true : bNeedRedraw;
		hti = pTree->GetNextItem(hti, TVGN_NEXT);
	}
	if( bNeedRedraw ) pTree->Invalidate();
}
catch( DataBaseException )
{
	pTree->DeleteAllItems();
}
catch( std::bad_cast )
{
	pTree->DeleteAllItems();
};

// NOTE: assume that camera configuration don't change, so after cameras wera added we only refresh their states
 bool		CDiagnosticDoc::RefreshChildren(	COXTreeCtrl* pTree, CString sServerName, HTREEITEM htParent )
{
	bool bNeedRedraw = false;
	HTREEITEM hti = pTree->GetChildItem( htParent );
	while (hti != NULL)
	{
		HTREEITEM hNextItem = pTree->GetNextItem(hti, TVGN_NEXT);
		int nCameraID = pTree->GetItemData( hti );
		if( nCameraID == c_nExchangeID ) 
		{
			bool bNeedRedrawExchange = RefreshExchangeItem( pTree, sServerName, hti );
			if( bNeedRedrawExchange ) bNeedRedraw = bNeedRedrawExchange;
			hti = hNextItem;
			continue;
		}

		HRESULT hr;
		ISystemDiagnostic::DeviceState ds;
		hr = m_pDiagnostic->GetCameraState( nCameraID, ds  );
		if( hr != S_OK ) ds = ISystemDiagnostic::ds_offline;
#if 0
		ds = ISystemDiagnostic::ds_working ;
#endif
		std::pair<UINT, int> p = GetStateAndImage( false, ds );
		CString sState;
		sState.LoadString( p.first );
		pTree->SetItemText( hti, sState, 1 );
		int nImage, nSel;
		pTree->GetItemImage( hti, nImage, nSel );
		if( nImage != p.second )
		{
			pTree->SetItemImage( hti, p.second, p.second );
			bNeedRedraw = true;
		}
		hti = hNextItem;
	}
	return bNeedRedraw;
}

void	CDiagnosticDoc::InsertCameraItem(  COXTreeCtrl* pTree, HTREEITEM htiParent, int nCameraID )
{
	CString sCamName;
	sCamName.Format(_T("%d"), nCameraID );
	HTREEITEM htiChild = pTree->InsertItem(  sCamName, htiParent);
	pTree->SetItemData( htiChild, nCameraID );

	HRESULT hr;
	ISystemDiagnostic::DeviceState ds;
	hr = m_pDiagnostic->GetCameraState( nCameraID, ds  );
	if( hr != S_OK ) ds = ISystemDiagnostic::ds_offline;
	std::pair<UINT, int> p = GetStateAndImage( false, ds );
	BOOL res;
	CString sState;
	sState.LoadString( p.first );
	res = pTree->SetItemText( htiChild, sState, 1 );
	ASSERT(res);
	res = pTree->SetItemImage( htiChild, p.second, p.second );
	ASSERT(res);
}

HTREEITEM	CDiagnosticDoc::InsertComputerItem(  COXTreeCtrl* pTree, CString sName, int nRoomID)
{
	HTREEITEM hti = pTree->InsertItem( sName );
	HRESULT hr;
	BOOL res;
	ISystemDiagnostic::DeviceState ds;
	hr = m_pDiagnostic->GetComputerState( sName, nRoomID, ds );
	if( hr != S_OK ) ds = ISystemDiagnostic::ds_offline;
	std::pair<UINT, int> p = GetStateAndImage( true, ds);
	CString sState;
	sState.LoadString( p.first );
	res = pTree->SetItemText( hti, sState, 1 );
	ASSERT(res);
	res = pTree->SetItemImage( hti, p.second, p.second );
	ASSERT(res);
	pTree->Expand( hti, TVE_EXPAND);
//	RefreshCamera( pTree, hti, Cams );
	
	return hti;
}

void		CDiagnosticDoc::InsertExchangeItem ( COXTreeCtrl* pTree, CString sServerName, HTREEITEM htiParent )
{
	CString sName;
	sName.LoadString( IDS_BO_EXCHANGE );
	HTREEITEM htiChild = pTree->InsertItem(  sName, htiParent);
	pTree->SetItemData( htiChild, c_nExchangeID );

	HRESULT hr;
	ISystemDiagnostic::DeviceState ds;

	hr = m_pDiagnostic->GetExchangeState( (LPCWSTR)sServerName, ds  );
	if( hr != S_OK ) ds = ISystemDiagnostic::ds_offline;
	std::pair<UINT, int> p = GetStateAndImageForExchange( ds );
	BOOL res;
	CString sState;
	sState.LoadString( p.first );
	res = pTree->SetItemText( htiChild, sState, 1 );
	ASSERT(res);
	res = pTree->SetItemImage( htiChild, p.second, p.second );
	ASSERT(res);
}

bool	CDiagnosticDoc::RefreshExchangeItem ( COXTreeCtrl* pTree, CString sName, HTREEITEM hti )
{
	bool bNeedRedraw = false;
	HRESULT hr;
	ISystemDiagnostic::DeviceState ds;
	hr = m_pDiagnostic->GetExchangeState( sName, ds  );
	if( hr != S_OK ) ds = ISystemDiagnostic::ds_offline;
#if 0
	ds = ISystemDiagnostic::ds_working ;
#endif
	std::pair<UINT, int> p = GetStateAndImageForExchange( ds );
	CString sState;
	sState.LoadString( p.first );
	pTree->SetItemText( hti, sState, 1 );
	int nImage, nSel;
	pTree->GetItemImage( hti, nImage, nSel );
	if( nImage != p.second )
	{
		pTree->SetItemImage( hti, p.second, p.second );
		bNeedRedraw = true;
	}
	return bNeedRedraw;
}

void	CDiagnosticDoc::OnRClickItem( COXTreeCtrl* pTree, HTREEITEM hti, CWnd* pThis )
{
	HTREEITEM htParent = pTree->GetParentItem( hti );
	if( pTree->GetRootItem() == pTree->GetParentItem( hti ) ) return; // computer
	DWORD nItemID = pTree->GetItemData(hti);
	if( nItemID != c_nExchangeID ) return ; //camera
	CString sServerName = pTree->GetItemText( htParent );
	CMenu menu;
	menu.CreatePopupMenu();
	const UINT_PTR Item_Report		= 1;
	const UINT_PTR Item_Reconnect	= 2;
	
	ISystemDiagnostic::DeviceState ds;
	HRESULT hr;
	hr = m_pDiagnostic->GetExchangeState( sServerName, ds  );
	if( hr != S_OK ) ds = ISystemDiagnostic::ds_offline;

	CString sMenuItem;
	UINT nFlag;
	nFlag = (ds == ISystemDiagnostic::ds_working) || (ds == ISystemDiagnostic::ds_processing) ? 0 : MF_GRAYED;
	sMenuItem.LoadString( IDS_CREATEREPORT );
	menu.AppendMenu( MF_STRING | nFlag, Item_Report, sMenuItem );

	nFlag = (ds == ISystemDiagnostic::ds_stopped) ? 0 : MF_GRAYED;
	sMenuItem.LoadString( IDS_RECONNECT );
	menu.AppendMenu( MF_STRING | nFlag, Item_Reconnect, sMenuItem );

	CPoint pt;
	GetCursorPos( &pt );
	UINT nID = menu.TrackPopupMenuEx( TPM_RETURNCMD|TPM_NONOTIFY, pt.x, pt.y, pThis , 0 );

	switch (nID)
	{
	case Item_Report:
		hr = m_pDiagnostic->Report( sServerName );
		break;
	case Item_Reconnect:
		hr = m_pDiagnostic->Reconnect( sServerName );
		break;
	}
}

