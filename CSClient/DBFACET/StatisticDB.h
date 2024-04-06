#ifndef __STATISTICDB_H__
#define __STATISTICDB_H__

#include "common/Exception.h"

struct IStatisticObserver
{
	virtual void OnStatisticsUpdate(/*int nRoomID*/) = 0;
};

MACRO_EXCEPTION(StatisticsException, CommonException);
MACRO_EXCEPTION(NoStatisticsException, StatisticsException);

class StatisticDB
{
	std::set< IStatisticObserver* >	m_Observers;
	typedef	std::pair< DATE, long >			StatRecord_t;
	std::vector< StatRecord_t >				m_Statistics;

	void FireEvent(int nRoomID);
public:
	StatisticDB(void);
	virtual ~StatisticDB();

	void Register	( IStatisticObserver* pObserver );
	void Unregister	( IStatisticObserver* pObserver );

	void SetStatistics( int nRoomID, long PeopleNumber );
	long GetStatistics( int nRoomID ) const;
};

StatisticDB& GetStatisticsDB();

#endif //__STATISTICDB_H__