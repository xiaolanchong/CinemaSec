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
#ifndef __C_STATISTICS_SERVER_IMPL_H_INCLUDED_6127058658341551__
#define __C_STATISTICS_SERVER_IMPL_H_INCLUDED_6127058658341551__

#include "IStatisticsServer.h"
#include "../DBFacet/CinemaOleDB.h"
//=====================================================================================//
//                             class CStatisticsServerImpl                             //
//=====================================================================================//
class CStatisticsServerImpl :	public IStatisticsServer
{
	virtual HRESULT Start(/*LPCWSTR szConnectionString*/)		;
	virtual HRESULT Statistics( int nRoomID, const DBTIMESTAMP& Time, 
		const std::map<int, std::vector<BYTE> >& Images, int nSpectators );
	virtual void Release()	;
	virtual void	SetDebugInterface( IDebugOutput* pDebug )
	{
		m_Debug.Set( pDebug );
	}

	virtual HRESULT	GetCurrentOrNextFilmTime( int nRoomID, FilmTime& ft );
	virtual HRESULT	GetWorkParam( int nRoomID, int & After, int &Before, int& Period);
	virtual HRESULT	GetBackupParam( int nRoomID, double& fStart, double& fStop, int& nDaysKeep    ) ;
	virtual	HRESULT GetAlgoParam( int nRoomID, std::vector<BYTE>& Data );

	CCinemaOleDB		m_db;
	MyDebugOutputImpl	m_Debug;
public:
	CStatisticsServerImpl();
	~CStatisticsServerImpl();
};

#endif //__C_STATISTICS_SERVER_IMPL_H_INCLUDED_6127058658341551__