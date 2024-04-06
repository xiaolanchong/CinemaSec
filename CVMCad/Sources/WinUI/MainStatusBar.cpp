/*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: MainStatusBar.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-05
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "MainStatusBar.hpp"

CMainStatusBar::CMainStatusBar()
{


}

CMainStatusBar::~CMainStatusBar()
{



}

BOOL CMainStatusBar::Create( CWnd* pParent )
{
	if( !CStatusBar::CreateEx( pParent ) )
		return FALSE;

	CRect rcDefRect;
	pParent->GetWindowRect(&rcDefRect);
	m_DefSize.cx = rcDefRect.right - rcDefRect.left;
	m_DefSize.cy = 20;

	return TRUE;
}

CSize CMainStatusBar::GetDefSize()
{
	return m_DefSize;
}