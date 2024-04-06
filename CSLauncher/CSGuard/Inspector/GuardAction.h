//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Implements guard action
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 08.07.2005
//                                                                                      //
//======================================================================================//
#ifndef _GUARD_ACTION_6052017840995332_
#define _GUARD_ACTION_6052017840995332_

#include "IAction.h"
#include <boost/optional.hpp>

///// State ///////////

class StartState : public IState
{
public: 
	virtual IState * Process( ProcessEnvironment& pe );
};

class EndProcessState : public IState
{
	bool	m_bExitAfterEnd;
	void	WaitOrKillProcess( PROCESS_INFORMATION& pi, DWORD dwTimeout );
public: 
	EndProcessState( bool bExitAfterEnd) : m_bExitAfterEnd(bExitAfterEnd){}
	virtual IState * Process( ProcessEnvironment& pe );
};

class CreatedState : public IState
{
	enum QueueResult
	{
		res_ok, res_restart, res_exit
	};

	bool		CheckProcess( ProcessEnvironment& pe );
	QueueResult	CheckQueue( ProcessEnvironment& pe );
	bool		PingProcess( ProcessEnvironment& pe );

	boost::optional<INT64>	m_nTimeAfterNoWindow;
public: 
	virtual IState * Process( ProcessEnvironment& pe );
};


//======================================================================================//
//                                  class GuardAction                                   //
//======================================================================================//


#endif // _GUARD_ACTION_6052017840995332_