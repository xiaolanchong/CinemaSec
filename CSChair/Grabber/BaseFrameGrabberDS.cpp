// BaseFrameGrabberDS.cpp: implementation of the BaseFrameGrabberDS class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <initguid.h>
#include "BaseFrameGrabberDS.h"
#if (defined _DEBUG) && (defined USE_TRACE)
#include "fasttimer.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////

bool BaseFrameGrabberDS::Start( LPCTSTR name, const FrameGrabberParameters & param ) 
{
#if (defined _DEBUG) 
	m_dwLockNumber	= 0;
	m_dwTimeTicks	= 0;
#endif
	m_hrCoInit = CoInitializeEx(0, COINIT_MULTITHREADED );
	USES_CONVERSION;
	LPCWSTR sz = T2CW( name );
	try
	{
		Grab( sz );
	}
	catch(std::exception)
	{
		return false;
	}
	return true;
}

bool BaseFrameGrabberDS::Stop() 
{
//	TRACE1( "Number of locks %d\n", m_dwLockNumber );
#ifdef _DEBUG
	TRACE1( "Number of locks %d\n", m_dwLockNumber );
	if (m_dwLockNumber > 0)
		TRACE1( "[Average time of frame processing = %dms]\n", m_dwTimeTicks/m_dwLockNumber);
#endif
	return StopGraph();
}

bool BaseFrameGrabberDS::IsOk() const 
{
	return GetState() == State_Running;
}

bool BaseFrameGrabberDS::IsEnd() const 
{
	if( !m_pGraph ) return true;
	// always false when loopback
	if( m_bLoopback) return false;
	CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( m_pGraph );
	LONGLONG Current, All;
	pSeeking->GetCurrentPosition( &Current );
	pSeeking->GetDuration(  &All);
	float z  = Current / float( All );
	TRACE1("Current pos = %f\n",z);
	return Current / float( All ) > 0.99; 	
}

bool BaseFrameGrabberDS::LockLatestFrame( LPCBITMAPINFO & pHeader, LPCUBYTE & pImage, int & imgSize )  
{
#if (defined _DEBUG) && (defined USE_TRACE)
	CFastTimer FastTimer;
	FastTimer.Start();
	DWORD dwStartTime = timeGetTime();
#endif
	if( !m_pGrabber ) 
	{
		pHeader	= NULL;
		pImage	= NULL;
		imgSize	= 0;
		return false;
	}
	HRESULT hr;
	if(m_bPerFrameMode)
	{
		hr = m_pFrameStep->Step(1, NULL);
		if( hr != S_OK ) return false;
#if 1
		long evCode;
		LONG_PTR evParam1, evParam2;
		//while( GetMessage( &msg,  ) )
		while( true )
		{
			hr = m_pMediaEvent->GetEvent( &evCode, &evParam1, &evParam2, INFINITE );
			ProcessCode(evCode);
			if( SUCCEEDED(hr) && 
				(evCode == EC_STEP_COMPLETE  || 
				 evCode == EC_COMPLETE ) )
			{
				TCHAR buf[255];
				INT64 cur, all;
				GetCurFrame( cur );
				GetTotalFrames( all );
				_sntprintf( buf, 255, _T("%I64u/%I64u\n"), cur, all );
				OutputDebugString( buf );
				hr = m_pMediaEvent->FreeEventParams(evCode, evParam1, evParam2);
				break;
			}
		}
#else
		CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(m_pGraph );
		OAFilterState state;
		do
		{
			pControl->GetState(20, &state);
		} while (state != State_Paused);
		TCHAR buf[255];
		INT64 cur, all;
		GetCurFrame( cur );
		GetTotalFrames( all );
		_sntprintf( buf, 255, _T("%I64u/%I64u\n"), cur, all );
		OutputDebugString( buf );
#endif
	}

	long	BufferSize;
	
	hr = m_pGrabber->GetCurrentBuffer(  &BufferSize, 0);
	if( FAILED(hr) ) return false;
	m_SampleBuffer.resize( BufferSize );
	hr = m_pGrabber->GetCurrentBuffer( &BufferSize, (long*)&m_SampleBuffer[0] );
	if( FAILED(hr) ) return false;
	pHeader = &m_bi;
	pImage	= &m_SampleBuffer[0];
	imgSize	= BufferSize;
#ifdef _DEBUG
	InterlockedIncrement( &m_dwLockNumber );
#endif

#if (defined _DEBUG) && (defined USE_TRACE)
	FastTimer.End();
	m_dwTimeTicks += timeGetTime() - dwStartTime;
#endif
	return true;
}

bool BaseFrameGrabberDS::UnlockProcessedFrame() 
{
	return true;
}

bool BaseFrameGrabberDS::HasFreshFrame() const 
{
//	return m_BitBuffer.WasWritten();
	HRESULT hr;
	long BufferSize;
	hr = m_pGrabber->GetCurrentBuffer(  &BufferSize, 0);
	return hr == S_OK && BufferSize != 0;
}


/////////////////////////////////////////////////////////////////////////////////////
static HRESULT GetPin(IBaseFilter * pFilter, PIN_DIRECTION dirrequired,  int iNum, IPin **ppPin);
static IPin *  GetInPin ( IBaseFilter *pFilter, int Num );
static IPin *  GetOutPin( IBaseFilter *pFilter, int Num );


static HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
{
    CComPtr< IEnumPins > pEnum;
    *ppPin = NULL;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) 
        return hr;

    ULONG ulFound;
    IPin *pPin;
    hr = E_FAIL;

    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        PIN_DIRECTION pindir = (PIN_DIRECTION)3;

        pPin->QueryDirection(&pindir);
        if(pindir == dirrequired)
        {
            if(iNum == 0)
            {
                *ppPin = pPin;  // Return the pin's interface
                hr = S_OK;      // Found requested pin, so clear error
                break;
            }
            iNum--;
        } 

        pPin->Release();
    } 

    return hr;
}


static IPin * GetInPin( IBaseFilter * pFilter, int nPin )
{
    CComPtr<IPin> pComPin=0;
    GetPin(pFilter, PINDIR_INPUT, nPin, &pComPin);
    return pComPin;
}


static IPin * GetOutPin( IBaseFilter * pFilter, int nPin )
{
    CComPtr<IPin> pComPin=0;
    GetPin(pFilter, PINDIR_OUTPUT, nPin, &pComPin);
    return pComPin;
}

#define FOURCC_Y800 mmioFOURCC('Y','8','0','0')
#define FOURCC_GRAY mmioFOURCC('G','R','A','Y')
#define FOURCC_Y8   mmioFOURCC('Y','8',' ',' ')

DEFINE_GUID(MEDIASUBTYPE_Y800,	FOURCC_Y800, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

#ifndef CheckPointer
#define CheckPointer(p,ret) {if((p)==NULL) return (ret);}
#endif

bool CreateFilter(IBaseFilter **ppFilter, LPCTSTR stFilterMoniker)
{
	CheckPointer(ppFilter, false);
	CheckPointer(stFilterMoniker, false);

	*ppFilter = NULL;

	HRESULT hr;
	IBindCtx *pBC = NULL;

	hr = CreateBindCtx(0, &pBC);
	if(SUCCEEDED(hr))
	{
		DWORD dwEaten;
		IMoniker *pmCam = NULL;

#ifdef UNICODE
		hr = MkParseDisplayName(pBC, stFilterMoniker, &dwEaten, &pmCam);
#else
		WCHAR swFilterMoniker[MAX_PATH];

		MultiByteToWideChar(CP_ACP, 0, stFilterMoniker, lstrlen(stFilterMoniker) + 1,
			swFilterMoniker, MAX_PATH);

		hr = MkParseDisplayName(pBC, swFilterMoniker, &dwEaten, &pmCam);
#endif
		if(SUCCEEDED(hr))
		{
			pmCam->BindToObject(0, 0, IID_IBaseFilter, (void**)ppFilter);
			pmCam->Release();
		}

		pBC->Release();
	}

	return (*ppFilter == NULL) ? false : true;
}

void	BaseFrameGrabberDS::Grab( const std::wstring& FileName)
{
	HRESULT hr;

	try
	{

    hr = m_pGrabber.CoCreateInstance( CLSID_SampleGrabber );
    if( !m_pGrabber )	throw VideoFileException2("Can't create sample grabber");


    CComQIPtr< IBaseFilter, &IID_IBaseFilter > pGrabberBase( m_pGrabber );

	CComPtr< IBaseFilter >		pSource;
    pSource.CoCreateInstance( CLSID_AsyncReader );
    if( !pSource )throw DirectShowException2("Can't create async reader");

    m_pGraph.CoCreateInstance( CLSID_FilterGraph );
    if( !m_pGraph )throw DirectShowException2("Can't create filter graph");

    hr = m_pGraph->AddFilter( pSource, L"Source" );
	if( FAILED(hr) ) throw DirectShowException2("Can't add source filter");
    hr = m_pGraph->AddFilter( pGrabberBase, L"Grabber" );
	if( FAILED(hr) ) throw DirectShowException2("Can't add grabber filter");

    CComQIPtr< IFileSourceFilter, &IID_IFileSourceFilter > pLoad( pSource );
    hr = pLoad->Load(  FileName.c_str() , NULL );
    if( FAILED( hr ) )
    {
        if( !m_pGraph )throw DirectShowException2("Can't load video file");
    }
	AM_MEDIA_TYPE GrabType;
	memset( &GrabType, 0, sizeof(AM_MEDIA_TYPE) );
	GrabType.majortype	= MEDIATYPE_Video;
	GrabType.formattype = FORMAT_VideoInfo;

	// FIXME
	switch( m_nBitPerPixel )
	{
	case FOURCC_Y800:
	case FOURCC_Y8:
		//	case FOURCC_GRAY:
	case MY_BI_GRAY : GrabType.subtype = MEDIASUBTYPE_Y800; break;
	case 8 :	GrabType.subtype = MEDIASUBTYPE_RGB8; break;
	case 16 :	GrabType.subtype = MEDIASUBTYPE_RGB565; break;
	case 24 :	GrabType.subtype = MEDIASUBTYPE_RGB24; break;
	case MY_BI_YUY2 : 
		GrabType.subtype = MEDIASUBTYPE_YUY2; break;
	case 32 :	
	default:	GrabType.subtype = MEDIASUBTYPE_RGB32; break;
	}

	//-----------------------------------------------------------------

	// Multimedia format types are marked with DWORDs built from four 8-bit
	// chars and known as FOURCCs. New multimedia AM_MEDIA_TYPE definitions include
	// a subtype GUID. In order to simplify the mapping, GUIDs in the range:
	//    XXXXXXXX-0000-0010-8000-00AA00389B71
	// are reserved for FOURCCs.


	// Add converter to graph
	//

	IBaseFilter *pY800Encoder = NULL;

	if(CreateFilter(&pY800Encoder, TEXT("@device:cm:{33D9A760-90C8-11D0-BD43-00A0C911CE86}\\y800")))
	{
		m_pGraph->AddFilter(pY800Encoder, L"Y800 Encoder");
		pY800Encoder->Release();
	}	

	//--------------------------------------------------------------------	

	
    hr = m_pGrabber->SetMediaType( &GrabType );

    // Get the output pin and the input pin
    //
    CComPtr< IPin > pSourcePin;
    CComPtr< IPin > pGrabPin;

    pSourcePin = GetOutPin( pSource, 0 );
    pGrabPin   = GetInPin( pGrabberBase, 0 );

	CComPtr< IPin >  pGrabPinOutput   = GetOutPin( pGrabberBase, 0 );
	CComPtr< IBaseFilter> GraphRender;
	// Null render can't support step mode
	hr = GraphRender.CoCreateInstance( m_bPerFrameMode?CLSID_VideoRendererDefault: CLSID_NullRenderer);
	if( FAILED( hr ) )
	{
		throw DirectShowException2("Can't create Null Render");
	}

	hr = m_pGraph->AddFilter( GraphRender, L"Render" );
	CComPtr< IPin >  pRenderPin   = GetInPin( GraphRender, 0 );

    hr = m_pGraph->Connect( pSourcePin, pGrabPin );
    if( FAILED( hr ) )
    {
        throw DirectShowException2("Can't connect pins in render graph");
    }
	hr = m_pGraph->Connect(   pGrabPinOutput, pRenderPin );
	if( FAILED( hr ) )
	{
		throw DirectShowException2("Can't connect pins in render graph");
	}

    AM_MEDIA_TYPE mt;
    hr = m_pGrabber->GetConnectedMediaType( &mt );

	if( mt.majortype	!= MEDIATYPE_Video ||
		mt.formattype	!= FORMAT_VideoInfo ||
		mt.cbFormat >= sizeof(VIDEOINFO) )
		throw DirectShowException2("Invalid output format");

    VIDEOINFO * vih = (VIDEOINFO*) mt.pbFormat;
	memcpy( &m_bi.bmiHeader, &vih->bmiHeader, sizeof( BITMAPINFOHEADER ) );
	int nBitCount =  vih->bmiHeader.biBitCount;
	if( nBitCount > 0 && nBitCount <= 8 )
	{
		DWORD dwPaletteSize = m_bi.bmiHeader.biClrUsed ? m_bi.bmiHeader.biClrUsed : (1 << nBitCount);
		memcpy( &m_bi.bmiColors, &vih->bmiColors, sizeof( RGBQUAD ) * dwPaletteSize );
		m_bi.bmiHeader.biPlanes = 1;
	}
	m_fFPS = 10000000.0/vih->AvgTimePerFrame; // in 100 ns units

	CSize Size ( vih->bmiHeader.biWidth, vih->bmiHeader.biHeight );
	SetSize( vih->bmiHeader.biWidth, vih->bmiHeader.biHeight);

	CoTaskMemFree( mt.pbFormat );

    CComPtr <IPin> pGrabOutPin = GetOutPin( pGrabberBase, 0 );

	hr = m_pGrabber->SetBufferSamples( TRUE );
	hr = m_pGrabber->SetOneShot( FALSE);

    CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( m_pGraph );

	hr = pSeeking->SetTimeFormat( &TIME_FORMAT_FRAME );
	LONGLONG pos = 0;
	pSeeking->SetPositions( &pos, AM_SEEKING_AbsolutePositioning ,NULL, AM_SEEKING_NoPositioning );
    CComQIPtr< IVideoWindow, &IID_IVideoWindow > pWindow = m_pGraph;
    if (pWindow)
    {
        hr = pWindow->put_AutoShow(OAFALSE);
    }

    hr = m_pGraph.QueryInterface( &m_pMediaEvent );
	if( hr != S_OK )
	{
		 throw DirectShowException2("Can't get IMediaEventEx interface");
	}

	if( m_hWndStatic )
	{
		hr = m_pMediaEvent->SetNotifyWindow( (OAHWND)m_hWndStatic, WM_GRAPHNOTIFY , 0);
	}
	
	if( m_bPerFrameMode)
	{
		hr = m_pGraph.QueryInterface( &m_pFrameStep ); 
		if( hr != S_OK )
		{
			 throw DirectShowException2("Can't get IVideoFrameStep interface");
		}
		hr = m_pFrameStep->CanStep( 1, NULL);
		if(hr != S_OK)
		{
			throw DirectShowException2("Can't step through graph interface");
		}
	}

	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(m_pGraph );
	hr = AddToRot(m_pGraph);
	hr = pControl->Run( );

	OAFilterState state;
	do
	{
		pControl->GetState(0, &state);
	} while (state != State_Running);
	if( m_bPerFrameMode)
	{
	pControl->Pause();
	LONGLONG nStreamLengthVid;
	pSeeking->SetPositions( 0, AM_SEEKING_AbsolutePositioning,&nStreamLengthVid, AM_SEEKING_NoPositioning);
	}
	}
	catch( DirectShowException2)		// release all comptrs
	{
		throw;
	}
}

bool		BaseFrameGrabberDS::StopGraph()
{
	HRESULT hr;
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(m_pGraph );
	hr = pControl->Stop();

	CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( m_pGraph );
	LONGLONG pos = 0;
	pSeeking->SetPositions( &pos, AM_SEEKING_AbsolutePositioning ,NULL, AM_SEEKING_NoPositioning );
	m_pFrameStep.Release();
	m_pMediaEvent.Release();
	m_pGrabber.Release();
	m_pGraph.Release();
	RemoveFromRot();
	if( m_hrCoInit == S_OK ) 
	{
		CoUninitialize();
		m_hrCoInit = E_FAIL;
	}
	return SUCCEEDED(hr);
}

bool		BaseFrameGrabberDS::PauseGraph()
{
	HRESULT hr;
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(m_pGraph );
	hr = pControl->Pause();
	return SUCCEEDED(hr);
}

bool		BaseFrameGrabberDS::	PlayGraph()
{
	HRESULT hr;
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(m_pGraph );
	hr = pControl->Run();
	return SUCCEEDED(hr);
}

OAFilterState	BaseFrameGrabberDS::GetState() const
{
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl( m_pGraph );
	OAFilterState fs;
	pControl->GetState(10, &fs);
	return fs;
}

float			BaseFrameGrabberDS::GetPos() const
{
	CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( m_pGraph );
	LONGLONG Current, All;
	pSeeking->GetCurrentPosition( &Current );
	pSeeking->GetDuration(  &All);
	return Current/(float)All;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BaseFrameGrabberDS::BaseFrameGrabberDS(int nBitPerPixel, bool bPerFrameMode , bool bLoopback):
	m_fRate(1.0f) ,
	m_bPerFrameMode(bPerFrameMode),
	m_bLoopback(bLoopback),
	m_nBitPerPixel(nBitPerPixel),
	m_hrCoInit( E_FAIL )
{
	m_hWndStatic	= CreateWindow( _T("Static"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	SetWindowLongPtr(m_hWndStatic, GWL_USERDATA, (LONG_PTR) this );
	m_fnOldWndProc	= (WNDPROC)SetWindowLongPtr(m_hWndStatic, GWL_WNDPROC, (LONG_PTR) WindowProc);
}

BaseFrameGrabberDS::~BaseFrameGrabberDS()
{
//	m_Sync.Lock();

	if( m_pGraph.p )
	{
		Stop();
#if 0
		CComQIPtr< IMediaEvent, &IID_IMediaEvent > pEvent(m_pGraph );
	
		long code = 0;
		pEvent->WaitForCompletion(INFINITE, &code );
#endif 
	}
	DestroyWindow(m_hWndStatic);
//	m_Sync.Unlock();
}

HRESULT BaseFrameGrabberDS::AddToRot(IUnknown *pUnkGraph) 
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT))) {
        return E_FAIL;
    }
    WCHAR wsz[256];
    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) {
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,pMoniker, &m_dwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    return hr;
}


void BaseFrameGrabberDS::RemoveFromRot()
{
    IRunningObjectTable *pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
        pROT->Revoke(m_dwRegister);
        pROT->Release();
    }
}

HRESULT BaseFrameGrabberDS::GetCurFrame( INT64 &nCurPos )
{
	HRESULT hr;
	CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( m_pGraph );
	if( !pSeeking ) return E_NOINTERFACE; 
	LONGLONG Current;
	hr = pSeeking->GetCurrentPosition( &Current );
	if( hr != S_OK ) return hr;
	nCurPos = Current;
	return S_OK;
}

HRESULT BaseFrameGrabberDS::GetTotalFrames( INT64 &nTotal )
{
	HRESULT hr;
	CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( m_pGraph );
	if( !pSeeking ) return E_NOINTERFACE; 
	LONGLONG All;

	hr = pSeeking->GetDuration(  &All);

	if( hr != S_OK ) return hr;
	nTotal = All;
	return S_OK;
}

HRESULT BaseFrameGrabberDS::Seek( INT64 nCurPos )
{
	HRESULT hr;
	CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( m_pGraph );
	if( !pSeeking ) return E_NOINTERFACE;
	hr = pSeeking->SetPositions( &nCurPos, AM_SEEKING_AbsolutePositioning ,NULL, AM_SEEKING_NoPositioning);
	return hr;
}

HRESULT BaseFrameGrabberDS::LockFrame( const BITMAPINFO * & pHeader, const BYTE * & pImage, int &  nImgSize  ) 
{
	return LockLatestFrame( pHeader, pImage, nImgSize ) ? S_OK : E_FAIL;
}

HRESULT BaseFrameGrabberDS::UnlockFrame()
{
	return UnlockProcessedFrame() ? S_OK : E_FAIL ;
}

HRESULT BaseFrameGrabberDS::GetFPS(double& fFPS)
{
	fFPS = m_fFPS;
	return S_OK;
}

//////// window subclassing
LRESULT	BaseFrameGrabberDS::WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam  )
{
	LONG_PTR lData = GetWindowLongPtr( hWnd, GWL_USERDATA );
	BaseFrameGrabberDS* pThis = (BaseFrameGrabberDS*)lData;
	if( msg == WM_GRAPHNOTIFY && pThis)
	{
		LONG evCode, evParam1, evParam2;
		HRESULT hr;

		CComQIPtr< IMediaEvent, &IID_IMediaEvent > pME(pThis->m_pGraph );
		// Process all queued events
		while(SUCCEEDED(pME->GetEvent(&evCode, (LONG_PTR *) &evParam1,
			(LONG_PTR *) &evParam2, 0)))
		{
			// Free memory associated with callback, since we're not using it
			hr = pME->FreeEventParams(evCode, evParam1, evParam2);
#if 0
			// If this is the end of the clip, reset to beginning
			if(EC_COMPLETE == evCode && pThis->m_bLoopback )
			{
				// Reset to first frame of movie
				CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( pThis->m_pGraph );
				LONGLONG pos = 0;
				pSeeking->SetPositions( &pos, AM_SEEKING_AbsolutePositioning ,NULL, AM_SEEKING_NoPositioning );
				CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(pThis->m_pGraph );
				pControl->Run( );	
			}
#else
			pThis->ProcessCode( evCode ); 
#endif
		}
	}

	return CallWindowProc(pThis->m_fnOldWndProc, hWnd, msg, wParam, lParam);
}

void	BaseFrameGrabberDS::ProcessCode( LONG evCode )
{
	if(EC_COMPLETE == evCode && m_bLoopback )
	{
		// Reset to first frame of movie
		CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( m_pGraph );
		LONGLONG pos = 0;
		pSeeking->SetPositions( &pos, AM_SEEKING_AbsolutePositioning ,NULL, AM_SEEKING_NoPositioning );
		CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(m_pGraph );
		pControl->Run( );	
	}
}