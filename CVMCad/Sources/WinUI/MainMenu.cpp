/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: MainMenu.cpp
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
#include "CommandExecutor.hpp"
#include "CommandManager.hpp"
#include "MainMenu.hpp"

CMainMenu::CMainMenu()
{

}

CMainMenu::~CMainMenu()
{

}

BOOL CMainMenu::Create( CWnd* pParentWnd )
{
	BOOL bResult;
	LoadMenu( IDR_MAIN_MENU );
	bResult = pParentWnd->SetMenu( this );

	CCommandManager* pCM = CCommandManager::GetInstance();
	
	//File
	pCM->AddCommandExecutor( ID_MENU_FILE_NEW, this );
	pCM->AddCommandExecutor( ID_MENU_FILE_CLOSE, this );
	pCM->AddCommandExecutor( ID_MENU_FILE_OPEN, this );
	pCM->AddCommandExecutor( ID_MENU_FILE_SAVE, this );
	pCM->AddCommandExecutor( ID_MENU_FILE_EXIT, this );

	//View
	pCM->AddCommandExecutor( ID_TOOL_SELECT, this );
	pCM->AddCommandExecutor( ID_TOOL_PAN, this );
	pCM->AddCommandExecutor( ID_TOOL_ZOOM, this );


	return bResult;

}

void CMainMenu::Enable( uint nID, bool bEnable )
{
	EnableMenuItem( nID, bEnable? MF_ENABLED : MF_GRAYED );
}

void CMainMenu::Check( uint nID, bool bCheck )
{
	CheckMenuItem( nID, bCheck ? MF_CHECKED : MF_UNCHECKED );
}