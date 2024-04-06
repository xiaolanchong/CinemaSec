/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: GraphicsView.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-05
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __GRAPHICSVIEW_HPP__
#define __GRAPHICSVIEW_HPP__

class IBaseView;
class CGraphicsDevice;
class CSceneDoc;
class CBaseMesh;
class CBorderMesh;

class CGraphicsView : public CWnd, public IBaseView
{
public:
	CGraphicsView();
	virtual ~CGraphicsView();
public:
	enum ViewMode
	{
		TexCoordEditingMode,
		FullSceneMode
	};

public:
	BOOL Create( CWnd* pParent );
	BOOL PreCreateWindow( CREATESTRUCT& cs );

//Notification
public:
	void OnUpdate();
	void ChangeViewMode( ViewMode m );


//View
private:
	struct VCameraInfo
	{
		bool bChecked;
        CVirtualCamera* pCamera;
        CBorderMesh* pBorderMesh;
	};



//View
private:
	void RenderView();
	void ClearView();

	void RenderFullSceneView();
	void RenderTexCoordEditingView();

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
	DECLARE_DYNCREATE( CGraphicsView );

private:
	bool m_bMouseLButtonDown;
	bool m_bMouseRButtonDown;
	bool point_selected;
	CPoint m_MouseLButtonDownPoint;

	ViewMode m_ActiveViewMode;
	CRect m_GeometryViewRect;
	CRect m_SceneViewRect;
	CRect m_EditingViewRect;
	list<VCameraInfo> m_CamerasInfo;
	vector<Vertex>::iterator  m;
	CBaseElement* pe;
};



#endif //__GRAPHICSVIEW_HPP__
