#include "StdAfx.h"
#include ".\videocapcontrol.h"

void VideoCapControlFactory::Create( int nFPS )
{
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;

	HRESULT hr;
	// Create the System Device Enumerator.
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, 
		reinterpret_cast<void**>(&pDevEnum));
	if (SUCCEEDED(hr))
	{
		// Create an enumerator for the video capture category.
		hr = pDevEnum->CreateClassEnumerator(
			CLSID_VideoInputDeviceCategory,
			&pEnum, 0);
		if( hr != S_OK ) 
			throw CreateException(L"No capture devices detected");
	}
	else
	{
		throw CreateException(L"No capture devices detected");
	}

	CComPtr<IMoniker> pMoniker;
	IBaseFilter* pCap;
	while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
	{
		hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pCap);
		m_Cameras.push_back( boost::shared_ptr< VideoCapControl > ( new VideoCapControl( pCap, nFPS ) ) );
		pMoniker.Release();
	}
}


void	VideoCapControlFactory::GetThumbView( size_t nIndex, const BITMAPINFOHEADER* &pBih, const BYTE* &pByte  ) const 
{
	ASSERT( nIndex < m_Cameras.size() );
	return m_Cameras[ nIndex  ]->GetThumbView( pBih, pByte );
}

void	VideoCapControlFactory::StartRecord( size_t nIndex,LPCWSTR szFileName )
{
	ASSERT( nIndex < m_Cameras.size() );
#if 1
	CString s; s.Format( _T("[%Id]--- RECORD->%ws\n"), nIndex, szFileName );
	OutputDebugString( s );
#endif
	m_Cameras[ nIndex  ]->StartRecord( szFileName );
}

void	VideoCapControlFactory::StopRecord(size_t nIndex)
{
	ASSERT( nIndex < m_Cameras.size() );
#if 1
	CString s; s.Format( _T("[%Id]+++ STOP\n"), nIndex );
	OutputDebugString( s );
#endif
	return m_Cameras[ nIndex  ]->StopRecord( );
}

void	VideoCapControlFactory::Destroy( )
{
	m_Cameras.clear();
}

//////////////////////////////////  

HRESULT VideoCapControl::AddToRot(IUnknown *pUnkGraph) 
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


void VideoCapControl::RemoveFromRot()
{
	IRunningObjectTable *pROT;
	if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
		pROT->Revoke(m_dwRegister);
		pROT->Release();
	}
}

///////////////////////////////////

#ifndef CheckPointer
#define CheckPointer(p,ret) {if((p)==NULL) return (ret);}
#endif

bool VideoCapControl::CreateFilter(IBaseFilter **ppFilter, LPCWSTR stFilterMoniker)
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

VideoCapControl::VideoCapControl(IBaseFilter* pCap, int nFPS)
{
//	const int c_FPS = 10;
	Create( );
	m_pCapFilter.Attach( pCap );  
	HRESULT hr;
	hr = m_pBuilder->AddFilter(pCap, L"Capture Filter");
	if( FAILED(hr))	throw CreateException(L"Failed to the add capture filter to the graph");

	IAMStreamConfig *pConfig = NULL;
	hr = m_pGraph->FindInterface(
		&PIN_CATEGORY_CAPTURE,	// Preview pin.
		&MEDIATYPE_Video,		// Any media type.
		m_pCapFilter,			// Pointer to the capture filter.
		IID_IAMStreamConfig, (void**)&pConfig);
	if( FAILED( hr ) )	throw CreateException(L"Can't locate IAMStreamConfig");
	AM_MEDIA_TYPE mt;
	memset( &mt, 0, sizeof(AM_MEDIA_TYPE) );
#if 0
	mt.bFixedSizeSamples		= TRUE;
	mt.bTemporalCompression		= FALSE;
#endif
	mt.pbFormat = (BYTE*)CoTaskMemAlloc( sizeof(VIDEOINFOHEADER) );
	mt.cbFormat = sizeof (VIDEOINFOHEADER);
	mt.majortype	= MEDIATYPE_Video;
	mt.subtype		= MEDIASUBTYPE_YUY2;
	mt.formattype	= FORMAT_VideoInfo;
	VIDEOINFOHEADER	*pVih = (VIDEOINFOHEADER*)mt.pbFormat;
	memset( pVih, 0, sizeof(VIDEOINFOHEADER) );
	pVih->AvgTimePerFrame = nFPS <= 0 ? 0 : (1000 * 1000 * 10)/nFPS;
	BITMAPINFOHEADER	&bih = pVih->bmiHeader;
	bih.biWidth		= 720;
	bih.biHeight	= 576;
	bih.biSize		= sizeof(BITMAPINFOHEADER);
#if 0
	bih.biBitCount	= 16;
	bih.biSizeImage	= DIBSIZE(bih);
	bih.biCompression = mmioFOURCC( 'Y', 'U', 'Y', '2' );
	bih.biPlanes	= 1;
	mt.lSampleSize	= bih.biSizeImage;
	pVih->dwBitRate	= 8 * bih.biSizeImage * c_FPS;
#endif
	hr = pConfig->SetFormat( &mt );
	CoTaskMemFree( mt.pbFormat);
	if( FAILED( hr ) )
	{
		pConfig->Release();
		throw CreateException(L"Failed to set capture format");
	}

#if 0
	AM_MEDIA_TYPE* pmt;
	hr = pConfig->GetFormat( &pmt );
	pVih = (VIDEOINFOHEADER*)pmt->pbFormat;
#endif
	pConfig->Release();
#if 1
	CreateThumbView();
#endif

	IBaseFilter* pY800Encoder;
	if(CreateFilter(&pY800Encoder, TEXT("@device:cm:{33D9A760-90C8-11D0-BD43-00A0C911CE86}\\y800")))
	{
		m_pBuilder->AddFilter(pY800Encoder, L"Y800 Encoder");
		m_pEncodeFilter.Attach(pY800Encoder);
	}
	else
	{
		throw CreateException(L"Y800 video codec is not installed" );
	}

	IBaseFilter *pMux;
	IFileSinkFilter* pFi;
	hr = m_pGraph->SetOutputFileName(
		&MEDIASUBTYPE_Avi,  // Specifies AVI for the target file.
		L"123",			// File name.
		&pMux,              // Receives a pointer to the mux.
		&pFi);              // (Optional) Receives a pointer to the file sink.
	m_pFileInt.Attach( pFi ) ;
	CString s;
	if( FAILED(hr) )
	{
		s.Format( _T("Failed to set output file name") );
		throw RecordException( (LPCWSTR)s );
	}

	hr = m_pGraph->RenderStream( 
		&PIN_CATEGORY_CAPTURE, // Pin category.
		&MEDIATYPE_Video,      // Media type.
		m_pCapFilter,
#if 1
		m_pEncodeFilter,
#else
		NULL,
#endif
		pMux);
	pMux->Release();

	if( FAILED(hr) )
	{
		s.Format( _T("Failed to create record graph") );
		throw RecordException( (LPCWSTR)s );
	}
}

VideoCapControl::~VideoCapControl(void)
{
	RemoveFromRot( );
}

void	VideoCapControl::Create( )
try
{
	HRESULT hr;

	hr = m_pBuilder.CoCreateInstance( CLSID_FilterGraph );
	if( FAILED(hr) || !m_pBuilder )throw CreateException(L"Can't create filter graph");

	hr = m_pGraph.CoCreateInstance( CLSID_CaptureGraphBuilder2 );
	if( FAILED(hr) || !m_pGraph )throw CreateException(L"Can't create capture filter graph");

	hr = m_pGraph->SetFiltergraph( m_pBuilder );
	if(FAILED(hr)) throw CreateException(L"Can't set IGraphBuilder for capture");
	AddToRot( m_pBuilder );


}
catch( DShowException )
{
	throw;
};

void	VideoCapControl::CreateThumbView()
{
	HRESULT hr;
	
	CComPtr< IBaseFilter>		pRender;
#if 1
	CComPtr< ISampleGrabber>	pGrabber;

	hr = pGrabber.CoCreateInstance( CLSID_SampleGrabber );
	if( !pGrabber || FAILED(hr) )	throw CreateException(L"Can't create sample grabber");
	CComQIPtr< IBaseFilter, &IID_IBaseFilter > pGrabberBase( pGrabber );
	
	hr = m_pBuilder->AddFilter( pGrabberBase, L"Grab filter" );
	if( FAILED(hr) ) throw CreateException(L"Can't add grabber filter");

	AM_MEDIA_TYPE GrabType;
	memset( &GrabType, 0, sizeof(AM_MEDIA_TYPE) );
	GrabType.majortype	= MEDIATYPE_Video;
	GrabType.formattype = FORMAT_VideoInfo;
	GrabType.subtype	= MEDIASUBTYPE_RGB24; 

	hr = pGrabber->SetMediaType( &GrabType );
	if( FAILED(hr) ) throw CreateException(L"Can't set input media type");
#endif
	hr = pRender.CoCreateInstance( CLSID_VideoRendererDefault);
	if( FAILED( hr ) )	throw CreateException(L"Can't create render");

	hr = m_pBuilder->AddFilter( pRender, L"Render filter" );
	if( FAILED(hr) ) throw CreateException(L"Can't add render filter");

	hr = m_pGraph->RenderStream( 
		&PIN_CATEGORY_PREVIEW, // Pin category.
		&MEDIATYPE_Video,      // Media type.
		m_pCapFilter,
		pGrabberBase,
		pRender);
	if( FAILED( hr ) )	throw CreateException(L"Can't render stream");
#if 1
	AM_MEDIA_TYPE mt;
	hr = pGrabber->GetConnectedMediaType( &mt );

	if( mt.majortype	!= MEDIATYPE_Video		||
		mt.formattype	!= FORMAT_VideoInfo		||
		mt.cbFormat < sizeof(VIDEOINFOHEADER)	)
		throw CreateException(L"Invalid output format of the sample grabber");

	VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) mt.pbFormat;
	memcpy( &m_bih, &vih->bmiHeader, sizeof( BITMAPINFOHEADER ) );
	if( m_bih.biBitCount != 24			 || 
		m_bih.biCompression != BI_RGB  ) 
			throw CreateException(L"24-bit image is not supported");
	CoTaskMemFree( mt.pbFormat );
#endif
	CComQIPtr< IVideoWindow, &IID_IVideoWindow > pWindow (pRender);
	if (pWindow)
	{
		hr = pWindow->put_AutoShow(OAFALSE);
	}
	pWindow.Release();
#if 1
	hr = pGrabber->SetBufferSamples( TRUE );
	hr = pGrabber->SetOneShot( TRUE);
#endif
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(m_pBuilder );
//	hr = AddToRot(m_pGraph);
	hr = pControl->Run( );

	OAFilterState state;
	do
	{
		pControl->GetState(0, &state);
		Sleep(10);
	} 
	while (state != State_Running);
	
#if 1
	long	BufferSize;

	do
	{
	hr = pGrabber->GetCurrentBuffer(  &BufferSize, 0);
	} 
	while ( hr != S_OK );
	if( FAILED(hr) ) throw CreateException(L"Invalid ISampleGrabber buffer");
	m_Image.resize( BufferSize );
	hr = pGrabber->GetCurrentBuffer( &BufferSize, (long*)&m_Image[0] );
	if( FAILED(hr) ) throw CreateException(L"Failed to get content of the buffer");
#endif	
	hr = pControl->Stop();
	do
	{
		pControl->GetState(0, &state);
		Sleep(10);
	} 
	while (state != State_Stopped);

	m_pBuilder->RemoveFilter( pGrabberBase );
	m_pBuilder->RemoveFilter( pRender );
}

void	VideoCapControl::GetThumbView( const BITMAPINFOHEADER* &pBih, const BYTE* &pByte  ) const 
{
	pBih	= &m_bih;
	pByte	= &m_Image[0];
}

void	VideoCapControl::StartRecord( LPCWSTR szFileName )
{
	HRESULT hr;

	hr = m_pFileInt->SetFileName( szFileName, 0 );
	if( FAILED(hr) )
	{
		CString s;
		s.Format( _T("Failed to set output file name for %ws"), szFileName );
		throw RecordException( (LPCWSTR)s );
	}

	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(m_pBuilder );
	hr = pControl->Run( );
	OAFilterState state;
	do
	{
		pControl->GetState(0, &state);
		Sleep(10);
	} 
	while (state != State_Running);
	if( FAILED(hr) ) throw RecordException( L"Failed to start record graph" );
}

void	VideoCapControl::StopRecord()
{
	HRESULT hr;
	CComQIPtr< IMediaControl, &IID_IMediaControl > pControl(m_pBuilder );
	hr = pControl->Stop( );
	OAFilterState state;
	do
	{
		pControl->GetState(0, &state);
		Sleep(10);
	} 
	while (state != State_Stopped);
	if( FAILED(hr) ) throw RecordException( L"Failed to disconnect record graph" );
}
