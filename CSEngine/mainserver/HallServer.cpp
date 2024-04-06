//=====================================================================================//
//                                       CSChair                                       //
//                           Copyright by ElVEES, 2005                                 //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		Hall thread server class
                                                                                       */
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   26.01.2005                                                                //
//=====================================================================================//
#include "stdafx.h"
#include "HallServer.h"

#ifdef USE_STUB
#include "hall_analyzer_stub.h"
#endif
#include "../../CSChair/dmpstuff/ExceptionHandler.h"
#include "../../CSAlgo/Interfaces/hall_analyzer_interface.h"
#include "../../CSAlgo/Interfaces/parameter_dialog.h"
#include "..\Timeouts.h"
#include <shlwapi.h>

//last header
#include "../../CSChair/public/memleak.h"

static void	DeleteHallAnalyzer( IHallAnalyzer * p )
{
	p->Release();
}

//=====================================================================================//
//                                  class HallServer                                   //
//=====================================================================================//
HallServer::HallServer()
{
}

HallServer::~HallServer()
{
}

IHallAnalyzer* HallServer::GetHallAnalyzer()
{
	 return m_HallAnalyzer.get() ? m_HallAnalyzer->m_pAnalyzer : NULL;
}

void	HallServer::InitHallAnalyzer(const std::vector<BaseChair>& Chairs, 
									 const std::vector<BYTE>& AlgoParam,
									 const std::pair<CTime, CTime> & TimeFilm)
{
	HRESULT hr;
	Int8Arr ParamData;
	hr = LoadAlgoParam( AlgoParam, ParamData );
	if( FAILED(hr) )
	{
		m_HallAnalyzer.reset();
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Failed to get default parameters" );
		throw HallServerException("Error");
	}

	IHallAnalyzer* pInt;
	hr = CreateHallAnalyzer( &pInt, &ParamData, &Chairs, Plat_AbsoluteTime(), m_pDebug.Get() );
	if( !pInt || hr != S_OK )
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"CreateHallAnalyzer failed, reason=%d", hr );
		throw HallServerException("Error");
	}
	bool res = SetFilmTime(pInt, TimeFilm);
	if(!res) throw HallServerException("Failed to set the film time period");
	HallAnalyzer_t ha = HallAnalyzer_t( new HallThread(pInt) );
	m_HallAnalyzer = ha;
	ResetEvent(m_HallAnalyzer->m_hEventForProcessing);
}

DWORD	HallServer::StartHallThread(const std::vector<BaseChair>& Chairs, 
									const std::vector<BYTE>& AlgoParam,
									const std::pair<CTime, CTime> & TimeFilm)
{
	unsigned int dwId;

	m_HallAnalyzer.reset();
	try
	{
	InitHallAnalyzer(Chairs, AlgoParam, TimeFilm);
	}
	catch(HallServerException&)
	{
		return ERR_ERROR;
	}

	if( m_HallAnalyzer.get() )
	{
		HANDLE hThread = (HANDLE)_beginthreadex( 0, 0, Handled_HallAnalyzerThreadProc, this, CREATE_SUSPENDED, &dwId  );
		if( reinterpret_cast<uintptr_t>( hThread ) != -1  )
		{
			m_HallAnalyzer->m_Thread.Start( hThread );
			m_pDebug.PrintW( IDebugOutput::mt_info, L"Hall thread started");
		}
		else
		{
			m_pDebug.PrintW( IDebugOutput::mt_error, L"Failed to create a hall thread");
			return ERR_ERROR;
		}
	}
	return ERR_OK;
}

DWORD	HallServer::StopHallThread()
{
	if( m_HallAnalyzer.get() )
	{
		bool res = m_HallAnalyzer->m_Thread.Stop( ThreadTimeout_HallAnalyzer );
		m_pDebug.PrintW( IDebugOutput::mt_info, res ? L"Hall thread stopped safely" : L"Hall thread was terminated");
		m_HallAnalyzer.reset();
		return res ? ERR_OK : ERR_ERROR;
	}
	return ERR_OK;
}

/////////// THREAD /////////////////

unsigned int WINAPI HallServer::Handled_HallAnalyzerThreadProc(void * pParam)
{
	unsigned int Res = err_exception;
	__try
	{
		Res = HallThread::ThreadProc(pParam);
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("HallAnalyzer thread"))
#ifndef _DEBUG
		, EXCEPTION_EXECUTE_HANDLER
#endif
		)
	{
	}
	return Res;
}

unsigned int WINAPI HallServer::HallThread::ThreadProc(void * pParam)
{
	const DWORD c_dwTimeOut = 10; // ms
	HallServer* pThis = (HallServer*)pParam;
	int nDrawCounter = 0;

	while( pThis->m_HallAnalyzer->m_Thread.CanThreadWork(c_dwTimeOut ))
	{
		DWORD res = WaitForSingleObject( pThis->m_HallAnalyzer->m_hEventForProcessing, c_dwTimeOut );
		if( res == WAIT_OBJECT_0 )
		{	
			// process data!!!
			pThis->Process();
		}
		else
		{
			// nothing
			if( nDrawCounter > 20 )
			{
				nDrawCounter = 0;
				pThis->DrawState();
			}
			else ++nDrawCounter;
		}
	}

	RETURN(err_noerror) ;
}

DWORD HallServer::Process()
{
	//CameraData_t LocalDataCache;
	if( !m_HallAnalyzer.get() || !m_HallAnalyzer->m_pAnalyzer ) return err_hallanalyzer_invalid;
	m_HallAnalyzer->m_SyncCS.Lock();
	
	CameraData_t::const_iterator it = m_HallAnalyzer->m_CameraData.begin();
	HRESULT hr;
	int nCamNumber = 0;
	for( ; it != m_HallAnalyzer->m_CameraData.end(); ++it, ++nCamNumber)
	{
		const Int8Arr& DataArr = it->second.m_Data;
		csalgocommon::TCameraStatistics dt(DataArr);
		hr = m_HallAnalyzer->m_pAnalyzer->SetData( &dt );
		if( FAILED(hr) )
		{
			m_pDebug.PrintW( IDebugOutput::mt_error, L"IHallAnalyzer::LoadCameraData failed with code %d",hr );
	//		return err_hallanalyzer_load;
		}
	}
	m_HallAnalyzer->m_SyncCS.Unlock();
	m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"IHallAnalyzer::LoadCameraData %d cameras loaded", nCamNumber );
	__int32 nSpectators;
	hr = m_HallAnalyzer->m_pAnalyzer->ProcessData( &nSpectators, Plat_AbsoluteTime(), 0 );
	if( FAILED(hr) )
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"IHallAnalyzer::ProcessData failed with code %x",hr );
		return E_FAIL;
	}

//	nSpectators = -1;
	std::vector<BYTE> StubArr/*( 1024, 'A' )*/;
	csinterface::TStateFlag sf;
	hr = m_HallAnalyzer->m_pAnalyzer->GetData( &sf );
	if( hr == S_OK )
	{
		StubArr.assign( (const BYTE*)&sf.data, (const BYTE*)&sf.data + sizeof(__int32) );
	}
	else
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"[hall]IHallAnalyzer::GetData returns %x", hr);
	}

	SpectatorData SpecData;
	SpecData.m_nSpectators	= nSpectators;
	SpecData.m_HallData		= StubArr;
	m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"[hall] PushImageAndData to server" );
	ThreadMessageAutoEnter tme(  m_ServerMessage, true );
	m_ServerMessage.Push( tme, rsp_hall_spectators,  SpecData );

	WCHAR szBuf[100];
	StrFormatByteSizeW( StubArr.size(), szBuf, 100 );
	m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"Spectator number = %d, data size=%s", nSpectators, szBuf );

	return S_OK;
}

void HallServer::DrawState()
{
#if 0
	std::map<int, HWND>::const_iterator it = m_CameraWindows.begin();
	__int32 nSpectator;
	QImage Image(640,480);
	for( ;it != m_CameraWindows.end(); ++it  )
	{
		if( IsWindow(it->second) )
		{
			HWND hWnd = it->second;
			HDC hDC = GetDC( hWnd );
			m_HallAnalyzer->m_pAnalyzer->DrawResult( it->first, &Image, &nSpectator );
			MyDrawColorImage( Image, hDC, CRect(0, 0, 640, 480), INVERT_AXIS_Y );
			ReleaseDC(hWnd, hDC);
		}
	}
#endif
}

HRESULT		HallServer::LoadAlgoParam( const std::vector<BYTE>& AlgoParam, Int8Arr& Data )
{
	if( AlgoParam.empty() )
	{
		m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"[hall] Load default algo parameters" );
		return SetDefaultParameters( &Data, m_pDebug.Get() );
	}
	else
	{
		WCHAR szBuf[100];
		StrFormatByteSizeW( AlgoParam.size(), szBuf, 100 );
		m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"[hall] Load db algo parameters, size=%s", szBuf );
		Data.assign( AlgoParam.begin(), AlgoParam.end() );
		return S_OK;
	}
}

bool	HallServer::SetFilmTime( IHallAnalyzer * pInt, const std::pair<CTime, CTime>& TimeFilm)
{
	CString sBegin = TimeFilm.first.Format(_T("%d %b %y %H:%M"));
	CString sEnd = TimeFilm.second.Format(_T("%d %b %y %H:%M"));
	m_pDebug.PrintW( IDebugOutput::mt_info, L"[hall] Film time = %s - %s", (LPCWSTR)sBegin, (LPCWSTR)sEnd );
	//
	//
	return true;
}