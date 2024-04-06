/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: TexCoordsEditorView.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-19
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __TEXCOORDSEDITORVIEW_HPP__
#define __TEXCOORDSEDITORVIEW_HPP__

using namespace std;
using namespace boost;

class IBaseView;
class CGraphicsDevice;
class CScene;
class CMesh;
class CGenericPane;

class CTexCoordsEditorView : public CGenericPane, IBaseView
{
public:
	CTexCoordsEditorView();
	virtual ~CTexCoordsEditorView();

public:
	BOOL Create( CWnd* pParent );

	//Tools
public:
	void SelectTool( CPoint& pt0, CPoint& pt1 );
	void Move( int dx, int dy );

	//Notification
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
	float m_fX;
	float m_fY;
	float m_fWidth;
	float m_fHeight;

	bool m_bMouseLButtonDown;
	bool m_bMouseRButtonDown;
	bool m_bMouseMButtonDown;
	CPoint m_MouseLButtonDownPoint;
	CPoint m_MouseRButtonDownPoint;
	CPoint m_MouseMButtonDownPoint;
	CRect m_TexCoordsViewportRect;

};



#endif //__TEXCOORDSEDITORVIEW_HPP__
