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
#ifndef __TIME_SHEDULER_H_INCLUDED_0556615680340318__
#define __TIME_SHEDULER_H_INCLUDED_0556615680340318__

#include "../DBFacet/TimeHelper.h"

//=====================================================================================//
//                                 class TimeSheduler                                  //
//=====================================================================================//

//	class T :
//	Wait		// wait->wait
//	Suspend		// suspend
//	Process		//
//	Stop		// wait,process -> suspend
//	Terminate	// 
struct TimeAction
{
	virtual void	Suspend(bool bTerminate)	= 0;
	virtual void	Wait()						= 0;
	virtual void	Process()					= 0;
	virtual void	Exec()						= 0;
};

MACRO_EXCEPTION(PropertyException, CommonException)
MACRO_EXCEPTION(UndefinedPropertyException, PropertyException)
MACRO_EXCEPTION(AssignPropertyException, PropertyException)

template< typename T> class Property
{
	bool		m_bSet;
	typename T	m_t;
public:
	Property(const Property<T>& prop) : m_bSet(prop.m_bSet), m_t(prop.m_t){ /* if(int(prop.m_bSet) > 1 ) throw 0;*/  }
	Property(const T& prop) : m_bSet(true), m_t(prop){}
	Property() : m_bSet(false){}

	bool		IsSet() const	{ return m_bSet;	};
	void		Reset() { m_bSet = false;}
	const T&	Get() const	
	{ 
		if(!m_bSet) throw UndefinedPropertyException("Property undefined"); 
		return m_t;		
	}

	T&	Get()	
	{ 
		if(!m_bSet) throw UndefinedPropertyException("Property undefined"); 
		return m_t;		
	}

	T& operator	= (const T& t)			{ m_bSet = true; return this->m_t = t;}
	Property<T>& operator	= (const Property<T>& prop)			
	{ 
		if(&prop != this) 
		{
			if( !prop.IsSet() ) throw AssignPropertyException("Assign undefined property");
			t.m_t = prop.m_t; 
			t.m_bSet = prop.m_bSet;
		}
		return *this;
	}
};

class TimeSheduler
{
public:
	struct Settings
	{
		FilmTime		m_FilmTime;
		int				m_nAfter, m_nBefore, m_nPeriod;
	} ;
private:
	Property< Settings>			m_Settings;
	Property< CTime>			m_LastProcessedTime;
//	Property< CTime>			m_LastWaitedTime;

	enum	WorkState
	{
		WorkState_Stopped,
		WorkState_Waiting,
		WorkState_Processing
	};
//	WorkState m_WorkState;
//	Settings m_Settings;

	void ProcessWhenIdChanged(TimeAction * Action, const TimeSheduler::Settings& Sets);
	void	Process( TimeAction* Action, Settings& Sets );
public:

	std::pair<CTime, CTime>		GetFilmTime() const 
	{
		if( !m_Settings.IsSet() ) return std::make_pair(CTime(), CTime());
		return std::make_pair(	m_Settings.Get().m_FilmTime.m_timeBegin,
								m_Settings.Get().m_FilmTime.m_timeEnd );
	}

	TimeSheduler();
	virtual ~TimeSheduler();

	// current or nearest film
	void ProcessNormalSettings	( TimeAction * Action, const TimeSheduler::Settings& Sets);
	void ProcessNoSetting		( TimeAction * Action);
};

#endif //__TIME_SHEDULER_H_INCLUDED_0556615680340318__
