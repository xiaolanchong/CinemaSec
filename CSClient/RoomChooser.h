#pragma once

#include "gui/Subclass.h"
#include "common/tstring.h"

struct IRoomChooserCB
{
	virtual std::set< int >					OnCinemaChange(int nCinemaID)	= 0;
	virtual void							OnRoomChange(int nRoomID)		= 0;
};

class RoomChooser 
{
	friend class CComboSubclass;
	class CComboSubclass : CSubclassWnd
	{
		RoomChooser* m_pThis;
		virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);
	public: 
		void HookWindow(CWnd* pWnd, RoomChooser* pThis);
	} m_ComboSubclass;

	IRoomChooserCB*	m_pCallback;
	

	void			ChangeCinemaInControl();
	void			ChangeRoomInControl();

	void			ChangeCinema( int nCinemaID );
	void			ChangeRoom( int nCinemaID );

	int				m_nCurCinemaIndex;
	int				m_nCurRoomIndex;

	CComboBox	m_wndCinema;
	CComboBox	m_wndRoom;
protected:


	void	Init(CWnd* pWnd, IRoomChooserCB * pCallback, 
					const std::map< int, std::tstring >& Cinemas);
	int		GetCurRoom();
	int		GetCurCinema();

	void	Subclass(UINT nCinemaComboID, UINT nRoomComboID, CWnd* pWnd )
	{
		m_wndCinema.SubclassDlgItem( nCinemaComboID, pWnd );
		m_wndRoom.SubclassDlgItem( nRoomComboID, pWnd );
	}
public:
	RoomChooser(void);
	~RoomChooser(void);
};
