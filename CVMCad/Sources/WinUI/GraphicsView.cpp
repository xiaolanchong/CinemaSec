/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: GraphicsView.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-05
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Graphics.hpp"
#include "Configuration.hpp"
#include "Exceptions.hpp"
#include "Elements.hpp"
#include "Documents.hpp"
#include "BaseView.hpp"
#include "GraphicsView.hpp"



CGraphicsView::CGraphicsView()
{
	m_bMouseLButtonDown = false;
	m_bMouseRButtonDown = false;
	point_selected = false;
	pe = NULL;
}

CGraphicsView::~CGraphicsView()
{

}

IMPLEMENT_DYNCREATE(CGraphicsView, CWnd )
BEGIN_MESSAGE_MAP(CGraphicsView, CWnd)
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

BOOL CGraphicsView::Create( CWnd* pParent )
{
	if ( !CWnd::Create( NULL, NULL, NULL,
		CRect(0,0,0,0), pParent, 0, NULL ) )
		return FALSE;

	return TRUE;
}

BOOL CGraphicsView::PreCreateWindow( CREATESTRUCT& cs )
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.style |= WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPSIBLINGS|WS_OVERLAPPED;
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass( 
		CS_HREDRAW | CS_VREDRAW, 
		::LoadCursor(NULL, IDC_ARROW), 
		NULL,
		NULL);

	return TRUE;
}


void CGraphicsView::RenderView()
{
	try
	{
		CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
		CApplicationManager* pAM = CApplicationManager::GetInstance();

		RenderFullSceneView();
	}
	catch( CGraphicsDeviceException e )
	{
		e.DisplayErrorMessage();
	}
	catch( CApplicationManagerException e )
	{
		e.DisplayErrorMessage();
	}
}

void CGraphicsView::ClearView()
{
	try
	{
		CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
		CApplicationManager* pAM = CApplicationManager::GetInstance();

		pGD->Clear();
		pGD->Present();
	}
	catch( CGraphicsDeviceException e )
	{
		e.DisplayErrorMessage();
	}
	catch( CApplicationManagerException e )
	{
		e.DisplayErrorMessage();
	}
}

void CGraphicsView::ChangeViewMode( ViewMode m )
{
	m_ActiveViewMode = m;
}

void CGraphicsView::RenderFullSceneView()
{
	try
	{
		CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
		CApplicationManager* pAM = CApplicationManager::GetInstance();
		CSceneDoc* pScene = pAM->GetActiveScene();

		list<CVirtualCamera*>* pCameras;
		list<CBaseElement*>* pElements;
		pScene->GetCameras( &pCameras );
		ASSERT(pCameras);

		pGD->Clear();
		pGD->BeginScene();

		for( list<CVirtualCamera*>::iterator i = pCameras->begin(); i != pCameras->end(); ++i )
		{
			ASSERT(*i);
			(*i)->GetElements( &pElements );

			for( list<CBaseElement*>::iterator j = pElements->begin(); j!=pElements->end(); ++j )
			{
				(*j)->Draw();

			}
		}

		pGD->EndScene();
		pGD->Present();
	}
	catch( CGraphicsDeviceException e )
	{
		e.DisplayErrorMessage();
	}
	catch( CApplicationManagerException e )
	{
		e.DisplayErrorMessage();
	}
}

void CGraphicsView::RenderTexCoordEditingView()
{
	try
	{
		CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
		CApplicationManager* pAM = CApplicationManager::GetInstance();
		CSceneDoc* pScene = pAM->GetActiveScene();

		list<CVirtualCamera*>* pCameras;
		list<CBaseElement*>* pElements;

		pScene->GetCameras( &pCameras );
		ASSERT(pCameras);

		for( list<CVirtualCamera*>::iterator i = pCameras->begin(); i != pCameras->end(); ++i )
		{
			(*i)->GetElements( &pElements );
			ASSERT(pElements);

			for ( list<CBaseElement*>::iterator j = pElements->begin(); j != pElements->end(); ++j )
			{
				
			}
		}
	}
	catch( CGraphicsDeviceException e )
	{
		e.DisplayErrorMessage();
	}
	catch( CApplicationManagerException e )
	{
		e.DisplayErrorMessage();
	}
}

//////////////////////////////////////////////////////////////////////////
void CGraphicsView::OnUpdate()
{
	CSceneDoc* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();
	if( pActiveScene == 0 )
	{
		ClearView();
		return;
	}

	RenderView();

}

void CGraphicsView::OnContextMenu(CWnd* pWnd, CPoint point)
{


}

INT CGraphicsView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{ 
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
	CApplicationManager* pAM = CApplicationManager::GetInstance();

	//Create additional swap chain
	pGD->CreateAdditionalSwapChain( this );

	//Add to listeners
	pAM->AddViewer( this );

	return 0;
}

void CGraphicsView::OnDestroy()
{
	CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
	CApplicationManager* pAM = CApplicationManager::GetInstance();

	pAM->RemoveViewer( this );
}

void CGraphicsView::OnPaint()
{
	if( !IsWindowVisible() )
		return;

	CSceneDoc* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();
	if( pActiveScene == 0 )
	{
		ClearView();
		return;
	}

	RenderView();
}


void CGraphicsView::OnLButtonUp( UINT nFlags, CPoint point )
{
	try
	{
		CSceneDoc* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();
		if( pActiveScene == 0 )
		{
			return;
		}


		CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
		CApplicationManager* pAM = CApplicationManager::GetInstance();
		CSceneDoc* pScene = pAM->GetActiveScene();

		list<CVirtualCamera*>* pCameras;
		list<CBaseElement*>* pElements;
		vector<Vertex>::iterator index;

		pScene->GetCameras( &pCameras );
		ASSERT(pCameras);

		

		//for( list<CVirtualCamera*>::iterator i = pCameras->begin(); i != pCameras->end(); ++i )
		//{
		//	(*i)->GetElements( &pElements );
		//	ASSERT(pElements);

		//	for( list<CBaseElement*>::iterator j = pElements->begin(); j!= pElements->end(); ++j )
		//	{
  //              if( (*j)->HitTest( point, index ) )
		//		{
		//			vector<Vertex>* pVertices;
  //                  (*j)->GetVertices( &pVertices );
		//			(*index).color = RGB(rand()%255,rand()%255,rand()%255);
		//			(*j)->Update();

		//			pe = *j;
  //              }

		//	}

		//}

		m_bMouseLButtonDown = false;
		point_selected = false;
	}
	catch( CGraphicsDeviceException e )
	{
		e.DisplayErrorMessage();
	}
	catch( CApplicationManagerException e )
	{
		e.DisplayErrorMessage();
	}

}
void CGraphicsView::OnLButtonDown( UINT nFlags, CPoint point )
{
	CSceneDoc* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();
	if( pActiveScene == 0 )
	{
		ClearView();
		return;
	}

	m_bMouseLButtonDown = true;
	m_MouseLButtonDownPoint = point;

	try
	{
		CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
		CApplicationManager* pAM = CApplicationManager::GetInstance();
		CSceneDoc* pScene = pAM->GetActiveScene();

		list<CVirtualCamera*>* pCameras;
		list<CBaseElement*>* pElements;
		vector<Vertex>::iterator index;

		pScene->GetCameras( &pCameras );
		ASSERT(pCameras);

		for( list<CVirtualCamera*>::iterator i = pCameras->begin(); i != pCameras->end(); ++i )
		{
			(*i)->GetElements( &pElements );
			ASSERT(pElements);

			for( list<CBaseElement*>::iterator j = pElements->begin(); j!= pElements->end(); ++j )
			{

					if( (*j)->HitTest( point, index ) )
					{
						vector<Vertex>* pVertices;
						(*j)->GetVertices( &pVertices );
						pe = *j;
						m = index;
						point_selected = true;
					}

			}

		}

	}
	catch( CGraphicsDeviceException e )
	{
		e.DisplayErrorMessage();
	}
	catch( CApplicationManagerException e )
	{
		e.DisplayErrorMessage();
	}


}
void CGraphicsView::OnRButtonUp( UINT nFlags, CPoint point )
{


}

void CGraphicsView::OnRButtonDown( UINT nFlags, CPoint point )
{
	if (pe == NULL )
	{
		return;
	}

	Vertex v( point.x, point.y );


	pe->AddVertex(v);
	pe->RecreateMesh();
	pe->Update();

}

void CGraphicsView::OnMButtonUp( UINT nFlags, CPoint point )
{


}

void CGraphicsView::OnMButtonDown( UINT nFlags, CPoint point )
{
	if (pe == NULL )
	{
		return;
	}

	vector<Vertex>* pVert;
	pe->GetVertices( &pVert );

	for( int i = 0; i<pVert->size();++i )
	{
		(*pVert)[i].Scale(0.9);
	}
	pe->Update();





}

void CGraphicsView::OnMouseMove( UINT nFlags, CPoint point )
{
	try
	{
		CSceneDoc* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();
		if( pActiveScene == 0 )
		{
			ClearView();
			return;
		}

		if( (m_bMouseLButtonDown == false) || (point_selected == false ) )
		{
            return;
		}

		CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
		CApplicationManager* pAM = CApplicationManager::GetInstance();
		CSceneDoc* pScene = pAM->GetActiveScene();

		list<CVirtualCamera*>* pCameras;
		list<CBaseElement*>* pElements;
		vector<Vertex>::iterator index;

		float dx = point.x - m_MouseLButtonDownPoint.x;
		float dy = point.y - m_MouseLButtonDownPoint.y;

		vector<Vertex>* pVertices;
		(*m).x += dx;
		(*m).y += dy;
		pe->Update();

		m_MouseLButtonDownPoint = point;

	}
	catch( CGraphicsDeviceException e )
	{
		e.DisplayErrorMessage();
	}
	catch( CApplicationManagerException e )
	{
		e.DisplayErrorMessage();
	}

}

BOOL CGraphicsView::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	return true;
}

void CGraphicsView::OnSize( UINT nType, int cx, int cy )
{
	if( cx == 0 && cy == 0 )
		return;

	CGraphicsDevice* pGD = CGraphicsDevice::GetInstance();
	CApplicationManager* pAM = CApplicationManager::GetInstance();

	//Set client rect
	pGD->SetActiveSwapChain( this );
	pGD->SetViewport( &CRect(0,0,cx,cy) );

	
}
