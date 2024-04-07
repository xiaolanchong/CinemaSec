#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include ".\statisticdb.h"

StatisticDB::StatisticDB(void)
{
}

StatisticDB::~StatisticDB(void)
{
}

void StatisticDB::Register	( IStatisticObserver* pObserver )
{
	m_Observers.insert( pObserver );
}

void StatisticDB::Unregister	( IStatisticObserver* pObserver )
{
	m_Observers.erase( pObserver );
}

void StatisticDB::SetStatistics( int nRoomID, long PeopleNumber )
{
	SYSTEMTIME st;
	GetLocalTime( &st );
	DATE vtime;
	SystemTimeToVariantTime( &st, &vtime );
	m_Statistics.push_back( std::make_pair( vtime, PeopleNumber ) );
	FireEvent(nRoomID);
}

long StatisticDB::GetStatistics( int nRoomID) const
{
	if( m_Statistics.empty() ) throw NoStatisticsException( "Don't have statistics for now" );
	else return m_Statistics.back().second;
}

void StatisticDB::FireEvent(int nRoomID)
{
	std::for_each( m_Observers.begin(), m_Observers.end(), std::mem_fun( &IStatisticObserver::OnStatisticsUpdate  ) );
}

StatisticDB& GetStatisticsDB()
{
	static StatisticDB db;
	return db;
}