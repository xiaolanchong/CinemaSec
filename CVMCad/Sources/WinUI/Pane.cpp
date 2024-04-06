/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Pane.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-08
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "BaseView.hpp"
#include "Pane.hpp"

CGenericPane::CGenericPane()
{
	m_DefIndentRect.left	= 5;
	m_DefIndentRect.right	= 5;
	m_DefIndentRect.bottom	= 5;
	m_DefIndentRect.top		= 5;

	m_DefCaptionRect.left	= 2;
	m_DefCaptionRect.right	= 2;
	m_DefCaptionRect.top	= 2;
	m_DefCaptionRect.bottom = 18;

	m_ClientRect = CRect(0,0,0,0);
}

CGenericPane::~CGenericPane()
{
	

}


BOOL CGenericPane::Create( CWnd* pParent, LPCTSTR lpsWindowName )
{
	if ( !CWnd::Create( NULL, lpsWindowName, NULL,
		CRect(0,0,0,0), pParent, NULL, NULL ) )
	{
		//FIX: Throw
	}

	//m_wndSizingWnd.Create( L"", WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, NULL );
	//m_wndSizingWnd.OnSetCursor()

	return TRUE;
}

void CGenericPane::CalcClientRect( CRect* rect )
{
	*rect = m_ClientRect;
}

BOOL CGenericPane::PreCreateWindow( CREATESTRUCT& cs )
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~WS_VISIBLE;
	cs.style |= WS_CHILD|WS_OVERLAPPED|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	cs.lpszClass = AfxRegisterWndClass( 
		CS_HREDRAW|CS_VREDRAW, 
		::LoadCursor(NULL, IDC_ARROW), 
		CreateSolidBrush(GetSysColor( CTLCOLOR_DLG )),
		NULL);
	return TRUE;
}

BEGIN_MESSAGE_MAP( CGenericPane, CWnd )
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

INT CGenericPane::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CGenericPane::OnSize( UINT nType, int cx, int cy )
{
	m_ClientRect.left	= m_DefIndentRect.left;
	m_ClientRect.right	= cx - m_DefIndentRect.right;
	m_ClientRect.top	= m_DefIndentRect.top + m_DefCaptionRect.bottom;
	m_ClientRect.bottom = cy - m_DefIndentRect.bottom;
}

void CGenericPane::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect( &rect );
	
	rect.left	= m_DefCaptionRect.left;
	rect.right -= m_DefCaptionRect.right;
	rect.top	= m_DefCaptionRect.top;
	rect.bottom	= m_DefCaptionRect.bottom;

	DrawCaption( &dc, &rect, DC_SMALLCAP|DC_ACTIVE|DC_TEXT );
}


void CGenericPane::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	__asm nop;
}

void  CGenericPane::OnRButtonUp( UINT nFlags, CPoint point )
{
	__asm nop
}

void  CGenericPane::OnRButtonDown( UINT nFlags, CPoint point )
{
	__asm nop
}


