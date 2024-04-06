/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Application.cpp
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
#include "Configuration.hpp"
#include "Model.hpp"
#include "Graphics.hpp"
#include "WinUI.hpp"
#include "Application.hpp"

using namespace Gdiplus;

//CApplication instance
CApplication	theApplication;


CApplication::CApplication()
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	CoInitialize(NULL);
}

CApplication::~CApplication()
{
	GdiplusShutdown(gdiplusToken);
	CoUninitialize();
}

BOOL CApplication::InitInstance()
{
	InitCommonControls();
	AfxEnableControlContainer();
	RegisterCommands();

	DWORD dwValue;
	CRegKey key;

	if(  ERROR_SUCCESS == key.Open( HKEY_LOCAL_MACHINE, L"SOFTWARE\\ELVEES\\Common", KEY_READ  ) 
		&& ERROR_SUCCESS == key.QueryDWORDValue( L"LangLCID", dwValue ) )
	{
		SetThreadLocale( dwValue );
	}

	CWinApp::InitInstance();

	//Create main window
	m_pMainWindow = new CMainWindow();
	m_pMainWnd = m_pMainWindow;
	m_pMainWindow->Create( CRect(0,0,700,600) );

	m_pMainWindow->CenterWindow();
	m_pMainWindow->ShowWindow(SW_SHOW);
	m_pMainWindow->UpdateWindow();

//	throw 0;


	return TRUE;
}

void CApplication::RegisterCommands()
{
	CCommandManager* pCM = CCommandManager::GetInstance();

	//File commands
	pCM->RegisterCommand( new CFileNewCommand() );
	pCM->RegisterCommand( new CFileCloseCommand() );
	pCM->RegisterCommand( new CFileSaveCommand() );
	pCM->RegisterCommand( new CFileOpenCommand() );
	pCM->RegisterCommand( new CFileExitCommand() );

	//Context menu commands
	pCM->RegisterCommand( new CAddCameraCommand() );
	pCM->RegisterCommand( new CRemoveCameraCommand() );
	pCM->RegisterCommand( new CAddElementCommand() );
	pCM->RegisterCommand( new CRemoveElementCommand() );
	pCM->RegisterCommand( new CAddPointCommand() );
	pCM->RegisterCommand( new CRemovePointsCommand() );

	pCM->RegisterCommand( new CMakeActiveElementCommand() );
	pCM->RegisterCommand( new CMakeActiveCameraCommand() );

	pCM->RegisterCommand( new CTriangulateCommand() );
	pCM->RegisterCommand( new CSelectCameraSourceCommand() );
	pCM->RegisterCommand( new CApplyNormalMappingCommand() );

	//GeomentryView-specific commands
	pCM->RegisterCommand( new CGVSelectTool() );
	pCM->RegisterCommand( new CGVPanTool() );
	pCM->RegisterCommand( new CGVZoomTool() );
	pCM->RegisterCommand( new CGVVerticalAlignCommand() );
	pCM->RegisterCommand( new CGVHorizontalAlignCommand() );

}

BOOL CApplication::OnIdle( LONG lCount )
{

	//Disable this, because all cmd routing done via CCommandManager
	//CWinApp::OnIdle( lCount );

	//Call CommandManager to update commands state
	CCommandManager::GetInstance()->UpdateCommands();

	return FALSE;
}

