#include "stdafx.h"

using std::max;
using std::min;

#include "CameraServer.h"
#include "../../CSChair/public/NameParse.h"
#include "../../CSChair/dmpstuff/ExceptionHandler.h"
#include "..\Timeouts.h"
#include <shlwapi.h>

//last header
#include "../../CSChair/public/memleak.h"

#define USE_NEW_GRABBER

static void	DeleteCameraAnalyzer( ICameraAnalyzer * p )
{
	p->Release();
}

//=====================================================================================//
//                                 class CameraServer                                  //
//=====================================================================================//
CameraServer::CameraServer() : m_pGrabberFactory(0)
{
}

CameraServer::~CameraServer()
{
}

///////////// UTILITY ///////////////////

const LocalCameraConfig* CameraServer::FindLocalCameraConfig(int CameraNo)
{
	AutoLockCriticalSection alcs( m_CameraThreadCS );
	for( size_t i = 0; i < m_CameraAnalyzers.size(); ++i )
	{
		if( m_LocalCameras[i].m_nCameraNo == CameraNo )
			return &m_LocalCameras[i];
	}
	return NULL;
}
CameraServer::CameraThread* CameraServer::FindCameraThread(int CameraNo)
{
	AutoLockCriticalSection alcs( m_CameraThreadCS );
	for( size_t i = 0; i < m_CameraAnalyzers.size(); ++i )
	{
		if( m_CameraAnalyzers[i]->GetCameraNo() == CameraNo )
			return m_CameraAnalyzers[i].get();
	}
	return NULL;
}

ICameraAnalyzer*	CameraServer::GetCameraAnalyzer(int ZoneNo)	
{
	AutoLockCriticalSection alcs( m_CameraThreadCS );
	for( size_t i = 0; i < m_CameraAnalyzers.size(); ++i )
	{
		if( m_CameraAnalyzers[i]->GetCameraNo() == ZoneNo )
			return m_CameraAnalyzers[i]->GetAnalyzer();
	}
	return NULL;
}

//////// INITIALIZE /////////////////

HRESULT CameraServer::StartCameraThreads(CTime timeBegin, CTime timeEnd)
{
	m_LastFilmTime = std::make_pair( timeBegin, timeEnd );
	HRESULT hr;
	for( size_t i = 0; i < m_LocalCameras.size(); ++i )
	{
		// suspend & resume later
		hr = StartCameraAnalyzer( i, timeBegin, timeEnd, true );
		if( hr != S_OK )
		{
			m_CameraAnalyzers.clear();
			return hr;
		}
	}
	for( size_t i = 0; i < m_CameraAnalyzers.size(); ++i )
	{
		m_CameraAnalyzers[i]->m_Thread.Resume();
	}
	return S_OK;
}

HRESULT	CameraServer::StartCameraAnalyzer(size_t nIndex, CTime timeBegin, CTime timeEnd, bool bSuspend)
{
	if( nIndex >= m_LocalCameras.size() )
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"Failed to start camera with Index=%d, no such index", nIndex );
		return E_INVALIDARG;
	}
	const LocalCameraConfig& cc = m_LocalCameras[ nIndex ] ;

	Int8Arr ParamData;
	HRESULT hr;
	hr = LoadAlgoParam( cc, ParamData );
	if( FAILED(hr) )
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"Failed to get default parameters" );
		return E_FAIL;
	}

	ICameraAnalyzer* pInt;
	DWORD res = CreateCameraAnalyzer( &pInt, &ParamData, &cc.m_Chairs, &cc.m_Background, cc.m_nCameraNo, m_pDebug.Get() );
	if( res || !pInt )
	{
		m_pDebug.PrintW(IDebugOutput::mt_error,L"[camera %d] CreateCameraAnalyzer failed, reason=%d",  cc.m_nCameraNo, res );
	//	throw CameraServerException("Error");
		return E_FAIL;
	}
	bool bRes = SetFilmTime( pInt, cc.m_nCameraNo, timeBegin, timeEnd );
	if(!bRes)
	{
		return E_FAIL;
	}

	CameraAnalyzer_t ca = CameraAnalyzer_t( new CameraThread(pInt, cc.m_nCameraNo ) );
	CameraThreadParam* ct = new CameraThreadParam(this, cc.m_nCameraNo  );
	unsigned int dwId;
	HANDLE hThread = (HANDLE)_beginthreadex( 0, 0, Handled_CameraAnalyzerThreadProc, ct, CREATE_SUSPENDED , &dwId  );
	if( reinterpret_cast<uintptr_t>( hThread ) != -1  )
	{
		ca->m_Thread.Start( hThread );
		m_pDebug.PrintW( IDebugOutput::mt_info, L"[camera %d] thread started", cc.m_nCameraNo);
	}
	else
	{
		m_pDebug.PrintW( IDebugOutput::mt_error, L"[camera %d] Failed to create a thread", cc.m_nCameraNo);
		return E_FAIL;
	}
	if( !bSuspend ) ca->m_Thread.Resume();
	m_CameraAnalyzers.push_back(ca);
	return S_OK;
}

HRESULT CameraServer::StopCameraThreads()
{
//	AutoLockCriticalSection alcs( m_CameraThreadCS );
	
	// nothing to stop
	if(m_CameraAnalyzers.empty()) return ERR_OK;
	size_t dwSize = m_CameraAnalyzers.size();
	std::vector<HANDLE> Threads;
	std::vector<HANDLE> Events;
	for( size_t i = 0; i < m_CameraAnalyzers.size(); ++i )
	{
		Threads.push_back( m_CameraAnalyzers[i]->m_Thread.m_hThread );
		Events.push_back( m_CameraAnalyzers[i]->m_Thread.m_hEvent );
	}

	ASSERT( dwSize <= std::numeric_limits<DWORD>::max() );
	MultipleThreadSync ts( &Threads[0] , &Events[0], (DWORD)dwSize );
	bool res = ts.Stop( ThreadTimeout_CameraAnalyzer );
	m_pDebug.PrintW( IDebugOutput::mt_info, res ? L"Camera threads were stopped safely" : L"Camera threads were terminated");
	m_CameraAnalyzers.clear();
	return res ? ERR_OK : ERR_ERROR;
}

HRESULT	CameraServer::CheckCameraThreads( )
{ 
	AutoLockCriticalSection alcs( m_CameraThreadCS );
	std::vector<size_t> BadThreads;

	BadThreads.reserve( m_CameraAnalyzers.size() );
	std::vector<int> BadNumbers;
	for( size_t i = 0 ; i < m_CameraAnalyzers.size(); ++i )
	{
		std::pair<bool, DWORD> stat = m_CameraAnalyzers[i]->m_Thread.IsThreadEnds();
		//FIXME: return codes
		if( stat.first && (stat.second > 0 && stat.second < 500) )
		{
			m_pDebug.PrintW( IDebugOutput::mt_error, L"[camera %d] thread exits with code=%d, desc=%s", 
				m_CameraAnalyzers[i]->GetCameraNo(), 
				stat.second,
				GetThreadErrorDescription(stat.second)  );
			BadThreads.push_back( i );
			BadNumbers.push_back( m_CameraAnalyzers[i]->GetCameraNo() );
		}
	}

	for( size_t i = 0; i < BadThreads.size() ; ++i)
	{
		size_t nIndex = BadThreads[i] - i;
		ASSERT( nIndex >= 0 );
		m_CameraAnalyzers[ nIndex ].reset();
		m_CameraAnalyzers.erase( m_CameraAnalyzers.begin() + nIndex );
	}

	std::set<int> HaveNumbers;
	size_t nSize = m_CameraAnalyzers.size();
	for( size_t i = 0 ; i < m_CameraAnalyzers.size(); ++i )
	{
		HaveNumbers.insert( m_CameraAnalyzers[i]->GetCameraNo()  );
	}

	std::vector<size_t> NeedRestart;
	NeedRestart.reserve( 10 );
	for( size_t i = 0 ; i < m_LocalCameras.size(); ++i )
	{
		std::set<int>::const_iterator it = HaveNumbers.find( m_LocalCameras[i].m_nCameraNo);
		if( it == HaveNumbers.end() )
		{
			NeedRestart.push_back( i );
		}
	}

	HRESULT hr;
	for( size_t i = 0 ; i < NeedRestart.size(); ++i)
	{
		int nIndex = NeedRestart[i];
		m_pDebug.PrintW( IDebugOutput::mt_warning, L"[camera %d] thread is not working and will be restarted", 
						m_LocalCameras[nIndex].m_nCameraNo);
		hr = StartCameraAnalyzer( nIndex, m_LastFilmTime.first, m_LastFilmTime.second, false);
		if( hr != S_OK ) return hr;
	}
	return S_OK;
}

/////////////// THREAD /////////////////////////

unsigned int WINAPI CameraServer::Handled_CameraAnalyzerThreadProc(void * pParam)
{
	unsigned int Res = err_exception;
	__try
	{
		Res = CameraThread::ThreadProc(pParam);
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), _T("CameraAnalyzer thread"))
#ifndef _DEBUG
		, EXCEPTION_EXECUTE_HANDLER
#endif
		)
	{
	}
	return Res;
}

unsigned int WINAPI CameraServer::CameraThread::ThreadProc(void * pParam)
{
	struct ComInitializer
	{
		HRESULT m_hr;
		ComInitializer() { m_hr = CoInitializeEx(0, COINIT_MULTITHREADED);}
		~ComInitializer(){ if( m_hr == S_OK) CoUninitialize();}
	} m_ComInit;	

	CameraThreadParam*	pCamParam	= (CameraThreadParam*)pParam;
	CameraServer*		pThis		= pCamParam->m_pServer;
	int					CameraNo	= pCamParam->m_CameraNo;
	delete	pCamParam;

	SimpleTimer	timerFpsProcessing( 60 * 1000); // 1min
	SimpleTimer	timerRendering( 5 * 1000); // 5sec

	__int64		nProcessedFrames = 0;

	const int	c_nMaxFailedSequence	= 10 * 2; //after this limit thread will be restarted
	int			nFailedSequence			= 0;

	boost::shared_ptr< IBaseGrabber >		FrameGrabber;
	// FIXME: check for NULL
	std::wstring sUrl = pThis->FindLocalCameraConfig(CameraNo)->m_sVideoSourceUrl;
	NameParse np( sUrl.c_str() );
	IBaseGrabber* pGrabber = NULL;
	HRESULT hr;
	std::wstring sSource = np.GetSource();
	switch( np.GetType() )
	{
	case vidt_file :
		{
		IFileGrabber* pFileGrab;
		hr = pThis->m_pGrabberFactory->CreateFileGrabber( &pFileGrab, sSource.c_str(), 8, false, true );
		if( hr == S_OK )
		{
			FrameGrabber = boost::shared_ptr< IBaseGrabber >( pFileGrab, ReleaseInterface<IBaseGrabber>() ); 
			pGrabber = pFileGrab;
		}
		else
		{
			pThis->m_pDebug.PrintW(IDebugOutput::mt_error, L"[camera %d] Failed to create IFileGrabber for %s", CameraNo, sSource.c_str() );
			pGrabber = NULL;
		}
		}
		break;
	case vidt_camera :
		{
		ICameraGrabber* pCameraGrab;
		hr = pThis->m_pGrabberFactory->CreateCameraGrabber( &pCameraGrab, CameraNo );
		if( hr == S_OK )
		{
			FrameGrabber = boost::shared_ptr< IBaseGrabber >( pCameraGrab, ReleaseInterface<IBaseGrabber>() ); 
			pGrabber = pCameraGrab;
		}
		else
		{
			pThis->m_pDebug.PrintW(IDebugOutput::mt_error, L"[camera %d] Failed to create ICameraGrabber", CameraNo);
			pGrabber = NULL;
		}
		}
		break;
	}

	if(  !pGrabber ) 
	{
		RETURN (err_grabber_invalid_url);
	}
	Sleep(10);

	ICameraAnalyzer * pAnalyzer = pThis->GetCameraAnalyzer( CameraNo );
	if( pAnalyzer)
		pAnalyzer->Start( Plat_AbsoluteTime() );

	timerFpsProcessing.Start();
	timerRendering.Start();

	__int64		timeFpsProcessing = Plat_Int64Time();
	CameraThread* pThread;
	int		nLockFrameSequence = 0;
	const int c_nMaxLockFrameSequence = 100;
	while(  (pThread = pThis->FindCameraThread(CameraNo)) && pThread->m_Thread.CanThreadWork(10 )   )
	{
		LPCBITMAPINFO pBmpInfo;
		const BYTE* pImage;
		int ImgSize;
		bool res; HRESULT hrRes;
		ICameraAnalyzer * pAnalyzer = pThis->GetCameraAnalyzer( CameraNo );
		if( !pAnalyzer )
		{
			RETURN(err_camanalyzer_invalid);
		}
		res = pGrabber->LockFrame( pBmpInfo, pImage, ImgSize ) == S_OK;
		if( res && ( !pBmpInfo || !pImage || !ImgSize ) )
		{
			pThis->m_pDebug.PrintW( IDebugOutput::mt_warning, L"[camera %d] IBaseGrabber::LockFrame have executed successfully, but some of output parameters are invalid", CameraNo );
			pGrabber->UnlockFrame();
			res = false;
		}


		if(res)
		{
			nLockFrameSequence = 0; // lock succeeded
			bool bRequiredData = pThread->PopThreadMessage(pThis);
			if( bRequiredData )
			{
				HRESULT hr;
				pThis->m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"[camera %d] RequestData received, start processing...", CameraNo );
				bool res;
				CameraImage CameraImageProcess;
#if 0
				res = CameraImage.Fill( pBmpInfo, pImage );
#else
				csalgocommon::TDemoImage DemoImg;
				hr = pAnalyzer->GetData( &DemoImg );
				if( hr != S_OK )
				{
					pThis->m_pDebug.PrintW( IDebugOutput::mt_error, L"[camera %d] failed to get a demo image from ICameraAnalyzer, use the native image", CameraNo );
					res = CameraImageProcess.Fill( pBmpInfo, pImage );
				}
				else
					res = CameraImageProcess.Fill( DemoImg.data.second );
#endif
				if(!res)
				{
					pThis->m_pDebug.PrintW( IDebugOutput::mt_error, L"[camera %d] failed to save image", CameraNo );
				}
				csalgocommon::TCameraStatistics dt;
				hr = pAnalyzer->GetData( &dt );
				if( hr != S_OK )
				{
					pThis->m_pDebug.PrintW(IDebugOutput::mt_error, L"[camera %d] ICameraAnalyzer::GetData failed", CameraNo );
				}
				else
					pThread->PushImageAndData( pThis, CameraImageProcess.GetImage(), &dt.data[0]  , dt.data.size() );
			}

			hrRes = pAnalyzer->ProcessImage( pBmpInfo, pImage, Plat_AbsoluteTime() );
			if(FAILED(hrRes))
			{
				pThis->m_pDebug.PrintW(IDebugOutput::mt_error, L"[camera %d] ICameraAnalyzer::ProcessImage failed", CameraNo );
				++nFailedSequence;
			}
			else
			{
				// test//
#ifdef TEST_RESTART_THREAD 			
				nFailedSequence++;
#else
				nFailedSequence = 0;
#endif
			}
#ifdef USE_NEW_GRABBER
			res = pGrabber->UnlockFrame() == S_OK;
#else
			res = pGrabber->UnlockProcessedFrame();
#endif
			if( nFailedSequence > c_nMaxFailedSequence )
			{
				// exit
				if( pAnalyzer)
					pAnalyzer->Stop( Plat_AbsoluteTime() );
#ifndef USE_NEW_GRABBER
				if(  pGrabber ) pGrabber->Stop();
#endif
				pThis->m_pDebug.PrintW(IDebugOutput::mt_error, L"[camera %d] ICameraAnalyzer::ProcessFailed - too many faults", CameraNo );
				RETURN( err_camanalyzer_process  ) ;
			}
			if(!res)
			{
				pThis->m_pDebug.PrintW(IDebugOutput::mt_error, L"[camera %d] ICameraAnalyzer::UnlockProcessedFrame failed", CameraNo );
			}
			++nProcessedFrames;
		}
		else
		{
			++nLockFrameSequence;
			if( nLockFrameSequence > c_nMaxLockFrameSequence )
			{
				// exit
				if( pAnalyzer) pAnalyzer->Stop( Plat_AbsoluteTime() );
				pThis->m_pDebug.PrintW(IDebugOutput::mt_error, L"[camera %d] IBaseGrabber::LockLatestFrame - too many sequent faults, max allowed=%d", CameraNo, c_nMaxLockFrameSequence );
				RETURN( err_grabber_lock  ) ;
			}
			else
				pThis->m_pDebug.PrintW(IDebugOutput::mt_error, L"[camera %d] IBaseGrabber::LockLatestFrame failed", CameraNo );
			// wait some time
			Sleep(50);
		}
		if( timerFpsProcessing.IsSignaled() )
		{
			timerFpsProcessing.Start();
			if( nProcessedFrames )
			{
				__int64 timeNewFpsProcessing = Plat_Int64Time();
				double fFps = (1000.0 * nProcessedFrames)/double( timeNewFpsProcessing -  timeFpsProcessing);
				nProcessedFrames = 0;
				timeFpsProcessing = timeNewFpsProcessing;
				pThis->m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"[camera %d] %.1f fps",
					CameraNo, fFps );
			}
		}
		if( timerRendering.IsSignaled() )
		{
			timerRendering.Start();
			pThis->RenderCamera( CameraNo, pAnalyzer );
		}
	}
	if( pAnalyzer)
		pAnalyzer->Stop( Plat_AbsoluteTime() );
#ifndef USE_NEW_GRABBER
	if(  pGrabber ) pGrabber->Stop();
#endif
	RETURN(err_noerror) ;
}

bool		CameraServer::CameraThread::PopThreadMessage( CameraServer* pServer )
{
	// don't block, try later instead
	ThreadMessageAutoEnter tme(  m_CameraMessage, false );
	bool bRequestData = false;
	if( tme.IsEntered() && !m_CameraMessage.IsEmpty(tme) )
	{
		int nID;
		const boost::any&	Data = m_CameraMessage.Front( tme, nID );
		switch( nID )
		{
		case rqt_cam_data:
			// no data, just id
			bRequestData = true;
			break;
		case hall_cam_data:
		try
		{
			const std::vector<BYTE>& HallData = boost::any_cast<std::vector<BYTE> >	( Data );
			SetHallData( pServer, HallData );
		}
		catch ( boost::bad_any_cast) 
		{
		}
		break;
		default:
			pServer->m_pDebug.PrintW( IDebugOutput::mt_error, L"[camera %d] unknown message=%d", m_nCameraNo, nID );
		}
		m_CameraMessage.Pop( tme );
	}
	return bRequestData;
}

void		CameraServer::CameraThread::PushImageAndData(	CameraServer* pServer ,
															const std::vector<BYTE>& ImageBuf, 
															const void* pData, size_t nSize  )
{
	ASSERT(pServer);
	std::vector<BYTE> DataArr( (const BYTE*)pData, (const BYTE*)pData + nSize );
	// wait cause we lost message
	pServer->m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"[camera %d] PushImageAndData to server", m_nCameraNo );
	ThreadMessageAutoEnter tme(  pServer->m_ServerMessage, true );
	pServer->m_ServerMessage.Push( tme, rsp_cam_data,  
		CameraDataForProcessing( m_nCameraNo, DataArr, ImageBuf)  );
}

HRESULT		CameraServer::LoadAlgoParam( const LocalCameraConfig& cc, Int8Arr& Data )
{
	if( cc.m_AlgoParam.empty() )
	{
		m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"[camera %d] Load default algo parameters", cc.m_nCameraNo );
		return SetDefaultParameters( &Data, m_pDebug.Get() );
	}
	else
	{
		Data.assign( cc.m_AlgoParam.begin(), cc.m_AlgoParam.end() );
		WCHAR szBuf[100];
		StrFormatByteSizeW( Data.size(), szBuf, 100 );
		m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"[camera %d] Load db algo parameters, size=%s", cc.m_nCameraNo, szBuf );
		return S_OK;
	}
}

void	CameraServer::RenderCamera( int nCameraNo, ICameraAnalyzer* pAnalyzer )
{
	// readonly, so no sync
	std::map<int, HWND>::const_iterator it = m_CameraWindows.find( nCameraNo );
	if( it != m_CameraWindows.end() )
	{
		csalgocommon::TDemoImage DemoImg;
		HRESULT hr = pAnalyzer->GetData( &DemoImg );
		if( hr == S_OK )
		{
			QImage& Img = DemoImg.data.second;
			HDC hDC = ::GetDC( it->second );
			MyDrawColorImage( Img, hDC, CRect( 0, 0, Img.width(), Img.height() ), INVERT_AXIS_Y );
			::ReleaseDC( it->second, hDC );
		}
	}
}

void	CameraServer::CameraThread::SetHallData( CameraServer* pServer, const std::vector<BYTE>& Data )
{
	if( Data.size() != sizeof(__int32) )
	{
		pServer->m_pDebug.PrintW( IDebugOutput::mt_error, L"[camera %d]Wrong data size", m_nCameraNo);
		return;
	}
	ICameraAnalyzer * pAnalyzer = pServer->GetCameraAnalyzer( m_nCameraNo );
	if( pAnalyzer )
	{
		csinterface::TStateFlag Flag;
		Flag.data = *(const __int32*)&Data[0];
		HRESULT hr = pAnalyzer->SetData( &Flag );
		if( hr == S_OK )
			pServer->m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"[camera %d]SetHallData succeeded", m_nCameraNo);
		else 
			pServer->m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"[camera %d]SetHallData failed, res = %x", m_nCameraNo, hr);
	}
	else
	{
		pServer->m_pDebug.PrintW( IDebugOutput::mt_debug_info, L"[camera %d]SetHallData, pAnalyzer == 0", m_nCameraNo);
	}
}

bool	CameraServer::SetFilmTime( ICameraAnalyzer* pInt, int nCameraNo, CTime timeBegin, CTime timeEnd)
{
	CString sBegin	= timeBegin.Format(_T("%d %b %y %H:%M"));
	CString sEnd	= timeEnd.Format(_T("%d %b %y %H:%M"));
	m_pDebug.PrintW( IDebugOutput::mt_info, L"[camera %d] Film time = %s - %s", nCameraNo, (LPCWSTR)sBegin, (LPCWSTR)sEnd );
	//
	//
	csinterface::TSurveillanceInterval data ( std::make_pair( timeBegin.GetTime() * 1000, timeEnd.GetTime()* 1000 ) );
	return pInt->SetData(&data) == S_OK;
}