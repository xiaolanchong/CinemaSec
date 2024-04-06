#include "stdafx.h"
#include ".\roomchooser.h"

// NOTE : obsolete, use RoomChooserTab

RoomChooser::RoomChooser(void) : 
//	m_pWnd(0),
	m_nCurCinemaIndex(CB_ERR),
	m_nCurRoomIndex(CB_ERR)
{
}

RoomChooser::~RoomChooser(void)
{
}

void RoomChooser::CComboSubclass::HookWindow(CWnd* pWnd, RoomChooser* pThis)
{
	m_pThis = pThis;
	CSubclassWnd::HookWindow( pWnd );
}


LRESULT RoomChooser::CComboSubclass::WindowProc(UINT msg, WPARAM wp, LPARAM lp)
{
	if(msg != WM_COMMAND ) return Default();
	HWND hWnd = (HWND)lp;
	UINT NotMsg = (wp >> 16) & 0xFFFF;
	if(  NotMsg == CBN_SELCHANGE  )
	{
		if		( hWnd == m_pThis->m_wndCinema.GetSafeHwnd() ) m_pThis->ChangeCinemaInControl();
		else if	( hWnd == m_pThis->m_wndRoom.GetSafeHwnd() ) m_pThis->ChangeRoomInControl();
	}
	return Default();
}

void	RoomChooser::Init(CWnd* pWnd, IRoomChooserCB * pCallback, const std::map< int, std::tstring >& Cinemas)
{
	m_pCallback	= pCallback;
	m_ComboSubclass.HookWindow( pWnd, this );

	std::map< int, std::tstring >::const_iterator itCinR = Cinemas.begin();
	for( ; itCinR != Cinemas.end(); ++itCinR  )
	{
		int nIndex = m_wndCinema.AddString( itCinR->second.c_str() );
		m_wndCinema.SetItemData( nIndex, itCinR->first );
	}

	m_wndCinema.SetCurSel( 0 );
	ChangeCinemaInControl();
	m_wndRoom.SetCurSel(0);
	ChangeRoomInControl();
}

int		RoomChooser::GetCurRoom()
{
	int nIndex = m_wndRoom.GetCurSel();
	if( nIndex != CB_ERR)
	{
		return m_wndRoom.GetItemData( nIndex );
	}
	return -1;
}

int		RoomChooser::GetCurCinema()
{
	int nIndex = m_wndCinema.GetCurSel();
	if( nIndex != CB_ERR )
	{
		return m_wndCinema.GetItemData( nIndex );
	}
	return -1;
}

void RoomChooser::ChangeCinemaInControl()
{
	int nIndex = m_wndCinema.GetCurSel();
	if( nIndex != CB_ERR && m_nCurCinemaIndex != nIndex)
	{
		int nCinemaID = m_wndCinema.GetItemData( nIndex );
		m_nCurCinemaIndex = nIndex;
		ChangeCinema( nCinemaID );
	}
}

void RoomChooser::ChangeRoomInControl()
{
	int nIndex = m_wndRoom.GetCurSel();
	if( nIndex != CB_ERR && m_nCurRoomIndex != nIndex )
	{
		int nRoomID = m_wndRoom.GetItemData( nIndex );
		m_nCurRoomIndex = nIndex;
		ChangeRoom( nRoomID );
	}
}

void	RoomChooser::ChangeCinema( int nCinemaID )
{
	std::set<int> Rooms = m_pCallback->OnCinemaChange( nCinemaID );
	std::set<int>::const_iterator itRoomR = Rooms.begin();
	m_wndRoom.ResetContent();
	for( ; itRoomR != Rooms.end(); ++itRoomR  )
	{
		CString strName;
		strName.Format(_T("%d"), *itRoomR );
		int nIndex = m_wndRoom.AddString( strName );
		m_wndRoom.SetItemData( nIndex, *itRoomR );
	}
	m_nCurRoomIndex = CB_ERR;
	m_wndRoom.SetCurSel( 0 );
	ChangeRoomInControl();
}

void	RoomChooser::ChangeRoom( int nRoomID )
{
	m_pCallback->OnRoomChange( nRoomID );
}

