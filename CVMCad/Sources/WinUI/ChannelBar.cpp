/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: ChannelBar.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-07
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "BaseView.hpp"
#include "Model.hpp"
#include "Pane.hpp"
#include "CommandExecutor.hpp"
#include "CommandManager.hpp"
#include "ContextMenu.hpp"
#include "PropertiesView.hpp"
#include "ChannelBar.hpp"

	  
CChannelBar::CChannelBar()
{
	//Default channel button size
	m_DefButtonSize = CSize(20,40);

	//Default indent between button and control borders
	m_DefIndent = CRect( 2,2,2,2 );

	m_pActivePane = NULL;
}

CChannelBar::~CChannelBar()
{

}

BOOL CChannelBar::Create( CWnd* pParent, CChannelBar::Position p )
{
	if (!CWnd::CreateEx( NULL, NULL, NULL, WS_CHILD|WS_VISIBLE,
		CRect(0,0,0,0), pParent, 0 ) )   
	{
		return FALSE;
	}

	m_ePosition = p;

	CRect rcDefRect;
	pParent->GetWindowRect(&rcDefRect);
	m_DefSize.cx = m_DefIndent.left + m_DefIndent.right + m_DefButtonSize.cx;
	m_DefSize.cy = rcDefRect.bottom - rcDefRect.top;

	return TRUE;
}


CSize CChannelBar::GetDefSize()
{
	return m_DefSize;
}

BOOL CChannelBar::PreCreateWindow( CREATESTRUCT& cs )
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.style &= ~WS_DLGFRAME|WS_CLIPSIBLINGS;
	cs.lpszClass = AfxRegisterWndClass( 
		CS_HREDRAW | CS_VREDRAW, 
		::LoadCursor(NULL, IDC_ARROW), 
		CreateSolidBrush(GetSysColor(COLOR_WINDOW)),
		NULL);

	return TRUE;
}



void CChannelBar::AddPane( CGenericPane* pPane )
{
	m_pActivePane = pPane;

}

void CChannelBar::RemovePane( CGenericPane* pPane )
{

}

BEGIN_MESSAGE_MAP( CChannelBar, CWnd )
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CChannelBar::OnSize( UINT nType, int cx, int cy )
{
	if (!IsWindowVisible() || m_pActivePane == NULL )
		return;

	CRect rect,parentRect, paneRect;
	GetWindowRect( &rect );	
	GetParent()->ScreenToClient( &rect );
	GetParent()->GetClientRect( &parentRect );

	int x,y;
	int width, height;

	width = 200;
	height = cy;

	if( m_ePosition == Position::LeftAligned )
	{
		x = cx;
		y = rect.top;
	}
	else
	{
		width = 500;
		x = parentRect.right - ( cx + width );
		y = rect.top;
	}

	m_pActivePane->SetWindowPos( NULL, x,y, width,height,  SWP_NOZORDER );
//	m_pActivePane->BringWindowToTop();
}

void CChannelBar::OnPaint()
{
	CPaintDC dc(this);

}

void CChannelBar::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( m_pActivePane == NULL )
		return;

	static bool bTrig = false;

	if ( bTrig )
	{
		m_pActivePane->ShowWindow( SW_HIDE );
		bTrig = false;
	}else
	{
		m_pActivePane->ShowWindow( SW_SHOW );
		bTrig = true;
	}
}


