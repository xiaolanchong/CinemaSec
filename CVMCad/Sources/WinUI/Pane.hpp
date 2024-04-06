/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Pane.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-08
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __PANE_HPP__
#define __PANE_HPP__

class IBaseView;

class CGenericPane : public CWnd
{
public:
	CGenericPane();
	virtual ~CGenericPane();

	//Methods
public:
	BOOL	Create( CWnd* pParent, LPCTSTR lpsWindowName );
	void	CalcClientRect( CRect* rect );

	//Overrides
protected:
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs );

protected:
	afx_msg INT	 OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnPaint();
	afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnRButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	DECLARE_MESSAGE_MAP();

private:
	CRect m_DefIndentRect;
	CRect m_DefCaptionRect;
	CRect m_ClientRect;
	CStatic  m_wndSizingWnd;


};



#endif //__PANE_HPP__
