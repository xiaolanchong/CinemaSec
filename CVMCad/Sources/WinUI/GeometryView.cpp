/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: GeometryView.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-15
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Resource.hpp"
#include "Exceptions.hpp"
#include "Configuration.hpp"
#include "CommandExecutor.hpp"
#include "CommandManager.hpp"
#include "Commands.hpp"
#include "ContextMenu.hpp"
#include "Model.hpp"
#include "Graphics.hpp"
#include "BaseView.hpp"
#include "GeometryView.hpp"



CGeometryView::CGeometryView()
{

	//Buttons unpressed by default
	m_bMouseLButtonDown = false;
	m_bMouseRButtonDown = false;
	m_bMouseMButtonDown = false;
	m_bSelectionHit = false;
	m_bMoveHit = false;

	//Default tool is @select 
	m_ToolType = ID_TOOL_SELECT;

	//Pan
	m_fPanX = 0.0f;
	m_fPanY = 0.0f;


}

CGeometryView::~CGeometryView()
{


}

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGeometryView, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDOWN()	
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

BOOL CGeometryView::Create( CWnd* pParent )
{
	if ( !CWnd::Create( NULL, NULL, NULL,
		CRect(0,0,0,0), pParent, 0, NULL ) )
		return FALSE;

	m_wndCMenu.Create( this );

	return TRUE;
}

BOOL CGeometryView::PreCreateWindow( CREATESTRUCT& cs )
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.style |= WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_OVERLAPPED;
	cs.dwExStyle |= WS_EX_OVERLAPPEDWINDOW;
	cs.lpszClass = AfxRegisterWndClass( 
		CS_HREDRAW | CS_VREDRAW, 
		::LoadCursor(NULL, IDC_ARROW), 
		NULL,
		NULL);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//Tools
//////////////////////////////////////////////////////////////////////////
int CGeometryView::GetActiveTool()
{
	return m_ToolType;
}
void CGeometryView::ChangeTool( int t )
{
	m_ToolType = t;

	//case cursor
}

void CGeometryView::Pan( int dx, int dy )
{
	m_fPanX += (float)dx;
	m_fPanY += (float)dy;

	OnUpdate();
}

void CGeometryView::Zoom( CPoint& pt, float zDelta )
{

}

void CGeometryView::Select( CPoint& pt0, CPoint& pt1 )
{
	CScene* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();
	if( pActiveScene == 0 )
		return;

	bool bUseHitTest = false;
	CPoint TopLeft, BottomRight;

	float m_fPointRadius = 3.0;

	if( pt0.x <= pt1.x){
		TopLeft.x = pt0.x;
		BottomRight.x = pt1.x;
	}
	else{
		TopLeft.x = pt1.x;
		BottomRight.x = pt0.x;
	}

	if( pt0.y <= pt1.y){
		TopLeft.y = pt0.y;
		BottomRight.y = pt1.y;
	}
	else{
		TopLeft.y = pt1.y;
		BottomRight.y = pt0.y;
	}

	Vertex vTopLeft, vBottomRight;
	vTopLeft.x = (float)TopLeft.x - m_fPanX;
	vTopLeft.y = (float)TopLeft.y - m_fPanY;

	vBottomRight.x = (float)BottomRight.x - m_fPanX;
	vBottomRight.y = (float)BottomRight.y - m_fPanY;

	VertexPtrList SelectedVertexList;

	if( m_GVVertices.empty() )
		return;

	CVertexToolSet::HitTest( &m_GVPtrVertices, vTopLeft, vBottomRight );

	OnUpdate(); 
}

void CGeometryView::GetPan( float& x, float& y )
{
	x = m_fPanX;
	y = m_fPanY;
}

void CGeometryView::GetCMenuPoint( CPoint& pt )
{
	pt = m_MouseRButtonDownPoint;
}

//////////////////////////////////////////////////////////////////////////
//Notifications
//////////////////////////////////////////////////////////////////////////
void CGeometryView::OnUpdate()
{
	try
	{
		CScene* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();
		if( pActiveScene == NULL )
		{
			OnPaint();
			return;
		}

		//Clear Geometry View vertices
		m_GVPtrVertices.resize(0);


		//Collect all vertices in scene
		CNodeToolSet::CollectNodes<Vertex>( pActiveScene, m_GVPtrVertices );

		//Apply current view transformation
		m_GVVertices.resize( m_GVPtrVertices.size() );
		for(  size_t i = 0; i< m_GVPtrVertices.size(); ++i )
		{
			m_GVVertices[i].x = m_GVPtrVertices[i]->x + m_fPanX;
			m_GVVertices[i].y = m_GVPtrVertices[i]->y + m_fPanY;
			m_GVVertices[i].selected = m_GVPtrVertices[i]->selected;
			m_GVVertices[i].active = m_GVPtrVertices[i]->active;
		}

		//Update points...
		m_PointyMesh.Update( &m_GVVertices );

		//Clear previously founded cameras and elements
		m_TempCamerasArray.resize(0);

		//Collect cameras
		CNodeToolSet::CollectNodes<CCamera>( pActiveScene, m_TempCamerasArray);

		//Populate scene presentation
		while( m_ScenePresentation.size() < m_TempCamerasArray.size() )
		{
			SceneSegment sg;
			shared_ptr<CTexture> ptr( new CTexture() );
			sg.pCamera = NULL;
			sg.pTexture = ptr;
			m_ScenePresentation.push_back( sg );
		}

		while( m_ScenePresentation.size() > m_TempCamerasArray.size() )
			m_ScenePresentation.pop_back();

		for( size_t i = 0; i < m_ScenePresentation.size(); ++i )
		{

			//Get camera's source;
			BITMAPINFO* pBitmapInfo;
			BYTE* pBytes;


			//Attach camera
			m_ScenePresentation[i].pCamera = m_TempCamerasArray[i];
			m_ScenePresentation[i].pCamera->GetSource( &pBitmapInfo, &pBytes );
			m_ScenePresentation[i].pTexture->Update( pBitmapInfo, pBytes );


			//and camera childs
			m_TempElementsArray.resize(0);
			CNodeToolSet::CollectNodes<CElement>( m_TempCamerasArray[i], m_TempElementsArray );

			//Resize to fit current elements num
			while( m_ScenePresentation[i].geometry.size() < m_TempElementsArray.size() )
			{
				shared_ptr<CMesh> ptr( new CMesh() );
				m_ScenePresentation[i].geometry.push_back( make_pair( (CElement*)NULL, ptr ) );
			}
			while( m_ScenePresentation[i].geometry.size() > m_TempElementsArray.size() )
				m_ScenePresentation[i].geometry.pop_back();

			for( size_t j = 0; j < m_ScenePresentation[i].geometry.size(); ++j )
			{
				VertexArray* pVertices;
				IndexArray* pIndices;
				VertexArray tVertices;

				//Get element vertices & indices
				m_TempElementsArray[j]->GetVertices( &pVertices );
				m_TempElementsArray[j]->GetIndices( &pIndices );

				//Apply current view transformation
				tVertices.resize( pVertices->size() );
				for(  size_t k = 0; k < pVertices->size(); ++k )
				{
					tVertices[k].x = (*pVertices)[k].x + m_fPanX;
					tVertices[k].y = (*pVertices)[k].y + m_fPanY;

					tVertices[k].tu = (*pVertices)[k].tu;

					//Invert tv, because y-axis direction is top-to-bottom
					tVertices[k].tv = 1.0f - (*pVertices)[k].tv;
					tVertices[k].color = (*pVertices)[k].color;
					tVertices[k].selected = (*pVertices)[k].selected;
				}

				//Attach element and it representation
				m_ScenePresentation[i].geometry[j].first = m_TempElementsArray[j];
				m_ScenePresentation[i].geometry[j].second->Update( &tVertices, pIndices );
			}
		}

		OnPaint();
	}
	catch( CBaseException<CGraphicsDevice> e )
	{
		e.DisplayErrorMessage();
		//SHUTDOWN
	}

	
}



//////////////////////////////////////////////////////////////////////////
//Messaging
//////////////////////////////////////////////////////////////////////////

void CGeometryView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if( pWnd->m_hWnd = m_hWnd )
	{
		if (point.x == -1 && point.y == -1)
		{
			//Keystroke invocation
			CRect rect;
			GetClientRect(&rect);
			ClientToScreen(&rect);
			point = rect.TopLeft();
			point.Offset(5, 5);
		}
		m_wndCMenu.Show( this, point );
	}
}

INT CGeometryView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{ 
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
	CApplicationManager* pAM = CApplicationManager::GetInstance();

	try
	{
		//Create additional swap chain
		pGD->CreateSwapChain( this );
		pGD->SetActiveSwapChain( this );

		//Add to listeners
		pAM->AddViewer( this );
	}
	catch( CBaseException<CGraphicsDevice> e )
	{
		e.DisplayErrorMessage();
	}
	catch( CBaseException<CApplicationManager> e )
	{
		e.DisplayErrorMessage();
	}


	return 0;
}

BOOL CGeometryView::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo )
{
	//Redirect all command messages to CommandManager
	if( nCode == CN_COMMAND && CCommandManager::GetInstance()->ProcessCommand( nID ) )
		return true;

	return CWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void CGeometryView::OnDestroy()
{
	CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
	CApplicationManager* pAM = CApplicationManager::GetInstance();

	pGD->DestroySwapChain( this );
	pAM->RemoveViewer( this );
}

void CGeometryView::OnPaint()
{
	try
	{
		CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
		CApplicationManager* pAM = CApplicationManager::GetInstance();
		CScene* pAC = pAM->GetActiveScene();

		if( pAC == NULL )
		{
			pGD->SetActiveSwapChain( this );
			pGD->Clear();
			pGD->Present();
			return;
		}

		pGD->SetActiveSwapChain( this );
		pGD->Clear();
		pGD->BeginScene();

			pGD->EnableWireframe( true );

			for( size_t i = 0; i < m_ScenePresentation.size(); ++i )
			{
				pGD->EnableWireframe( false );
				pGD->EnableTexturedVertex( true );

				m_ScenePresentation[i].pTexture->Set();

				for( size_t j = 0; j < m_ScenePresentation[i].geometry.size(); ++j )
				{
					if( m_ScenePresentation[i].geometry[j].first->GetWired() || m_ScenePresentation[i].pTexture == NULL  )
					{
						pGD->EnableWireframe( true );
						pGD->EnableTexturedVertex( false );
						pGD->EnableColorVertex( true );
					}
					else
					{
						pGD->EnableWireframe( false );
						pGD->EnableTexturedVertex( true );
						pGD->EnableColorVertex( false );
					}

					m_ScenePresentation[i].geometry[j].second->Draw();
				}
			}

			pGD->EnableWireframe( false );
			pGD->EnableTexturedVertex( true );


			pGD->EnableWireframe( false );
			pGD->EnableColorVertex( true );
			//Draw points
			m_PointyMesh.Draw();

			//Draw selection rect
			m_SelectionRectMesh.Draw();

		pGD->EndScene();
		pGD->Present();
	}
	catch( CBaseException<CGraphicsDevice> e )
	{
		e.DisplayErrorMessage();
	}
	catch( CBaseException<CApplicationManager> e )
	{
		e.DisplayErrorMessage();
	}
}

void CGeometryView::OnSize( UINT nType, int cx, int cy )
{
	if( cx == 0 && cy == 0 )
		return;

	CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
	CApplicationManager* pAM = CApplicationManager::GetInstance();

	pGD->SetActiveSwapChain( this );
	pGD->ResizeSwapChain( cx, cy );
}

void CGeometryView::OnLButtonUp( UINT nFlags, CPoint point )
{
	CRect SelectionRect( 0,0,0,0 );
	m_SelectionRectMesh.Update( &SelectionRect );
	m_bMouseLButtonDown = false;
	m_bSelectionHit = false;
	m_bMoveHit = false;

	OnPaint();
}
void CGeometryView::OnLButtonDown( UINT nFlags, CPoint point )
{
	m_InitialMouseLButtonDownPoint = point;
	m_bMouseLButtonDown = true;
	m_bMoveHit = false;
	m_MouseLButtonDownPoint = point;
	m_InitialMouseLButtonDownPoint = point;

	if( m_GVVertices.empty() )
		return;

	Vertex* pVertex = NULL;
	Vertex v;
	v.x = (float)point.x - m_fPanX;
	v.y = (float)point.y - m_fPanY;

	if( pVertex = CVertexToolSet::HitTestOnSelected( &m_GVPtrVertices, v ) )
	{
		if( !(~nFlags&MK_SHIFT) )
		{
			pVertex->selected = pVertex->selected?false:true;
			m_bSelectionHit = true;
		}
		else if ( !(~nFlags&MK_CONTROL) )
		{
            if( pVertex->selected )
			{
				m_bMoveHit = true;
			}

		}
		else 
		{
            CVertexToolSet::DeselectAll( &m_GVPtrVertices );
			pVertex->selected = true;
			m_bSelectionHit = true;
		}

	}
	else
	{
		m_bSelectionHit = false;
		CVertexToolSet::DeselectAll( &m_GVPtrVertices );
	}

	OnUpdate();
	

}
void CGeometryView::OnRButtonUp( UINT nFlags, CPoint point )
{
	CScene* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();
	m_bMouseRButtonDown = false;
	CWnd::OnRButtonDown( nFlags, point );
}

void CGeometryView::OnRButtonDown( UINT nFlags, CPoint point )
{
	m_bMouseRButtonDown = true;
	m_MouseRButtonDownPoint = point;

	//Needed to handle context menu
	CWnd::OnRButtonDown( nFlags, point );
}

void CGeometryView::OnMButtonUp( UINT nFlags, CPoint point )
{
	m_bMouseMButtonDown = false;

}

void CGeometryView::OnMButtonDown( UINT nFlags, CPoint point )
{
	m_bMouseMButtonDown = true;
	m_MouseMButtonDownPoint = point;
}

void CGeometryView::OnMouseMove( UINT nFlags, CPoint point )
{
	CRect SelectionRect;


	if( m_bMouseLButtonDown )
	{
		switch ( m_ToolType ) 
		{

		case ID_TOOL_SELECT:

			if( !(~nFlags&MK_CONTROL) )
			{
				m_bMoveHit = true;

				float dx = (float)(point.x - m_MouseLButtonDownPoint.x);
				float dy = (float)(point.y - m_MouseLButtonDownPoint.y);
				
				CVertexToolSet::Translate( &m_GVPtrVertices, dx,dy );
				m_MouseLButtonDownPoint = point;
				OnUpdate();

			}else
			{
				SelectionRect.left =  m_MouseLButtonDownPoint.x;
				SelectionRect.top =  m_MouseLButtonDownPoint.y;

				SelectionRect.right =  point.x;
				SelectionRect.bottom =  point.y;

				Select( m_MouseLButtonDownPoint, point );
				m_SelectionRectMesh.Update( &SelectionRect );
			}

			OnPaint();
			break;

		case ID_TOOL_PAN:
			Pan( point.x - m_MouseLButtonDownPoint.x, point.y - m_MouseLButtonDownPoint.y );
			m_MouseLButtonDownPoint = point;
			break;

		}

		m_bMoveHit = false;
	}

}

BOOL CGeometryView::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	return true;
}