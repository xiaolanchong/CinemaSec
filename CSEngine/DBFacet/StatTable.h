//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		OLE DB database helper class for 'Statisitcs' & 'StatPhoto' tables access
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   12.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __STAT_TABLE_H_INCLUDED_6276664202142226__
#define __STAT_TABLE_H_INCLUDED_6276664202142226__

//=====================================================================================//
//                                   class StatTable                                   //
//=====================================================================================//
#include "DBRows.h"
#include "DBHelper.h"
#include "OleDBConnection.h"

class StatTable :	protected virtual COleDBConnection,
					protected DBTableStatFacet
{
	typedef std::map< std::pair<int, int>, std::vector< BYTE > > PhotoCacheMap_t;
	PhotoCacheMap_t m_PhotoCache;
#ifdef QUERY_ALL_STATISTICS
	std::pair<bool, DBTIMESTAMP>	m_LastQueryTime;
	StatRowSet_t					m_StatSet;
	virtual const StatRowSet_t&			GetStat() const			;
	virtual void						SelectFromLastQuery()	;
#endif
protected:
	virtual PhotoMap_t					GetImages(int nStatID)	;
	virtual void						SetStatisticsRow(int nRoomID, 
					const DBTIMESTAMP& Time, int nSpectators,
					const std::map< int, std::vector<BYTE> >& BigImage );
	void	GetStatForPeriod( CTime timeBegin, CTime timeEnd, int nRoomID, StatRowSet_t& Stats );

	void	GetStatForPeriodReport(	CTime timeBegin, CTime timeEnd, 
									const std::vector<int>& Rooms, StatRowSet_t& Stats )	;
	virtual void	GetStatInfo( int nStatID, int& nRoomID, CTime& timeStat, int& nSpectators );

	virtual void	GetStatForExchange( int nRoomID, CTime timeBegin, CTime timeEnd, std::vector< FilmStat_t >& StatArr,int &nStatID );
};

#endif //__STAT_TABLE_H_INCLUDED_6276664202142226__