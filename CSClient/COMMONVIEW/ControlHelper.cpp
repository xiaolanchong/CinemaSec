//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		Add code for tooltips and message as CFrameWnd normally does. Use it for non-CFrameWnd parent windows
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   26.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "ControlHelper.h"
#include <afxpriv.h>
//=====================================================================================//
//                                 class ControlHelper                                 //
//=====================================================================================//
ControlHelper::ControlHelper()
{
}

ControlHelper::~ControlHelper()
{
}

LRESULT ControlHelper::WindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( msg == WM_SETMESSAGESTRING )
	{
		return OnSetMessageString( wParam, lParam );
	}
	else if( msg == WM_NOTIFY/* && wParam*/)
	{
//		UINT nID = wParam;
		NMHDR* pHdr = (NMHDR*)lParam;
		if( pHdr->code == TTN_NEEDTEXTW ||
			pHdr->code == TTN_NEEDTEXTA )
		{
			return OnToolTipText( wParam, lParam );
		}
		else return Default();
	}
	else return Default();
}

BOOL ControlHelper::Attach( CWnd* pWnd)
{
	return HookWindow( pWnd );
}

void ControlHelperWithFrame ::Attach( CWnd* pWnd, CFrameWnd* pFrameWnd )
{
	ControlHelper::Attach(pWnd);
	m_pFrameWnd = pFrameWnd;
}

LRESULT ControlHelperWithFrame ::OnSetMessageString(WPARAM wParam, LPARAM lParam) 
{
	if( m_pFrameWnd )
		return m_pFrameWnd->SendMessage( WM_SETMESSAGESTRING, wParam, lParam );
	else 
		return  FALSE;
}

LRESULT	ControlHelperWithFrame ::OnToolTipText(WPARAM wParam, LPARAM lParam) 
{
	if( m_pFrameWnd )
		return m_pFrameWnd->SendMessage( WM_NOTIFY, wParam, lParam );
	else 
		return  FALSE;
}
