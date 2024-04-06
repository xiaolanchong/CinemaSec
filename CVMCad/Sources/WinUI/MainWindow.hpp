/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: MainWindow.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-04-27
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
//#pragma once
#ifndef __MAINWINDOW_HPP__
#define __MAINWINDOW_HPP__


class CMainWindow : public CFrameWnd
{

public:
	CMainWindow();
	virtual ~CMainWindow();

public:
	BOOL Create( const CRect&  rect );
	BOOL PreCreateWindow(CREATESTRUCT& cs);
	BOOL OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo );

public:
	CMainStatusBar* GetMainStatusBar();
	CMainToolBar*	GetMainToolbar();
	CMainMenu*		GetMainMenu();
	CGeometryView*	GetGeometryView();	
	CPropertiesView* GetPropertiesView();
	void SetClientTitle( const wstring& sClientTitle );
	void GetClientTitle( wstring& sClientTitle );


private:
	wstring			m_sClientTitle;
	CMainMenu		m_wndMainMenu;
	CMainToolBar	m_wndMainToolbar;
	CMainStatusBar	m_wndMainStatusBar;
	CGeometryView	m_wndGraphicsView;
	CChannelBar		m_wndLeftChannelBar;
	CChannelBar		m_wndRightChannelBar;
	CPropertiesView m_wndPropertiesView;
	CTexCoordsEditorView	m_wndTexCoordsView;
	
protected:
	afx_msg	INT	 OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize( UINT nType, int cx, int cy );
	DECLARE_MESSAGE_MAP();
	DECLARE_DYNAMIC(CMainWindow);

public:

};


#endif //__MAINWINDOW_HPP__
