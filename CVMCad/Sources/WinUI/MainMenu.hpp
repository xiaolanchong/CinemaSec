/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: MainMenu.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-19
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __MAINMENU_HPP__
#define __MAINMENU_HPP__

class ICommandExecutor;

class CMainMenu : public CMenu, public ICommandExecutor
{
public:
	CMainMenu();
	virtual ~CMainMenu();
public:
	BOOL Create( CWnd* pParentWnd );
	void Enable( uint nID, bool bEnable );
	void Check( uint nID, bool bCheck );


};



#endif //__MAINMENU_HPP__
