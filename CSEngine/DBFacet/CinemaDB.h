// CinemaDB.h: interface for the CCinemaDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CINEMADB_H__EC8C9E55_7C09_464D_9F9E_8CB1AB696A4B__INCLUDED_)
#define AFX_CINEMADB_H__EC8C9E55_7C09_464D_9F9E_8CB1AB696A4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>

#include "DBRows.h"

//! \brief класс конфигурации БД
//! \version 1.0
//! \date 10-28-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//!
class CCinemaDB :	protected DBTableCameraFacet,
					protected DBTableCinemaFacet,
					protected DBTableRoomFacet
{
protected:	
	//! ктнотеатры
	std::set<CinemaRow	>		m_Cinemas;
	//! залы
	std::set<RoomRow	>		m_Rooms;
	//! камеры
	std::set<CameraRow	>		m_Cameras;

	// DBCameraTableFacet
	virtual const std::set<CameraRow>&	GetCameras (  ) const	{ return m_Cameras;	};
	virtual	std::set<CameraRow>&		GetCameras (  )			{ return m_Cameras;	};
	virtual std::tstring				GetCameraSource( int nCameraID ) const ;

	virtual std::set<int>				SelectCamerasFromRoom(int nRoomID)		const;
	virtual bool						UpdateLayout( const std::map< int, std::pair<int, int > >&  LayoutMap ) ;

	// DBRoomTableFacet
	virtual const std::set<RoomRow>&	GetRooms (  ) const		{ return m_Rooms;	};
	virtual std::set<RoomRow>&			GetRooms (  )			{ return m_Rooms;	};
	virtual std::set<int>				SelectRoomsFromCinema(int nCinemaID)	const;
	virtual std::map<int,int>			GetRoomNumbers() const;
	virtual bool						GetRoomInfo(int nRoomID, CString& sCinemaName, int& nRoomNo );

	// cinemas
	virtual const std::set<CinemaRow>&	GetCinemas (  ) const   { return m_Cinemas; };
	virtual void						GetCinemaSet( CinemaSet_t& Cinemas ) const ;
	virtual NameMap_t					GetCinemaNames() const;

	virtual void						GetWorkParam(  int , int & , int &, int& )		
	{
	};
	virtual void						SetWorkParam(  int , int  , int , int )		
	{
	};

public:
	virtual void			Open()			= 0;
	virtual void			ReadData()		= 0;
	virtual bool			IsConnected() const	= 0;
	
	DBTableCameraFacet& GetTableCameraFacet() { return static_cast<DBTableCameraFacet&>(*this); }
	DBTableCinemaFacet& GetTableCinemaFacet(){ return static_cast<DBTableCinemaFacet&>(*this); }
	DBTableRoomFacet&	GetTableRoomFacet()	{ return static_cast<DBTableRoomFacet&>(*this); }

	virtual ~CCinemaDB(){};

};

#endif // !defined(AFX_CINEMADB_H__EC8C9E55_7C09_464D_9F9E_8CB1AB696A4B__INCLUDED_)
