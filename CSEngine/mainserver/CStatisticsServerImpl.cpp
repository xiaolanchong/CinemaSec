//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		IStatisticsServer implementation fo DB storages
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   04.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#include "stdafx.h"
#include "CStatisticsServerImpl.h"
#include "../../CSChair/interfaceEx.h"
#include "NetPort.h"

// last header
#include "../../CSChair/public/memleak.h"
//=====================================================================================//
//                             class CStatisticsServerImpl                             //
//=====================================================================================//

#define USE_TRUE_INTERFACE
#ifdef  USE_TRUE_INTERFACE
EXPOSE_INTERFACE( CStatisticsServerImpl, IStatisticsServer, STATISTICS_SERVER_INTERFACE_0 );
#else
CStatisticsServerImpl StatServer;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR ( CStatisticsServerImpl, IStatisticsServer, STATISTICS_SERVER_INTERFACE_0, StatServer );
#endif

void CStatisticsServerImpl::Release()
{
#ifdef USE_TRUE_INTERFACE
	delete this;
#endif
}


CStatisticsServerImpl::CStatisticsServerImpl()
{

}

CStatisticsServerImpl::~CStatisticsServerImpl()
{

}

HRESULT CStatisticsServerImpl::Start(/*LPCWSTR szConnectionString*/)	
{
	try
	{
		WCHAR szBuffer[1024];
		LPCWSTR szPath = RegSettings::c_szCinemaSecSubkey;
	//	if( !szConnectionString || !szConnectionString[0])
		{
			DWORD dwType = REG_SZ;
			DWORD dwSize = 1024 * sizeof(WCHAR);
			LONG res =  SHRegGetUSValueW( szPath, L"DBConnectionString", &dwType, (BYTE*)szBuffer, &dwSize, TRUE, 0, 0 );
			if( res != ERROR_SUCCESS )
			{
				throw DBConnectionException("Connection string is not available");
			}
	//		szConnectionString = szBuffer;
		}

		m_db.SetSource( szBuffer, false );
		m_db.Open();
		m_db.ReadData();
	}
	catch( DataBaseException & )
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CStatisticsServerImpl::Statistics( int nRoomID, const DBTIMESTAMP& Time, 
						   const std::map<int, std::vector<BYTE > >& Images, int nSpectators )
{
	if( !m_db.CheckConnection() ) 
	{
		Start(/*NULL*/);
	}

	std::set<int> CamerasInRoom = m_db.GetTableCameraFacet().SelectCamerasFromRoom( nRoomID );
	if( CamerasInRoom.empty() )
	{
		return E_INVALIDARG;
	}
	std::vector<BYTE> BigImage;
	std::map<int, std::vector<BYTE > >::const_iterator it = Images.begin();
	try
	{
	m_db.GetTableStatFacet().SetStatisticsRow( nRoomID, Time, nSpectators, Images );
	}
	catch(DataBaseException& /*ex*/)
	{
		m_Debug.PrintW( IDebugOutput::mt_error, L"StatisticsServer failed to update statistics", nSpectators, Images.size() );
		return E_FAIL;
	}
	m_Debug.PrintW( IDebugOutput::mt_debug_info, L"StatisticsServer %d spc, %Id images", nSpectators, Images.size() );
	return S_OK;
}

HRESULT	CStatisticsServerImpl::GetCurrentOrNextFilmTime( int nRoomID, FilmTime& ft ) 
{
	if( !m_db.CheckConnection() ) 
	{
		Start(/*NULL*/);
	}
	try
	{
		bool res = m_db.GetTableTimetableFacet().GetCurrentOrNextFilmTime( nRoomID, ft );
		return res? S_OK : S_FALSE;
	}
	catch(DataBaseException)
	{
		return E_FAIL;
	}
}

HRESULT	CStatisticsServerImpl::GetWorkParam( int nRoomID, int & After, int &Before, int& Period)
{
	if( !m_db.CheckConnection() ) 
	{
		Start(/*NULL*/);
	}
	try
	{
		m_db.GetTableRoomFacet().GetWorkParam(nRoomID, After, Before, Period);
		return S_OK;
	}
	catch(DataBaseException)
	{
		return E_FAIL;
	}
}

HRESULT	CStatisticsServerImpl::GetBackupParam( int nRoomID, double& fStart, double& fStop, int& nDaysKeep  ) 
try
{
	if( !m_db.CheckConnection() ) 
	{
		Start(/*NULL*/);
	}
	CTime timeLast;
	m_db.GetTableRoomFacet().GetArchiveParam(nRoomID, fStart, fStop, nDaysKeep);
	return S_OK ;
}
catch(DataBaseException)
{
	return E_FAIL;
};

HRESULT	CStatisticsServerImpl::GetAlgoParam( int nRoomID, std::vector<BYTE>& Data )
try
{
	if( !m_db.CheckConnection() ) 
	{
		Start(/*NULL*/);
	}
	bool res = m_db.GetTableRoomFacet().GetAlgoParam(nRoomID, Data );
	return res ? S_OK : E_FAIL ;
}
catch(DataBaseException)
{
	return E_FAIL;
};
