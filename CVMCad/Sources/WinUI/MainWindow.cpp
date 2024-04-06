/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: MainWindow.cpp
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
#include "Exceptions.hpp"
#include "Model.hpp"
#include "Graphics.hpp"
#include "WinUI.hpp"

CMainWindow::CMainWindow()
{
	m_sClientTitle = L"";
	m_bAutoMenuEnable = false;
}

CMainWindow::~CMainWindow()
{

}

//////////////////////////////////////////////////////////////////////////
BOOL CMainWindow::Create( const CRect& rect )
{

	try
	{
		CApplicationConfig* pAC =  CApplicationConfig::GetInstance();

		wstring sAppTitle;
		pAC->GetAppTitle( sAppTitle );

		if( !CFrameWnd::Create( NULL, sAppTitle.c_str(), 0, rect ))
			return FALSE;



		//Create GD and first swap chain
		CGraphicsDevice::GetInstance()->Create( this );

		//Create menu
		m_wndMainMenu.Create( this );

		//Create toolbar 
		m_wndMainToolbar.Create( this );

		//Create statusbar
		m_wndMainStatusBar.Create( this );

		//Create channelbars
		m_wndLeftChannelBar.Create( this, CChannelBar::Position::LeftAligned );
		m_wndRightChannelBar.Create( this, CChannelBar::Position::RightAligned );

		//Create channels
		m_wndTexCoordsView.Create( this );
		m_wndPropertiesView.Create( this );

		//Add channels to channelbar
		m_wndRightChannelBar.AddPane( &m_wndTexCoordsView );
		m_wndLeftChannelBar.AddPane( &m_wndPropertiesView );

		//Create graphics view
		m_wndGraphicsView.Create( this );

		
	}
	catch ( CBaseException<CGraphicsDevice> e )
	{
		e.DisplayErrorMessage();
		DestroyWindow();
	}

	return TRUE;
}


BOOL CMainWindow::PreCreateWindow( CREATESTRUCT& cs )
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.lpszClass = AfxRegisterWndClass( 
		CS_HREDRAW | CS_VREDRAW, 
		AfxGetApp()->LoadStandardCursor( IDC_ARROW ),
		CreateSolidBrush(GetSysColor(COLOR_3DDKSHADOW)),
		AfxGetApp()->LoadIcon( IDI_APPLICATION ) );

	cs.dwExStyle &= ~WS_EX_OVERLAPPEDWINDOW;
	cs.style |= WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_SYSMENU;

	return TRUE;
}


BOOL CMainWindow::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo )
{
	//Redirect all command messages to CommandManager
	if( nCode == CN_COMMAND && CCommandManager::GetInstance()->ProcessCommand( nID ) )
		return true;

	return CFrameWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}


//////////////////////////////////////////////////////////////////////////
CMainStatusBar* CMainWindow::GetMainStatusBar()
{
	return &m_wndMainStatusBar;
}

CMainToolBar* CMainWindow::GetMainToolbar()
{
	return &m_wndMainToolbar;
}

CMainMenu* CMainWindow::GetMainMenu()
{
	return &m_wndMainMenu;
}


CGeometryView* CMainWindow::GetGeometryView()
{
	return &m_wndGraphicsView;
}

CPropertiesView* CMainWindow::GetPropertiesView()
{
	return &m_wndPropertiesView;
}

void CMainWindow::SetClientTitle( const wstring& sClientTitle )
{
	CApplicationConfig* pAC =  CApplicationConfig::GetInstance();

	wstring sAppTitle;
	wstring sNewTitle;

	pAC->GetAppTitle( sAppTitle );
	sNewTitle = sAppTitle + L" - " + sClientTitle;

	m_sClientTitle = sClientTitle;

	if( sClientTitle.empty() )
		SetWindowText( sAppTitle.c_str() );
	else
		SetWindowText( sNewTitle.c_str() );
}

void CMainWindow::GetClientTitle( wstring& sClientTitle )
{
	sClientTitle = m_sClientTitle;
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC( CMainWindow, CFrameWnd )
//{{AFX_MESSAGE_MAP( CMainWindow )
BEGIN_MESSAGE_MAP( CMainWindow, CFrameWnd )
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
END_MESSAGE_MAP()
//}}AFX_MESSAGE_MAP

INT CMainWindow::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CMainWindow::OnSize( UINT nType, int cx, int cy )
{
	if (!IsWindowVisible())
		return;

	CSize MainStatusbarSize( 0, 20 );
	CSize MainToolbarSize( 0,28 );

	//Resize main channelbar
	CSize LeftChannelBar( 20, 0 );
	m_wndLeftChannelBar.SetWindowPos( NULL, 0, MainToolbarSize.cy, LeftChannelBar.cx,
		cy - ( MainToolbarSize.cy + MainStatusbarSize.cy ), SWP_NOACTIVATE|SWP_NOZORDER  );

	CSize RightChannelBar( 20,0 );
	m_wndRightChannelBar.SetWindowPos( NULL, cx - LeftChannelBar.cx, MainToolbarSize.cy, RightChannelBar.cx,
		cy - ( MainToolbarSize.cy + MainStatusbarSize.cy ), SWP_NOACTIVATE|SWP_NOZORDER  );

	//Resize main toolbar
	m_wndMainToolbar.SetWindowPos( NULL, 0,0,cx, MainToolbarSize.cy , SWP_NOACTIVATE|SWP_NOZORDER );


	//Resize main statusbar
	m_wndMainStatusBar.SetWindowPos( NULL, 0,cy-MainStatusbarSize.cy, cx, MainStatusbarSize.cy , SWP_NOACTIVATE|SWP_NOZORDER );


	//Resize graphicsview
	CRect DefMainGraphicsViewRect;

	LONG x = LeftChannelBar.cx;
	LONG y = MainToolbarSize.cy;
	LONG width = cx - ( LeftChannelBar.cx + RightChannelBar.cx );
	LONG height = cy - ( MainToolbarSize.cy + MainStatusbarSize.cy );

	m_wndGraphicsView.SetWindowPos( NULL, x,y, width, height,
		SWP_NOACTIVATE|SWP_NOZORDER  );

}
