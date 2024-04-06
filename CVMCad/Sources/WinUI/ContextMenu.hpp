/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ContextMenu.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-30
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __CONTEXTMENU_HPP__
#define __CONTEXTMENU_HPP__

class ICommandExecutor;

class CPropertiesViewCMenu : public CMenu, public ICommandExecutor
{
public:
	CPropertiesViewCMenu();
	virtual ~CPropertiesViewCMenu();
public:
	BOOL Create( CWnd* pParentWnd );
	void Enable( uint nID, bool bEnable );
	void Check( uint nID, bool bCheck );
	void Show( CWnd* pWnd, CPoint& pt );

private:
	CMenu* m_pContextMenu;


};

class CGeometryViewCMenu : public CMenu, public ICommandExecutor
{
public:
	CGeometryViewCMenu();
	virtual ~CGeometryViewCMenu();
public:
	BOOL Create( CWnd* pParentWnd );
	void Enable( uint nID, bool bEnable );
	void Check( uint nID, bool bCheck );
	void Show( CWnd* pWnd, CPoint& pt );

private:
	CMenu* m_pContextMenu;


};

#endif //__CONTEXTMENU_HPP__
