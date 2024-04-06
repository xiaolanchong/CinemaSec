/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: MainToolbar.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-04-27
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
#include "MainToolbar.hpp"

CMainToolBar::CMainToolBar()
{
	CBitmap bmpEnabled;
	CBitmap bmpDisabled;

	bmpEnabled.LoadBitmap( IDB_MAIN_TOOLBAR_ENABLED );
	bmpDisabled.LoadBitmap( IDB_MAIN_TOOLBAR_DISABLED );

	//Create ImageList with mask
	m_TBImageListEnabled.Create( 16, 16, ILC_COLOR24|ILC_MASK, 32,32 );
	m_TBImageListEnabled.Add( &bmpEnabled, RGB(180,30,150) );
	m_TBImageListEnabled.SetBkColor(CLR_NONE);

	m_TBImageListDisabled.Create( 16, 16, ILC_COLOR24|ILC_MASK, 32,32 );
	m_TBImageListDisabled.Add( &bmpDisabled, RGB(180,30,150) );
	m_TBImageListDisabled.SetBkColor(CLR_NONE);
}

CMainToolBar::~CMainToolBar()
{		  

}

BOOL CMainToolBar::Create( CWnd* pParent )
{
	if ( !CReBar::Create( pParent ) )
		return FALSE;

	//Create toolbar
	m_wndTB.Create( WS_CHILD|WS_VISIBLE|TBSTYLE_FLAT|CBRS_ALIGN_TOP, CRect(0,0,0,0), this, IDR_MAINTOOLBAR );
	m_wndTB.SetImageList( &m_TBImageListEnabled );
	m_wndTB.SetDisabledImageList( &m_TBImageListDisabled );

	AddBar( &m_wndTB );

	CCommandManager* pCM = CCommandManager::GetInstance();

	//Add buttons..
	TBBUTTON tb;

	tb.iString = -1;
	tb.dwData  = 0;
	tb.fsState = 0;
	tb.fsStyle = TBSTYLE_BUTTON;

	tb.iBitmap = 0;
	tb.idCommand = ID_MENU_FILE_NEW;
	m_wndTB.AddButtons( 1, &tb );
	pCM->AddCommandExecutor( tb.idCommand, this );

	tb.iBitmap = 1;
	tb.idCommand = ID_MENU_FILE_OPEN;
	m_wndTB.AddButtons( 1, &tb );
	pCM->AddCommandExecutor( tb.idCommand, this );

	tb.iBitmap = 2;
	tb.idCommand = ID_MENU_FILE_SAVE;
	m_wndTB.AddButtons( 1, &tb );
	pCM->AddCommandExecutor( tb.idCommand, this );

	//Separator
	tb.fsStyle = TBSTYLE_SEP;
	m_wndTB.AddButtons( 1, &tb );
	tb.fsStyle = TBSTYLE_BUTTON;

	tb.iBitmap = 3;
	tb.idCommand = ID_MENU_EDIT_CUT;
	m_wndTB.AddButtons( 1, &tb );
	pCM->AddCommandExecutor( tb.idCommand, this );

	tb.iBitmap =4;
	tb.idCommand = ID_MENU_EDIT_COPY;
	m_wndTB.AddButtons( 1, &tb );
	pCM->AddCommandExecutor( tb.idCommand, this );

	tb.iBitmap = 5;
	tb.idCommand = ID_MENU_EDIT_PASTE;
	m_wndTB.AddButtons( 1, &tb );
	pCM->AddCommandExecutor( tb.idCommand, this );

	tb.iBitmap = 6;
	tb.idCommand = ID_MENU_EDIT_DELETE;
	m_wndTB.AddButtons( 1, &tb );
	pCM->AddCommandExecutor( tb.idCommand, this );

	tb.fsStyle = TBSTYLE_SEP;
	m_wndTB.AddButtons( 1, &tb );
	tb.fsStyle = TBSTYLE_BUTTON;

	tb.iBitmap = 8;
	tb.idCommand = ID_MENU_EDIT_UNDO;
	m_wndTB.AddButtons( 1, &tb );
	pCM->AddCommandExecutor( tb.idCommand, this );

	tb.iBitmap = 9;
	tb.idCommand = ID_MENU_EDIT_REDO;
	m_wndTB.AddButtons( 1, &tb );
	pCM->AddCommandExecutor( tb.idCommand, this );

	//Separator
	tb.fsStyle = TBSTYLE_SEP;
	m_wndTB.AddButtons( 1, &tb );
	tb.fsStyle = TBSTYLE_BUTTON;

	tb.iBitmap = 10;
	tb.idCommand = ID_TOOL_SELECT;
	m_wndTB.AddButtons( 1, &tb );
	pCM->AddCommandExecutor( tb.idCommand, this );

	tb.iBitmap = 12;
	tb.idCommand = ID_TOOL_ZOOM;
	m_wndTB.AddButtons( 1, &tb );
	pCM->AddCommandExecutor( tb.idCommand, this );

	tb.iBitmap = 13;
	tb.idCommand = ID_TOOL_PAN;
	m_wndTB.AddButtons( 1, &tb );
	pCM->AddCommandExecutor( tb.idCommand, this );


	return TRUE;
}

void CMainToolBar::Enable( uint nID, bool bEnable )
{
	m_wndTB.EnableButton( nID, bEnable );
}

void CMainToolBar::Check( uint nID, bool bCheck )
{
	m_wndTB.PressButton( nID, bCheck );
}

