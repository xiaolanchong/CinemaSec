/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: GeometryView.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-15
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __GEOMETRYVIEW_HPP__
#define __GEOMETRYVIEW_HPP__

class IBaseView;
class CGraphicsDevice;
class CScene;
class CMesh;
class CCameraSource;
struct CGeometry;

using namespace std;
using namespace boost;

class CGeometryView : public CWnd, public IBaseView
{
public:
	struct SceneSegment
	{
		CCamera* pCamera;
		shared_ptr<CTexture> pTexture;
		vector< pair<CElement*, shared_ptr<CMesh> > > geometry;
	};

public:
	CGeometryView();
	virtual ~CGeometryView();

public:
	BOOL Create( CWnd* pParent );
	BOOL PreCreateWindow( CREATESTRUCT& cs );
	BOOL OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo );

	//Tools
public:
	int  GetActiveTool();
	void ChangeTool( int t );
	void Pan( int dx, int dy );
	void Zoom( CPoint& pt, float zDelta );
	void Select( CPoint& pt0, CPoint& pt1 );
	void GetPan( float& x, float& y );

	void GetCMenuPoint( CPoint& pt );
	

public:
	void OnUpdate();

protected:
	afx_msg	INT	 OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnDestroy();
	afx_msg void OnSize( UINT nType, int cx, int cy );

	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnRButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnMButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnMButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP();

private:
	//Context menu
	CGeometryViewCMenu m_wndCMenu;

	//Array for temporary vertices
	VertexPtrArray m_GVPtrVertices;
	VertexArray m_GVVertices;

	CPointyMesh m_PointyMesh;
	CThinBorderMesh m_SelectionRectMesh;

	//Scene presentation
	std::vector<SceneSegment> m_ScenePresentation;
	std::vector<CCamera*> m_TempCamerasArray;
	std::vector<CElement*> m_TempElementsArray;


	//Tools stuff
	int m_ToolType;
	bool m_bMoveHit;
	bool m_bSelectionHit;
	bool m_bMouseLButtonDown;
	bool m_bMouseRButtonDown;
	bool m_bMouseMButtonDown;
	CPoint m_MouseLButtonDownPoint;
	CPoint m_InitialMouseLButtonDownPoint;
	CPoint m_MouseRButtonDownPoint;
	CPoint m_MouseMButtonDownPoint;
	float m_fPanX;
	float m_fPanY;
};



#endif //__GEOMETRYVIEW_HPP__
