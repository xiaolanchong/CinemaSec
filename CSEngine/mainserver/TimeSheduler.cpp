//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   15.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#include "stdafx.h"
#include "TimeSheduler.h"

// last header
#include "../../CSChair/public/memleak.h"

//=====================================================================================//
//                                 class TimeSheduler                                  //
//=====================================================================================//
TimeSheduler::TimeSheduler()  
//	m_WorkState(WorkState_Stopped)
{
}

TimeSheduler::~TimeSheduler()
{
}

void TimeSheduler::ProcessNoSetting		( TimeAction * Action)
{
	if( m_Settings.IsSet() )
	Process( Action, m_Settings.Get() );
}

void TimeSheduler::ProcessNormalSettings( TimeAction * Action, const TimeSheduler::Settings& Sets)
{
	m_Settings = Sets;

	ASSERT( Sets.m_FilmTime.m_timeBegin < Sets.m_FilmTime.m_timeEnd);
	CTime timeCurrent = CTime::GetCurrentTime();
	ASSERT( timeCurrent < Sets.m_FilmTime.m_timeEnd  );
/*	if( timeCurrent <  Sets.m_FilmTime.m_timeBegin )
	{
		m_LastProcessedTime.Reset();
//		m_WorkState = WorkState_Stopped;
		Action->Suspend(false);
		return;// TimeSheduler::WorkState_Suspend;
	}*/

	Process( Action, m_Settings.Get() );
}

void	TimeSheduler::Process( TimeAction* Action, Settings& Sets )
{
	CTime timeCurrent = CTime::GetCurrentTime();
#define USE_STUB
#ifdef USE_STUB
	CTimeSpan spanAfter	( 0, 0, 0, Sets.m_nAfter );
	CTimeSpan spanBefore( 0, 0, 0, Sets.m_nBefore );
	CTimeSpan spanPeriod( 0, 0, 0, Sets.m_nPeriod );
#else
	CTimeSpan spanAfter	( 0, 0, Sets.m_nAfter, 0 );
	CTimeSpan spanBefore( 0, 0, Sets.m_nBefore, 0 );
	CTimeSpan spanPeriod( 0, 0, Sets.m_nPeriod, 0 );
#endif
	if( timeCurrent < Sets.m_FilmTime.m_timeBegin + spanAfter )
	{
		//		m_WorkState = WorkState_Stopped;
		Action->Suspend(false);
		return /*TimeSheduler::WorkState_Suspend*/;
	}
	if( timeCurrent + spanBefore > Sets.m_FilmTime.m_timeEnd )
	{
		m_Settings.Reset();
		m_LastProcessedTime.Reset();
		//		m_WorkState = WorkState_Stopped;
		Action->Suspend(false);
		return /*TimeSheduler::WorkState_Suspend*/;
	}


	if( !m_LastProcessedTime.IsSet() ) 
	{
		m_LastProcessedTime = timeCurrent;
		Action->Wait();
	}

	if( m_LastProcessedTime.Get() + spanPeriod <= timeCurrent)
	{
		m_LastProcessedTime = timeCurrent;
		//			m_WorkState = WorkState_Processing;
		Action->Process();
		return /*WorkState_Process*/;
	}
}