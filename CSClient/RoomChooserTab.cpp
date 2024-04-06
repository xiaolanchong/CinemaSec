//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   20.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#include "stdafx.h"
#include "RoomChooserTab.h"
#include "DBFacet/DBSerializer.h"

// for combobox it's TAB_ERR, for listbox - LB_ERR
#define TAB_ERR		-1

void SetItemData( CTabCtrl& tc, int nIndex, LPARAM Data)
{
	TCITEM ti;
	ti.lParam = Data;
	ti.mask = TCIF_PARAM;
	tc.SetItem( nIndex, &ti);
}

LPARAM GetItemData( CTabCtrl& tc, int nIndex )
{
	TCITEM ti;
	ti.mask = TCIF_PARAM;
	tc.GetItem( nIndex, &ti);
	return ti.lParam;
}

void RoomChooserTab::CTabSubclass::HookWindow(CWnd* pWnd, RoomChooserTab* pThis)
{
	m_pThis = pThis;
	CSubclassWnd::HookWindow( pWnd );
}

LRESULT RoomChooserTab::CTabSubclass::WindowProc(UINT msg, WPARAM wp, LPARAM lp)
{
	UNREFERENCED_PARAMETER(wp);
	if(msg != WM_NOTIFY ) return Default();
	NMHDR* pHdr = (NMHDR*)lp;
	UINT NotMsg = pHdr->code;
	HWND hWnd	= pHdr->hwndFrom;	
	if(  NotMsg == TCN_SELCHANGE  )
	{
		if		( hWnd == m_pThis->m_wndCinema.GetSafeHwnd() ) m_pThis->ChangeCinemaInControl();
		else if	( hWnd == m_pThis->m_wndRoom.GetSafeHwnd() ) m_pThis->ChangeRoomInControl();
	}
	return Default();
}

//=====================================================================================//
//                                class RoomChooserTabTab                                 //
//=====================================================================================//
RoomChooserTab::RoomChooserTab() : 
	m_pCallback(0),
	m_nCurCinemaIndex(TAB_ERR),
	m_nCurRoomIndex(TAB_ERR)
{
}


void	RoomChooserTab::Init(UINT nCinemaCtrlID, UINT nRoomCtrlID, CWnd* pWnd, IRoomChangeObserver* pCallback)
{
	m_pCallback	= pCallback;
	m_TabSubclass.HookWindow( pWnd, this );
	m_wndCinema.SubclassDlgItem( nCinemaCtrlID, pWnd );
	m_wndRoom.SubclassDlgItem( nRoomCtrlID, pWnd );
	m_wndCinema.ModifyStyle( TCS_FOCUSONBUTTONDOWN | WS_BORDER, TCS_FOCUSNEVER | TCS_TOOLTIPS );
	m_wndRoom.ModifyStyle( TCS_FOCUSONBUTTONDOWN | WS_BORDER, TCS_FOCUSNEVER | TCS_TOOLTIPS   );

	m_wndToolTip.Create( pWnd );
	pWnd->EnableToolTips();
//	m_wndToolTip.SetTitle( 1, _T("Gotcha!!!") );
	m_wndCinema.SetToolTips( &m_wndToolTip );
	m_wndRoom.SetToolTips( &m_wndToolTip );

	const std::set<CinemaRow>& Cinemas = GetDB().GetTableCinemaFacet().GetCinemas();
	std::set<CinemaRow>::const_iterator itCinR = Cinemas.begin();
	for( ; itCinR != Cinemas.end(); ++itCinR  )
	{
		int nIndex = m_wndCinema.InsertItem( m_wndCinema.GetItemCount() , itCinR->m_sName.c_str() );
		SetItemData(m_wndCinema, nIndex, itCinR->m_nID  );
	}

	m_wndCinema.SetCurSel( 0 );
	ChangeCinemaInControl();
	m_wndRoom.SetCurSel(0);
	ChangeRoomInControl();
}

int		RoomChooserTab::GetCurRoom()
{
	int nIndex = m_wndRoom.GetCurSel();
	if( nIndex != TAB_ERR)
	{
		return GetItemData( m_wndRoom, nIndex );
	}
	return INVALID_CUR;
}

int		RoomChooserTab::GetCurCinema()
{
	int nIndex = m_wndCinema.GetCurSel();
	if( nIndex != -1 )
	{
		return GetItemData(m_wndCinema, nIndex );
	}
	return INVALID_CUR;
}

void RoomChooserTab::ChangeCinemaInControl()
{
	int nIndex = m_wndCinema.GetCurSel();
	if( nIndex != TAB_ERR && m_nCurCinemaIndex != nIndex)
	{
		int nCinemaID = GetItemData( m_wndCinema, nIndex );
		m_nCurCinemaIndex = nIndex;
		ChangeCinema( nCinemaID );
	}
}

void RoomChooserTab::ChangeRoomInControl()
{
	int nIndex = m_wndRoom.GetCurSel();
	if( nIndex != TAB_ERR && m_nCurRoomIndex != nIndex )
	{
		int nRoomID = GetItemData( m_wndRoom, nIndex );
		m_nCurRoomIndex = nIndex;
		ChangeRoom( nRoomID );
	}
}

namespace
{
struct LessFirst
{
	bool operator() ( const std::pair<int, int>& _1st, const std::pair<int, int> _2nd ) const
	{
		return _1st.first < _2nd.first;
	}
};
}

void	RoomChooserTab::ChangeCinema( int nCinemaID )
{
	std::set<int> SelectedRooms = GetDB().GetTableRoomFacet().SelectRoomsFromCinema( nCinemaID );
	std::set<int>::const_iterator itRoomR = SelectedRooms.begin();
	m_wndRoom.DeleteAllItems();
	std::vector<std::pair<int, int> > RoomNumberArr;
	RoomNumberArr.reserve( 100 );
	for( ; itRoomR != SelectedRooms.end(); ++itRoomR  )
	{
		// write Number, not ID
		int nRoomID = *itRoomR;
		std::set<RoomRow>::const_iterator it = GetDB().GetTableRoomFacet().GetRooms().find( nRoomID );
		const RoomRow& Room = *it;
#if 0
		CString strName;
		strName.Format(_T("%d"), Room.m_nNumber );
		int nIndex = m_wndRoom.InsertItem( m_wndRoom.GetItemCount(), strName );
		SetItemData( m_wndRoom, nIndex, *itRoomR );
#else
		RoomNumberArr.push_back( std::make_pair( Room.m_nNumber, Room.m_nID ) );
#endif
	}
	std::sort ( RoomNumberArr.begin(), RoomNumberArr.end(), LessFirst() );
	for( size_t i = 0; i < RoomNumberArr.size(); ++i )
	{
		CString strName;
		strName.Format(_T("%d"), RoomNumberArr[i].first );
		int nIndex = m_wndRoom.InsertItem( m_wndRoom.GetItemCount(), strName );
		SetItemData( m_wndRoom, nIndex, RoomNumberArr[i].second );
	}
	m_nCurRoomIndex = TAB_ERR;
	m_wndRoom.SetCurSel( 0 );
	ChangeRoomInControl();
}

void	RoomChooserTab::ChangeRoom( int nRoomID )
{
	m_pCallback->OnRoomChange( nRoomID );
}