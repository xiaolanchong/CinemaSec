/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: TexCoordsEditorView.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-19
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/

#include "Common.hpp"
#include "Resource.hpp"
#include "Configuration.hpp"
#include "Exceptions.hpp"
#include "Model.hpp"
#include "Graphics.hpp"
#include "Pane.hpp"
#include "BaseView.hpp"
#include "TexCoordsEditorView.hpp"


CTexCoordsEditorView::CTexCoordsEditorView()
{

	m_bMouseLButtonDown = false;
	m_bMouseRButtonDown = false;
	m_bMouseMButtonDown = false;


}

CTexCoordsEditorView::~CTexCoordsEditorView()
{

}

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTexCoordsEditorView, CWnd)
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

BOOL CTexCoordsEditorView::Create( CWnd* pParent )
{
	if( !CGenericPane::Create( pParent, L"TexCoords Editor" ) )
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CTexCoordsEditorView::SelectTool( CPoint& pt0, CPoint& pt1 )
{
	CScene* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();
	if( pActiveScene == 0 )
		return;

	float m_fPointRadius = 5.0;

	CRect ClientRect;
	GetClientRect( &ClientRect );


	CPoint CentralPoint = pt0;

	float TexViewWidth = (float)(m_TexCoordsViewportRect.right - m_TexCoordsViewportRect.left);
	float TexViewHeight = (float)(m_TexCoordsViewportRect.bottom - m_TexCoordsViewportRect.top);

	CentralPoint.x-=m_TexCoordsViewportRect.left;
	CentralPoint.y-=m_TexCoordsViewportRect.top;

	Vertex c( (float)(CentralPoint.x/ TexViewWidth), (float)(CentralPoint.y / TexViewHeight) );

	float r =  m_fPointRadius/TexViewWidth;

	CCamera* pCamera;
	if( CCameraToolSet::FindActive( &pCamera ) )
	{
		VertexPtrArray vertices;
		CNodeToolSet::CollectNodes<Vertex>( pActiveScene, vertices );
		for( size_t i = 0; i < vertices.size(); ++i )
			vertices[i]->selected = false;

		vertices.clear();
		CNodeToolSet::CollectNodes<Vertex>( pCamera, vertices );
		CVertexToolSet::TCHitTest( &vertices, c, r );
	}

	CApplicationManager::GetInstance()->NotifyViewers();
}

//////////////////////////////////////////////////////////////////////////

void CTexCoordsEditorView::Move( int dx, int dy )
{
	CScene* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();
	if( pActiveScene == 0 )
		return;


	CCamera* pCamera;
	if ( CCameraToolSet::FindActive( &pCamera ) )
	{
		CRect ClientRect;
		GetClientRect( &ClientRect );

		float TexViewWidth = (float)(m_TexCoordsViewportRect.right - m_TexCoordsViewportRect.left);
		float TexViewHeight = (float)(m_TexCoordsViewportRect.bottom - m_TexCoordsViewportRect.top);

		float tx = dx / TexViewWidth;
		float ty = dy / TexViewHeight;

		VertexPtrArray vertices;
		CNodeToolSet::CollectNodes<Vertex>( pCamera, vertices );

		CVertexToolSet::TCTranslate( &vertices, tx,ty );

		CApplicationManager::GetInstance()->NotifyViewers();
	}

}

//////////////////////////////////////////////////////////////////////////
//Notifications
//////////////////////////////////////////////////////////////////////////
void CTexCoordsEditorView::OnUpdate()
{
	OnPaint();
}

//////////////////////////////////////////////////////////////////////////
//Messaging
//////////////////////////////////////////////////////////////////////////

void CTexCoordsEditorView::OnContextMenu(CWnd* pWnd, CPoint point)
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
	}
}

INT CTexCoordsEditorView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{ 
	if ( CWnd::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	CApplicationManager* pAM = CApplicationManager::GetInstance();

	//Add to listeners
	pAM->AddViewer( this );
	return 0;
}

void CTexCoordsEditorView::OnDestroy()
{
	CApplicationManager* pAM = CApplicationManager::GetInstance();

	//Add to listeners
	pAM->RemoveViewer( this );
}

void CTexCoordsEditorView::OnPaint()
{
	CDC* pDC = GetDC();
	CDC memDC;

	CRect ClientRect;
	CBrush bgBrush(GetSysColor( CTLCOLOR_DLG ));
	GetClientRect( &ClientRect );
	ClientRect.top+=20;

	memDC.CreateCompatibleDC( pDC );
	CBitmap membmp;
	membmp.CreateCompatibleBitmap( pDC, ClientRect.right-ClientRect.left, ClientRect.bottom - ClientRect.top );
	memDC.SelectObject( &membmp );

	memDC.FillRect( &ClientRect, &bgBrush );

	CPen TexCoordViewRectPen( PS_SOLID, 2, RGB(0,0,0) );

	//Draw view rect
	memDC.SelectObject( &TexCoordViewRectPen );
	memDC.Rectangle( m_TexCoordsViewportRect );

	//Draw current camera source bmp
	CApplicationManager* pAM = CApplicationManager::GetInstance();
	CScene* pActiveScene = pAM->GetActiveScene();
	if( pActiveScene != NULL )
	{
		CCamera* pCamera = NULL;
		if( CCameraToolSet::FindActive( &pCamera ) )
		{
			BITMAPINFO* pBitmapInfo;
			BYTE* pData;

			pCamera->GetSource( &pBitmapInfo, &pData );

			if( pData != NULL )
			{
				CDC dcMemory;
				CBitmap bmp;
				bmp.CreateBitmap( 
					pBitmapInfo->bmiHeader.biWidth, 
					pBitmapInfo->bmiHeader.biHeight,
					pBitmapInfo->bmiHeader.biPlanes,
					pBitmapInfo->bmiHeader.biBitCount,
					pData );

				dcMemory.CreateCompatibleDC( pDC );
				dcMemory.SelectObject( &bmp );

				//Select the bitmap into the in-memory DC
				memDC.StretchBlt( 
					m_TexCoordsViewportRect.left, m_TexCoordsViewportRect.top,
					m_TexCoordsViewportRect.right - m_TexCoordsViewportRect.left,
					m_TexCoordsViewportRect.bottom - m_TexCoordsViewportRect.top, &dcMemory, 
					0,0, pBitmapInfo->bmiHeader.biWidth,pBitmapInfo->bmiHeader.biHeight, SRCCOPY );

				dcMemory.DeleteDC();
			}
		}
	}

	//Draw TC vertices
	COLORREF LineColor = RGB( 0,255,0 );
	COLORREF PointColor = RGB( 255,0,0 );

	float TexViewWidth = (float)(m_TexCoordsViewportRect.right - m_TexCoordsViewportRect.left);
	float TexViewHeight = (float)(m_TexCoordsViewportRect.bottom - m_TexCoordsViewportRect.top);

	CPen TexCoordViewRectPen2( PS_SOLID, 1, LineColor );

	//Draw view rect
	memDC.SelectObject( &TexCoordViewRectPen2 );

	if( pActiveScene != NULL )
	{
		CCamera* pCamera = NULL;
		if( CCameraToolSet::FindActive( &pCamera ) )
		{
			CElementPtrArray elements;
			CNodeToolSet::CollectNodes<CElement>( pCamera, elements );

			float xs = (float)m_TexCoordsViewportRect.Width();
			float ys = (float)m_TexCoordsViewportRect.Height();

			float xt = (float)m_TexCoordsViewportRect.left;
			float yt = (float)m_TexCoordsViewportRect.top;

			for( size_t i = 0; i < elements.size(); ++i )
			{
				VertexArray* pVertices;
				IndexArray* pIndices;
				elements[i]->GetVertices( &pVertices );
				elements[i]->GetIndices ( &pIndices );

#pragma warning( disable : 4244 )

				//Draw triangle edges
				for( size_t pos = 0; pos < pIndices->size(); )
				{

					memDC.MoveTo( (*pVertices)[(*pIndices)[pos]].tu*xs + xt, (*pVertices)[(*pIndices)[pos]].tv*ys + yt );
					memDC.LineTo( (*pVertices)[(*pIndices)[pos+1]].tu*xs + xt, (*pVertices)[(*pIndices)[pos+1]].tv*ys + yt );
					memDC.LineTo( (*pVertices)[(*pIndices)[pos+2]].tu*xs + xt, (*pVertices)[(*pIndices)[pos+2]].tv*ys + yt );
					memDC.LineTo( (*pVertices)[(*pIndices)[pos]].tu*xs + xt, (*pVertices)[(*pIndices)[pos]].tv*ys + yt );
					pos+=3;
				}

#pragma warning( default : 4244 )

				CRect rect;
				LONG x,y;
				CBrush SelectedPointColor( RGB(255,0,0) );
				CBrush UnSelectedPointColor( RGB( 0,0,255 ) );

				//Draw points
				LONG width = 3;
				LONG height = 3;
				for( size_t pos = 0; pos < pVertices->size(); ++pos )
				{
					x = (LONG)((*pVertices)[pos].tu*xs + xt);
					y = (LONG)((*pVertices)[pos].tv*ys + yt);

					rect.left = x - width;
					rect.top = y - height;
					rect.right = x + width;
					rect.bottom = y + height;

					if( (*pVertices)[pos].selected )
						memDC.FillRect( &rect, &SelectedPointColor );
					else
						memDC.FillRect( &rect, &UnSelectedPointColor );
				}

			}

		}
	}

	pDC->BitBlt( 0,20, ClientRect.right-ClientRect.left, ClientRect.bottom - ClientRect.top, &memDC, 0,20, SRCCOPY );

	membmp.DeleteObject();
	memDC.DeleteDC();

	CGenericPane::OnPaint();

}

void CTexCoordsEditorView::OnSize( UINT nType, int cx, int cy )
{
	m_TexCoordsViewportRect.top = 100;
	m_TexCoordsViewportRect.left = 5;
	m_TexCoordsViewportRect.right = cx - 5;
	m_TexCoordsViewportRect.bottom = 400;

	CGenericPane::OnSize( nType, cx, cy );
}


void CTexCoordsEditorView::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bMouseLButtonDown = false;
	m_MouseLButtonDownPoint = point;
}
void CTexCoordsEditorView::OnLButtonDown( UINT nFlags, CPoint point )
{
	m_bMouseLButtonDown = true;
	m_MouseLButtonDownPoint = point;

	SelectTool( m_MouseLButtonDownPoint, point );
}
void CTexCoordsEditorView::OnRButtonUp( UINT nFlags, CPoint point )
{
	CWnd::OnRButtonDown( nFlags, point );

	m_bMouseRButtonDown = false;
	m_MouseRButtonDownPoint = point;
}

void CTexCoordsEditorView::OnRButtonDown( UINT nFlags, CPoint point )
{
	CScene* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();

	m_bMouseRButtonDown = true;
	m_MouseRButtonDownPoint = point;
	CWnd::OnRButtonDown( nFlags, point );
}

void CTexCoordsEditorView::OnMButtonUp( UINT nFlags, CPoint point )
{
	CScene* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();
	m_bMouseMButtonDown = false;
}

void CTexCoordsEditorView::OnMButtonDown( UINT nFlags, CPoint point )
{
	CScene* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();



	m_bMouseMButtonDown = true;
	m_MouseMButtonDownPoint = point;
}

void CTexCoordsEditorView::OnMouseMove( UINT nFlags, CPoint point )
{
	CScene* pActiveScene = CApplicationManager::GetInstance()->GetActiveScene();


	if( m_bMouseLButtonDown )
	{
		CPoint pt;
		pt.x = (point.x - m_MouseLButtonDownPoint.x);
		pt.y = (point.y - m_MouseLButtonDownPoint.y);
		
		Move( pt.x, pt.y );
		m_MouseLButtonDownPoint = point;
	}

}

BOOL CTexCoordsEditorView::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	return true;
}

