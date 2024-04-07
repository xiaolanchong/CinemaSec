//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Action interface during thread work fo CSGuard
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 08.07.2005
//                                                                                      //
//======================================================================================//
#ifndef _I_ACTION_8221655305922738_
#define _I_ACTION_8221655305922738_

struct  ProcessEnvironment
{
	TCHAR				m_szPath[MAX_PATH];
	PROCESS_INFORMATION	m_pi;
	DWORD				m_dwTimeout;
	LPARAM				m_WindowID;
	UINT				m_WindowMsg;

	DWORD				n_WaitOrKillProcess		;	// ms
	DWORD				n_CheckProcess			;	// ms
	DWORD				n_TimeAfterNoWindow		;	// ms
	DWORD				n_SendMessageTimeout	;	//ms
};

//======================================================================================//
//                                    struct IAction                                    //
//======================================================================================//
/*
struct IAction
{
	enum Result
	{
		ResOk,
		ResExit,
		ResRestart
	};

	virtual Result	Process( ProcessEnvironment& pe );
	virtual ~IAction();
};*/

struct	IState
{
	static	IState*	GetStartState();
	virtual ~IState() = default;
	virtual IState*	Process( ProcessEnvironment& pe ) = 0;
};

// special state
struct  ExitState : IState
{
	virtual IState*	Process(ProcessEnvironment& pe)	{ return 0; 	}
};

#endif // _I_ACTION_8221655305922738_