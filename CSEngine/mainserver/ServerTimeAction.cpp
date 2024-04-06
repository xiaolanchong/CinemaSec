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
#include "stdafx.h"

using std::max;
using std::min;

#include "ServerTimeAction.h"
#include "main_server_impl.h"

// last header
#include "../../CSChair/public/memleak.h"

bool	StaAction::IsUseTimeTable() const 
{ 
	return m_Server.m_bUseTimeTable;
}

//=====================================================================================//
//                               class Stopeed			                                //
//=====================================================================================//
StaStopped ::StaStopped(CMainServerImpl& s ): 
	StaAction(s)
{
	m_Server.m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"STOP state" );
}

void	StaStopped ::Suspend(bool bTerminate)	
{

}

void	StaStopped ::Wait()		
{
	DWORD dwRes = m_Server.StartWork(true);
	if( dwRes == S_OK)
	{
		m_Server.m_pTimeAction = std::auto_ptr<TimeAction>( new StaWorked(m_Server)  );
	}
}

void	StaStopped ::Process()
{
	// nothing
}

void	StaStopped ::Exec()
{
	if( !IsUseTimeTable() && m_Server.m_State == CMainServerImpl::ss_running )
		m_Server.m_pTimeAction = std::auto_ptr<TimeAction>( new StaWorked(m_Server)  );
}

//=====================================================================================//
//                               class Worked			                               //
//=====================================================================================//
StaWorked::StaWorked( CMainServerImpl& s) : 
	StaAction(s),
	m_timerGetData(10 * 1000)
{
	if( !IsUseTimeTable() )
		m_timerGetData.Start();
	m_Server.m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"WORK state" );
}

void	StaWorked ::Suspend(bool bTerminate)	
{
	DWORD dwRes = m_Server.StopWork(true);
	dwRes;
	m_Server.m_pTimeAction = std::auto_ptr<TimeAction>( new StaStopped(m_Server) );
}

void	StaWorked ::Wait()		
{
	
}

void	StaWorked ::Process()
{
	m_Server.m_pTimeAction = std::auto_ptr<TimeAction>( new StaProcessed( m_Server,
			m_Server.m_HallProcessConfig.m_nRequestDataPeriod , 
			m_Server.m_HallProcessConfig.m_nProcessingPeriod) );
}

void	StaWorked::Exec()
{
	if( !IsUseTimeTable() && m_timerGetData.IsSignaled()  )
	{
		Process();
	}
}
//=====================================================================================//
//                               class Processed		                               //
//=====================================================================================//

void	StaProcessed::StartProcess()
{
}

void	StaProcessed ::Suspend(bool bTerminate)	
{
	// null all
}

void	StaProcessed ::Exec()		
{
	// work
	switch(m_stState)
	{
		case State_Usual :
#if 0
			if( m_Server.m_HallAnalyzer.get())
			{
				m_stState				= State_GetData;
				m_timerRepeatRequest.Start();
				m_nProcessingNumber = 0;
				m_Server.m_HallAnalyzer->StartGatherMode();
				// start
				m_Server.StartProcessing();
				std::set<int> ProcCams;
				m_Server.m_ServerCameraData.WhatCamerasRequired(ProcCams);
				m_Server.GetDataFromCameras(ProcCams);
			}
			else
				ToWaiting();
#endif
			break;
		case State_GetData:
			if( m_timerRepeatRequest.IsSignaled() )
			{
				if( m_nProcessingNumber >= m_Server.m_HallProcessConfig.m_nRequestDataNumber  )
				{
					m_Server.m_pDebug.PrintW(IDebugOutput::mt_error, L"Timeout : some cameras didn't send data");
#if 0
					m_Server.m_HallAnalyzer->EndGatherMode();
#endif
					// caution it delete this!!!
					ToWaiting();
					return;
				}
				else
				{
					std::set<int> ProcCams;
					m_Server.m_ServerCameraData.WhatCamerasRequired(ProcCams);
					if( !ProcCams.empty() )
					{
						m_Server.GetDataFromCameras(ProcCams);
					}
					else
					{
						m_Server.LoadDataToHallAnalyzer();
#if 0
						m_Server.m_HallAnalyzer->EndGatherMode();
#endif
						m_timerProcessData.Start();
						m_stState = State_Process;
					}
					m_timerRepeatRequest.Start();
					++m_nProcessingNumber;
				}
			}
			break;
		case State_Process:
			if( m_Server.m_Spectators.IsReady() && m_Server.m_ServerCameraData.IsAllImages() )
			{
				m_Server.UpdateStat();
				m_Server.m_Spectators.Reset();
				// exit
				ToWaiting();
			}
			else if(m_timerProcessData.IsSignaled())
			{
				m_Server.m_pDebug.PrintW(IDebugOutput::mt_error, L"Timeout : some cameras didn't send images or spectator number is not set");
				//timerGetCameraData.Start();
				// exit
				ToWaiting();		
			}
	}
}

void	StaProcessed ::Process()
{
	// null all
}

StaProcessed ::StaProcessed( CMainServerImpl& s, int nMSRepeatRequest, int nMSProcessData ) : 
	StaAction(s),
	m_stState( State_GetData ),
	m_timerRepeatRequest(nMSRepeatRequest),
	m_timerProcessData(nMSProcessData),
	m_nProcessingNumber(0)

{
	if( m_Server.m_HallAnalyzer.get())
	{
		m_Server.m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"PROCESS state" );
		//m_stState				= State_GetData;
		m_timerRepeatRequest.Start();
		m_nProcessingNumber = 0;
#if 0
		m_Server.m_HallAnalyzer->StartGatherMode();
#endif
		// start
		m_Server.StartProcessing();
		std::set<int> ProcCams;
		m_Server.m_ServerCameraData.WhatCamerasRequired(ProcCams);
		m_Server.GetDataFromCameras(ProcCams);
	}
	else
		ToWaiting();
}

void	StaProcessed ::Wait()						
{
	// noth
}

void  StaProcessed::ToWaiting()
{
	m_Server.m_pTimeAction = std::auto_ptr<TimeAction>( new StaWorked(m_Server)  );
}
