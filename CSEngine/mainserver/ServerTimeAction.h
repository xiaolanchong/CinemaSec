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
//   Date:   16.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __SERVER_TIME_ACTION_H_INCLUDED_3216287426710345__
#define __SERVER_TIME_ACTION_H_INCLUDED_3216287426710345__

#include "TimeSheduler.h"
#include "ThreadError.h"

class CMainServerImpl;

#define USE_OLD_TIMER
//=====================================================================================//
//                               class ServerTimeAction                                //
//=====================================================================================//
class StaAction : public TimeAction
{
protected:
	CMainServerImpl&	m_Server;
	StaAction(CMainServerImpl& s ): m_Server(s){};
	virtual ~StaAction(){}

	bool	IsUseTimeTable() const;
	virtual void	Exec()						{};
};

class StaStopped : public StaAction
{
public:
	StaStopped( CMainServerImpl& s);

	virtual void	Suspend(bool bTerminate)	;
	virtual void	Wait()						;
	virtual void	Process()					;
	virtual void	Exec();
};

class StaWorked : public StaAction
{
	SimpleTimer m_timerGetData;
public:
	StaWorked( CMainServerImpl& s);

	virtual void	Suspend(bool bTerminate)	;
	virtual void	Wait()						;
	virtual void	Process()					;
	virtual void	Exec();
};

class StaProcessed : public StaAction
{
	enum	ServerThreadState
	{
		State_Usual,
		State_GetData,
		State_Process
	};

	ServerThreadState m_stState;
#if 1
	SimpleTimer m_timerRepeatRequest;
	SimpleTimer m_timerProcessData;
#else
	AutoSimpleTimer m_timerRepeatRequest;
	SimpleTimer m_timerProcessData;
#endif
	LONG		m_nProcessingNumber;

	void	ToWaiting();
public:
	StaProcessed( CMainServerImpl& s, int nMSRepeatRequest, int nMSProcessData ) ;

	virtual void	Suspend(bool bTerminate)	;
	virtual void	Wait()						;
	virtual void	Process()					;

	void			StartProcess();
	virtual void	Exec()					;
};


#endif //__SERVER_TIME_ACTION_H_INCLUDED_3216287426710345__
