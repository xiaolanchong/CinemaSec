// VideoBackground.cpp: implementation of the CVideoBackground class.
//
//////////////////////////////////////////////////////////////////////
#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "VideoBackground.h"

//#include "EditorState.h"
#include "../Common/Exception.h"

//#include "ResourceMgr.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


STDMETHODIMP CVideoBackground::BufferCB( double SampleTime, BYTE * pBuffer, long BufferSize )
    {

        BITMAPINFOHEADER bih;
        memset( &bih, 0, sizeof( bih ) );
        bih.biSize = sizeof( bih );
        bih.biWidth = m_Width;
        bih.biHeight = m_Height;
        bih.biPlanes = 1;
        bih.biBitCount = 24;

		

		if( m_Sync.Try())
		{		

				memcpy( &m_bi, &bih, sizeof(BITMAPINFOHEADER));
				m_Bits.resize( BufferSize );
				memcpy(   &m_Bits[0] , pBuffer, BufferSize);

			m_Sync.Unlock();
			m_pWnd->Invalidate(FALSE);
		}		

        return 0;
    }


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//MACRO_EXCEPTION(DirectShowException)


CVideoBackground::CVideoBackground(CWnd* pWnd, const std::wstring& FileName):
	m_pWnd(pWnd), 
	m_Width(0), m_Height(0)
{
	Grab(pWnd, FileName);
}

CVideoBackground::~CVideoBackground()
{
	m_Sync.Lock();
	Stop();
    CComQIPtr< IMediaEvent, &IID_IMediaEvent > pEvent(pGraph );
	
	long code = 0;
    pEvent->WaitForCompletion(INFINITE, &code );
	m_Sync.Unlock();
}


void CVideoBackground::Draw(Graphics& gr, const CRectF& rc) const
{
	if( !m_Bits.empty() )
	{
		Bitmap bmp( & m_bi, (void*)& m_Bits[0] );
		gr.DrawImage( &bmp, rc);
	}
}


HRESULT GetPin(IBaseFilter * pFilter, PIN_DIRECTION dirrequired,  int iNum, IPin **ppPin);
IPin *  GetInPin ( IBaseFilter *pFilter, int Num );
IPin *  GetOutPin( IBaseFilter *pFilter, int Num );


HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
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


IPin * GetInPin( IBaseFilter * pFilter, int nPin )
{
    CComPtr<IPin> pComPin=0;
    GetPin(pFilter, PINDIR_INPUT, nPin, &pComPin);
    return pComPin;
}


IPin * GetOutPin( IBaseFilter * pFilter, int nPin )
{
    CComPtr<IPin> pComPin=0;
    GetPin(pFilter, PINDIR_OUTPUT, nPin, &pComPin);
    return pComPin;
}

///////////////////////////////////////////////

void CVideoBackground::Grab( CWnd * pWnd, const std::wstring& FileName)
{
	HRESULT hr;

	try
	{

    hr = pGrabber.CoCreateInstance( CLSID_SampleGrabber );
    if( !pGrabber )	throw VideoFileException("Can't create sample grabber");


    CComQIPtr< IBaseFilter, &IID_IBaseFilter > pGrabberBase( pGrabber );

    pSource.CoCreateInstance( CLSID_AsyncReader );
    if( !pSource )throw DirectShowException("Can't create async reader");

    pGraph.CoCreateInstance( CLSID_FilterGraph );
    if( !pGraph )throw DirectShowException("Can't create filter graph");

    hr = pGraph->AddFilter( pSource, L"Source" );
    hr = pGraph->AddFilter( pGrabberBase, L"Grabber" );

    CComQIPtr< IFileSourceFilter, &IID_IFileSourceFilter > pLoad( pSource );
    hr = pLoad->Load(  FileName.c_str() , NULL );
    if( FAILED( hr ) )
    {
        if( !pGraph )throw DirectShowException("Can't load video file");
    }
	AM_MEDIA_TYPE GrabType;
	memset( &GrabType, 0, sizeof(AM_MEDIA_TYPE) );
	GrabType.majortype	= MEDIATYPE_Video;
	GrabType.subtype	= MEDIASUBTYPE_RGB24;
    hr = pGrabber->SetMediaType( &GrabType );

    // Get the output pin and the input pin
    //
    CComPtr< IPin > pSourcePin;
    CComPtr< IPin > pGrabPin;

    pSourcePin = GetOutPin( pSource, 0 );
    pGrabPin   = GetInPin( pGrabberBase, 0 );


    hr = pGraph->Connect( pSourcePin, pGrabPin );
    if( FAILED( hr ) )
    {
        throw DirectShowException("Can't connect pins in render graph");
    }

    AM_MEDIA_TYPE mt;
    hr = pGrabber->GetConnectedMediaType( &mt );

    VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) mt.pbFormat;

	CSize Size ( vih->bmiHeader.biWidth, vih->bmiHeader.biHeight );
	SetSize( vih->bmiHeader.biWidth, vih->bmiHeader.biHeight);

    CComPtr <IPin> pGrabOutPin = GetOutPin( pGrabberBase, 0 );
    hr = pGraph->Render( pGrabOutPin );
    if( FAILED( hr ) )
    {
        throw DirectShowException("Can't render video");
    }


    hr = pGrabber->SetBufferSamples( FALSE );
    hr = pGrabber->SetOneShot( FALSE);
    hr = pGrabber->SetCallback( this, 1 );

    CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( pGraph );

	LONGLONG pos = 0;
	pSeeking->SetPositions( &pos, AM_SEEKING_AbsolutePositioning ,NULL, AM_SEEKING_NoPositioning );
    CComQIPtr< IVideoWindow, &IID_IVideoWindow > pWindow = pGraph;
    if (pWindow)
    {
        hr = pWindow->put_AutoShow(OAFALSE);
    }

    CComQIPtr< IMediaEventEx, &IID_IMediaEvent > pEvent( pGraph );

	hr = pEvent->SetNotifyWindow( reinterpret_cast<OAHWND>( pWnd->GetSafeHwnd() ), WM_GRAPHNOTIFY , 0);
   CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(pGraph );
	hr = pControl->Run( );
	}
	catch( DirectShowException)		// release all comptrs
	{
		throw;
	}

}

void		CVideoBackground::Stop()
{
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(pGraph );
	pControl->Stop();

	CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( pGraph );
	LONGLONG pos = 0;
	pSeeking->SetPositions( &pos, AM_SEEKING_AbsolutePositioning ,NULL, AM_SEEKING_NoPositioning );
}

void		CVideoBackground::Pause()
{
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(pGraph );
	pControl->Pause();
}

void		CVideoBackground::	Play()
{
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(pGraph );
	pControl->Run();
}

float			CVideoBackground::GetPos() const
{
	CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( pGraph );
	LONGLONG Current, All;
	pSeeking->GetCurrentPosition( &Current );
	pSeeking->GetDuration(  &All);
	return Current/(float)All;
}

void			CVideoBackground::SetPos(float Pos)
{
	CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( pGraph );
	LONGLONG All;
	pSeeking->GetDuration(  &All);
	LONGLONG pos = LONGLONG(Pos * All);
	pSeeking->SetPositions( &pos, AM_SEEKING_AbsolutePositioning ,NULL, AM_SEEKING_NoPositioning);
	
}

void			CVideoBackground::Replay()
{
    CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( pGraph );

	LONGLONG pos = 0;
	pSeeking->SetPositions( &pos, AM_SEEKING_AbsolutePositioning ,NULL, AM_SEEKING_NoPositioning );

        // activate the threads
    CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(pGraph );
	
    pControl->Run( );	
}

void	CVideoBackground::ProcessMessage()
{
	LONG evCode, evParam1, evParam2;
	HRESULT hr;
	
	CComQIPtr< IMediaEvent, &IID_IMediaEvent > pME(pGraph );
    // Process all queued events
    while(SUCCEEDED(pME->GetEvent(&evCode, (LONG_PTR *) &evParam1,
                    (LONG_PTR *) &evParam2, 0)))
    {
        // Free memory associated with callback, since we're not using it
        hr = pME->FreeEventParams(evCode, evParam1, evParam2);

        // If this is the end of the clip, reset to beginning
        if(EC_COMPLETE == evCode)
        {
            // Reset to first frame of movie
             Replay();
        }
    }	
}

bool	CVideoBackground::IsSeekable()
{
	DWORD caps = AM_SEEKING_CanSeekAbsolute | AM_SEEKING_CanGetDuration; 

	CComQIPtr< IMediaSeeking, &IID_IMediaSeeking > pSeeking( pGraph );
	return (S_OK == pSeeking->CheckCapabilities(&caps));
}

OAFilterState	CVideoBackground::GetState()
{
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl( pGraph );
	OAFilterState fs;
	pControl->GetState(10, &fs);
	return fs;
}

void		CVideoBackground::Stop2()
{
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl( pGraph );
	pControl->Stop();
}