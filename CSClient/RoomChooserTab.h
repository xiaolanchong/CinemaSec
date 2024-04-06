//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:   helper for tab cinema & room selection                             //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   20.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __ROOM_CHOOSER_TAB_H_INCLUDED_7760734276131614__
#define __ROOM_CHOOSER_TAB_H_INCLUDED_7760734276131614__

#include "gui/Subclass.h"

//=====================================================================================//
//                                class RoomChooserTab                                 //
//=====================================================================================//

struct IRoomChangeObserver
{
	virtual void OnRoomChange(int nRoomID) = 0;
};

class RoomChooserTab
{
	friend class CTabSubclass;
	class CTabSubclass : CSubclassWnd
	{
		RoomChooserTab* m_pThis;
		virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);
	public: 
		void HookWindow(CWnd* pWnd, RoomChooserTab* pThis);
	} m_TabSubclass;

	CTabCtrl		m_wndCinema;
	CTabCtrl		m_wndRoom;
	CToolTipCtrl	m_wndToolTip;

	int				m_nCurCinemaIndex;
	int				m_nCurRoomIndex;

	void			ChangeCinema( int nCinemaID );
	void			ChangeRoom( int nCinemaID );
	void			ChangeCinemaInControl(  );
	void			ChangeRoomInControl( );

	IRoomChangeObserver* m_pCallback;
public:	
	RoomChooserTab();
	// if u got it call 911!
	enum
	{
		INVALID_CUR = -1
	};
	void	Init( UINT nCinemaCtrlID, UINT nRoomCtrlID, CWnd* p, IRoomChangeObserver* pObs);
	int		GetCurCinema();
	int		GetCurRoom();
};

#endif //__ROOM_CHOOSER_TAB_H_INCLUDED_7760734276131614__
