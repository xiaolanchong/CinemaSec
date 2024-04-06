#ifndef __DBROOMCHOOSER_H__
#define __DBROOMCHOOSER_H__
/********************************************************************
	created:	2005/02/11
	created:	11:2:2005   20:49
	author:		Eugene Gorbachev
	
	purpose:	
*********************************************************************/

#include "../RoomChooser.h"

template <class T>
class DBRoomChooser :  protected RoomChooser, IRoomChooserCB
{
	T&		m_t;
public:
	DBRoomChooser(T& t) : m_t(t){}
	void Init(	UINT nCinemaComboID, UINT nRoomComboID, CWnd* pWnd )
	{
		const std::set<CinemaRow>& Cinemas = GetDB().GetTableCinemaFacet().GetCinemas();
		std::set<CinemaRow>::const_iterator itCinR = Cinemas.begin();
		std::map<int, std::tstring > Names;
		for( ; itCinR != Cinemas.end(); ++itCinR  )
		{
			Names.insert( std::make_pair( itCinR->m_nID , itCinR->m_sName.c_str() ) );
		}
		RoomChooser::Subclass( nCinemaComboID, nRoomComboID, pWnd );
		RoomChooser::Init(pWnd, this, Names);
	}

	std::set< int >		OnCinemaChange(int nCinemaID)
	{
#ifdef TEST_CHOOSER
		CString str;
		str.Format( _T("Cinema %d"), nCinemaID);
		AfxMessageBox( str );
#endif
		std::set<int> SelectedRooms = GetDB().GetTableRoomFacet().SelectRoomsFromCinema( nCinemaID );
		return SelectedRooms;
	}

	void				OnRoomChange(int nRoomID)	
	{
#ifdef TEST_CHOOSER
		CString str;
		str.Format( _T("Room %d"), nRoomID);
		AfxMessageBox( str );
#endif
		m_t.OnRoomChange(nRoomID);
	}
};
#endif //__DBROOMCHOOSER_H__