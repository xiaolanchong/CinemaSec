#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "gauge_hall/gauge_hall_doc.h"
#include "gauge_hall/gauge_hall_frm.h"
#include "gauge_hall/gauge_hall_view.h"
#include "../res/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGaugeHallFrame, CChildFrame)

BEGIN_MESSAGE_MAP(CGaugeHallFrame, CChildFrame)
  //{{AFX_MSG_MAP(CGaugeHallFrame)
  ON_WM_CREATE()
  ON_WM_CANCELMODE()
  ON_NOTIFY( TCN_SELCHANGE, IDC_TAB_CAMERA, OnCameraChange )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
  ID_SEPARATOR,           // status line indicator
  ID_INDICATOR_CAPS,
  ID_INDICATOR_NUM,
  ID_INDICATOR_SCRL,
};


CGaugeHallFrame::CGaugeHallFrame()
{
}


CGaugeHallFrame::~CGaugeHallFrame()
{
}


int CGaugeHallFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CChildFrame::OnCreate( lpCreateStruct ) == -1)
		return -1;
	// TODO: Delete these three lines if you don't want the toolbar to be dockable.

	//    EnableDocking( CBRS_ALIGN_ANY );
	if (!m_wndToolBar.CreateEx( this, TBSTYLE_FLAT/*|TBSTYLE_LIST*/, WS_CHILD | WS_VISIBLE | CBRS_TOP |
						CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
						!(m_wndToolBar.LoadToolBar( IDR_GaugeHall )) )
	{
	TRACE0( "Failed to create toolbar\n" );
	return -1;
	}

	if ( !m_wndMainReBar.Create( this, RBS_AUTOSIZE  ) )
	{
		TRACE0("Failed to create main rebar\n");
		return -1;      // fail to create
	}
	if( !m_wndSelectBar.Create( this, IDD_ROOM_SELECT, CBRS_TOP , 1) ||
		!m_wndMainReBar.AddBar( &m_wndSelectBar, (LPCTSTR)0, 0, RBBS_BREAK|RBBS_NOGRIPPER  )  ||
		!m_wndMainReBar.AddBar( &m_wndToolBar, (LPCTSTR)0, 0, RBBS_BREAK|RBBS_NOGRIPPER  ) )
	{
		TRACE0("Failed to create select dialog bar\n");
		return -1;      // fail to create
	}

	m_LayoutManager.Attach( &m_wndSelectBar );
	m_LayoutManager.AddAllChildren();
	m_LayoutManager.SetConstraint(IDC_TAB_CINEMA,	OX_LMS_RIGHT, OX_LMT_SAME, 0);
	m_LayoutManager.SetConstraint(IDC_TAB_ROOM,		OX_LMS_RIGHT, OX_LMT_SAME, 0);
	m_LayoutManager.SetConstraint(IDC_TAB_CAMERA,	OX_LMS_RIGHT, OX_LMT_SAME, 0);
	m_LayoutManager.RedrawLayout();

	Init(IDC_TAB_CINEMA, IDC_TAB_ROOM, &m_wndSelectBar, this);
	m_wndCamera.SubclassDlgItem( IDC_TAB_CAMERA, &m_wndSelectBar );
	return 0;
}


BOOL CGaugeHallFrame::PreCreateWindow( CREATESTRUCT & cs )
{
  if (!(CChildFrame::PreCreateWindow( cs )))
    return FALSE;

  cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
    /*FWS_ADDTOTITLE |*/ WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;
  return TRUE;
}


//-------------------------------------------------------------------------------------------------
// CGaugeHallFrame diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CGaugeHallFrame::AssertValid() const
{
  CChildFrame::AssertValid();
}
void CGaugeHallFrame::Dump( CDumpContext & dc ) const
{
  CChildFrame::Dump( dc );
}
#endif //_DEBUG


BOOL CGaugeHallFrame::OnCommand( WPARAM wParam, LPARAM lParam )
{
  CGaugeHallDoc  * pDoc = reinterpret_cast<CGaugeHallDoc*>( GetActiveDocument() );
  CGaugeHallView * pView = pDoc->GetView();

  if ((pDoc != 0) && (pView != 0) && ((HIWORD( wParam ) == 0) || (HIWORD( wParam ) == 1)))
  {
    switch (LOWORD( wParam ))
    {
      case IDM_Edit_DeleteObject:
      {
		pDoc->DeleteObject();
        return TRUE;
      }

      case IDM_Edit_ObjectContouring:
      {
        if (pDoc->GetSubMode() != LOWORD( wParam ))
          pDoc->PartialClear();
        pDoc->SetSubMode( LOWORD( wParam ) );
        pView->Invalidate( FALSE );
        return TRUE;
      }

      case IDM_Edit_ObjectSelection:
      case IDM_Edit_CurveAdjusting:
      case IDM_Edit_CurveSmoothing:
      {
        pDoc->SetSubMode( LOWORD( wParam ) );
        pView->Invalidate( FALSE );
        return TRUE;
      }

      case IDM_Edit_AdjustingRadius_1: pDoc->GetAdjustRadius() = 1;  return TRUE;
      case IDM_Edit_AdjustingRadius_2: pDoc->GetAdjustRadius() = 2;  return TRUE;
      case IDM_Edit_AdjustingRadius_3: pDoc->GetAdjustRadius() = 3;  return TRUE;
      case IDM_Edit_AdjustingRadius_4: pDoc->GetAdjustRadius() = 4;  return TRUE;
      case IDM_Edit_AdjustingRadius_5: pDoc->GetAdjustRadius() = 5;  return TRUE;
      case IDM_Edit_AdjustingRadius_6: pDoc->GetAdjustRadius() = 6;  return TRUE;
      case IDM_Edit_AdjustingRadius_7: pDoc->GetAdjustRadius() = 7;  return TRUE;
      case IDM_Edit_AdjustingRadius_8: pDoc->GetAdjustRadius() = 8;  return TRUE;
      case IDM_Edit_AdjustingRadius_9: pDoc->GetAdjustRadius() = 9;  return TRUE;
    }
  }
  return CChildFrame::OnCommand( wParam, lParam );
}


BOOL CGaugeHallFrame::OnCmdMsg( UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo )
{
  CGaugeHallDoc * pDoc = reinterpret_cast<CGaugeHallDoc*>( GetActiveDocument() );
  if ((pDoc != 0) && (nCode == CN_UPDATE_COMMAND_UI))
  {
    CCmdUI * pCmdUI = reinterpret_cast<CCmdUI*>( pExtra );
    bool     bEnable = (!(pDoc->GetCountouting()) && !(pDoc->IsGradientsEmpty()/*m_gradients.empty()*/));
    bool     bCheck = (pDoc->GetSubMode() == pCmdUI->m_nID);

    switch (pCmdUI->m_nID)
    {
      case IDM_Edit_ObjectSelection: bEnable = (bEnable && !pDoc->IsObjectListEmpty () ); break;
      case IDM_Edit_DeleteObject: bEnable = (bEnable && pDoc->HasSelectObject() ); break;
      case IDM_Edit_ObjectContouring: break;
      case IDM_Edit_CurveAdjusting:
      case IDM_Edit_CurveSmoothing: bEnable = (bEnable && pDoc->HasSelectObject() &&
                                               !(pDoc->IsSelectedObjectEmpty() )); break;

      case IDM_Edit_AdjustingRadius_1: bCheck = (pDoc->GetAdjustRadius() == 1); break;
      case IDM_Edit_AdjustingRadius_2: bCheck = (pDoc->GetAdjustRadius() == 2); break;
      case IDM_Edit_AdjustingRadius_3: bCheck = (pDoc->GetAdjustRadius() == 3); break;
      case IDM_Edit_AdjustingRadius_4: bCheck = (pDoc->GetAdjustRadius() == 4); break;
      case IDM_Edit_AdjustingRadius_5: bCheck = (pDoc->GetAdjustRadius() == 5); break;
      case IDM_Edit_AdjustingRadius_6: bCheck = (pDoc->GetAdjustRadius() == 6); break;
      case IDM_Edit_AdjustingRadius_7: bCheck = (pDoc->GetAdjustRadius() == 7); break;
      case IDM_Edit_AdjustingRadius_8: bCheck = (pDoc->GetAdjustRadius() == 8); break;
      case IDM_Edit_AdjustingRadius_9: bCheck = (pDoc->GetAdjustRadius() == 9); break;

      default: return CChildFrame::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
    }

    pCmdUI->Enable( bEnable == true );
    pCmdUI->SetCheck( bCheck == true );
    return TRUE;
  }
  else
  {
    return CChildFrame::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
  }
}

void CGaugeHallFrame::OnCancelMode()
{
	CView* pView = GetActiveView();
	if ( pView) pView->SendMessage(WM_CANCELMODE);
}

bool	CGaugeHallFrame::SetImageButtonText(const std::vector<int> & v)
{
	return true;
}

inline void InsertCamera( CTabCtrl& Tab, int nCameraNo )
{
	TCITEM tc;
	tc.mask = TCIF_PARAM;
	CString s;
	// FIXME: to resources
	if( nCameraNo != -1 )
		s.Format( IDS_TAB_CAMERA, nCameraNo );
	else
		s.LoadString( IDS_TAB_ROOM );
	int nCount = Tab.GetItemCount();
	Tab.InsertItem( TCIF_PARAM | TCIF_TEXT, nCount, s, 0, LPARAM(nCameraNo), 0, 0  );
}

void	CGaugeHallFrame::OnRoomChange(int nRoomID)
{
	OnRoomChange( nRoomID, true );
}

int		CGaugeHallFrame::GetCurCamera()
{
	int nIndex = m_wndCamera.GetCurSel();
	TCITEM tc;
	tc.mask = TCIF_PARAM;
	BOOL b = m_wndCamera.GetItem( nIndex, &tc );
	//ASSERT(b);
	if(!b) return -1;
	return tc.lParam;
}

bool	CGaugeHallFrame::ShowAllCameras()
{
	if( !IsReady() ) return false;
	return GetCurCamera() == -1;
}

void	CGaugeHallFrame::InitRoomChange()
{
	int nRoomID = GetCurRoom();
	OnRoomChange( nRoomID, true );
}

void CGaugeHallFrame::OnCameraChange(NMHDR* pHdr, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pHdr);
	CGaugeHallDoc * pDoc = reinterpret_cast<CGaugeHallDoc*>( GetActiveDocument() );
	if( !pDoc) return;
	int nCameraID = GetCurCamera();
	pDoc->OnCameraChange( nCameraID );
	pDoc->UpdateAllViews(0);
	*pResult = 0;
}

void	CGaugeHallFrame::OnCinemaChange(int nRoomID)
{
//	IRoomChangeObserver::OnCinemaChange( nRoomID );
}

void	CGaugeHallFrame::OnRoomChange(int nRoomID, bool bNotify)
{
	CGaugeHallDoc * pDoc = reinterpret_cast<CGaugeHallDoc*>( GetActiveDocument() );
	if( !pDoc) return;
	std::vector< int > CameraNo;
	pDoc->GetCameras( nRoomID, CameraNo );
	m_wndCamera.DeleteAllItems();
	if( !CameraNo.empty() )
	{
		for( size_t i = 0; i < CameraNo.size(); ++i )
		{
			InsertCamera( m_wndCamera, CameraNo[i] );
		}
		// last is "show all" tab
		InsertCamera( m_wndCamera, -1 );
	}
	m_wndCamera.SetCurSel( 0 );
	pDoc->OnRoomChange( nRoomID );
	pDoc->UpdateAllViews(0);
}