/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ContextMenu.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-30
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
#include "ContextMenu.hpp"

CPropertiesViewCMenu::CPropertiesViewCMenu()
{
	CMenu* m_pContextMenu = NULL;
}

CPropertiesViewCMenu::~CPropertiesViewCMenu()
{

}

BOOL CPropertiesViewCMenu::Create( CWnd* pParentWnd )
{
	if( !LoadMenu( IDR_PROPERTIES_VIEW_CONTEXT_MENU ) )
		return false;

	m_pContextMenu = GetSubMenu(0);

	CCommandManager* pCM = CCommandManager::GetInstance();

	pCM->AddCommandExecutor( ID_CMENU_ADD_CAMERA, this );
	pCM->AddCommandExecutor( ID_CMENU_ADD_ELEMENT, this );
	pCM->AddCommandExecutor( ID_CMENU_SELECT_CAMERA_SOURCE, this );
	pCM->AddCommandExecutor( ID_CMENU_REMOVE_ELEMENT, this );
	pCM->AddCommandExecutor( ID_CMENU_REMOVE_CAMERA, this );
	pCM->AddCommandExecutor( ID_CMENU_ADD_HELPER_ELEMENT, this );
	pCM->AddCommandExecutor( ID_CMENU_MAKE_ACTIVE_ELEMENT, this );
	pCM->AddCommandExecutor( ID_CMENU_MAKE_ACTIVE_CAMERA, this );
	pCM->AddCommandExecutor( ID_CMENU_TRIANGULATE_ELEMENTS_POINTS, this );
	pCM->AddCommandExecutor( ID_CMENU_APPLY_NORMAL_MAPPING, this );


	return TRUE;
}

void CPropertiesViewCMenu::Enable( uint nID, bool bEnable )
{
	EnableMenuItem( nID, bEnable? MF_ENABLED : MF_GRAYED );
}

void CPropertiesViewCMenu::Check( uint nID, bool bCheck )
{
	CheckMenuItem( nID, bCheck ? MF_CHECKED : MF_UNCHECKED );
}

void CPropertiesViewCMenu::Show( CWnd* pWnd, CPoint& pt )
{
	//Call CommandManager to update commands state
	CCommandManager::GetInstance()->UpdateCommands();

	ASSERT(m_pContextMenu);
	m_pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, pWnd );
}

//////////////////////////////////////////////////////////////////////////

CGeometryViewCMenu::CGeometryViewCMenu()
{
	CMenu* m_pContextMenu = NULL;
}

CGeometryViewCMenu::~CGeometryViewCMenu()
{

}

BOOL CGeometryViewCMenu::Create( CWnd* pParentWnd )
{
	if( !LoadMenu( IDR_GEOMETRY_VIEW_CONTEXT_MENU ) )
		return false;

	m_pContextMenu = GetSubMenu(0);

	CCommandManager* pCM = CCommandManager::GetInstance();

	pCM->AddCommandExecutor( ID_CMENU_ADD_POINT, this );
	pCM->AddCommandExecutor( ID_CMENU_REMOVE_POINTS, this );

	pCM->AddCommandExecutor( ID_TOOL_HORIZONTAL_ALIGN, this );
	pCM->AddCommandExecutor( ID_TOOL_VERTICAL_ALIGN, this );


	return TRUE;
}

void CGeometryViewCMenu::Enable( uint nID, bool bEnable )
{
	EnableMenuItem( nID, bEnable? MF_ENABLED : MF_GRAYED );
}

void CGeometryViewCMenu::Check( uint nID, bool bCheck )
{
	CheckMenuItem( nID, bCheck ? MF_CHECKED : MF_UNCHECKED );
}

void CGeometryViewCMenu::Show( CWnd* pWnd, CPoint& pt )
{
	//Call CommandManager to update commands state
	CCommandManager::GetInstance()->UpdateCommands();

	ASSERT(m_pContextMenu);
	m_pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, pWnd );
}