#include "stdafx.h"
#include "CinemaDB.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

////// DB

std::set<int>	CCinemaDB::SelectCamerasFromRoom(int nRoomID)		const
{
	std::set<int>		RetSet;
	std::set<CameraRow>::const_iterator it = m_Cameras.begin();
	for(; it != m_Cameras.end();++it)
	{
		if(it->m_nRoomID == nRoomID) RetSet.insert( it->m_nID );
	}
	return RetSet;
}

std::set<int>	CCinemaDB::SelectRoomsFromCinema(int nCinemaID)		const
{
	std::set<int>		RetSet;
	std::set<RoomRow>::const_iterator it = m_Rooms.begin();
	for(; it != m_Rooms.end();++it)
	{
		if(it->m_nCinemaID == nCinemaID) RetSet.insert( it->m_nID );
	}
	return RetSet;
}

std::tstring			CCinemaDB::GetCameraSource( int nCameraID ) const 
{
	std::set<CameraRow>::const_iterator it = m_Cameras.find( CameraRow( nCameraID) );
	ASSERT ( it != m_Cameras.end() );
	return it->m_sGUID;
}

void CCinemaDB::GetCinemaSet( CinemaSet_t& Cinemas ) const 
{
	Cinemas.clear();
	std::set<CameraRow>::const_iterator itCam = m_Cameras.begin();
	for( ; itCam != m_Cameras.end(); ++itCam )
	{
		int nCameraID	= itCam->m_nID;
		int nRoomID		= itCam->m_nRoomID;
		std::set<RoomRow>::const_iterator itRoom = m_Rooms.find( RoomRow(nRoomID  ));
		ASSERT( itRoom != m_Rooms.end() );
		int nCinemaID	= itRoom->m_nCinemaID;
		Cinemas[ nCinemaID ][ nRoomID ].insert( nCameraID );
	}
}

NameMap_t		CCinemaDB::GetCinemaNames() const
{
	NameMap_t		RetMap;
	std::set<CinemaRow>::const_iterator it = m_Cinemas.begin();
	for(; it != m_Cinemas.end();++it)
	{
		RetMap.insert( std::make_pair( it->m_nID, std::tstring( it->m_sName ) ) );
	}
	return RetMap;
}

std::map<int,int>		CCinemaDB::GetRoomNumbers() const
{
	std::map<int,int>		RetMap;
	std::set<RoomRow>::const_iterator it = m_Rooms.begin();
	for(; it != m_Rooms.end();++it)
	{
		RetMap.insert( std::make_pair( it->m_nID,  it->m_nNumber ) );
	}
	return RetMap;
}

bool		CCinemaDB::GetRoomInfo(int nRoomID, CString& sCinemaName, int& nRoomNo )
{
	std::set<RoomRow>::const_iterator it = m_Rooms.find( RoomRow( nRoomID) );
	if ( it == m_Rooms.end() )
	{
		return false;
	}
	int nCinemaID = it->m_nCinemaID;
	nRoomNo = it->m_nNumber;

	std::set<CinemaRow>::const_iterator itCin = m_Cinemas.find( CinemaRow( nCinemaID) );
	if ( itCin == m_Cinemas.end() )
	{
		return false;
	}
	sCinemaName = itCin->m_sName.c_str();
	return true;
}

bool	CCinemaDB::UpdateLayout( const std::map< int, std::pair<int, int > >&  LayoutMap ) 
{
	std::map< int, std::pair<int, int > >::const_iterator it = LayoutMap.begin();
	for( ; it != LayoutMap.end(); ++it )
	{
		std::set<CameraRow>::iterator itCam = m_Cameras.find( CameraRow( it->first ) );
		if( itCam != m_Cameras.end() )
		{
			itCam->m_Pos = it->second;
		}
	}
	return true;
}






