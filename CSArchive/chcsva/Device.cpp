// Device.cpp: implementation of the CDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "chcs.h"
#include "Device.h"

#define NO_DSHOW_STRSAFE
#include <dshow.h>
#include <qedit.h>

//////////////////////////////////////////////////////////////////////////
// Part of Baseclasses

const LONGLONG MAX_TIME = 0x7FFFFFFFFFFFFFFF;   /* Maximum LONGLONG value */

#define CheckPointer(p,ret) {if((p)==NULL) return (ret);}

LPCTSTR GetSubtypeName(const GUID *pSubtype)
{
	struct SubtypesNames
	{
		const GUID *pSubtype;
		LPCTSTR	stName;
	};

	SubtypesNames SubtypesMap[] =
	{
		{ &MEDIASUBTYPE_RGB1, TEXT("RGB Monochrome") },
		{ &MEDIASUBTYPE_RGB4, TEXT("RGB VGA") },
		{ &MEDIASUBTYPE_RGB8, TEXT("RGB 8") },
		{ &MEDIASUBTYPE_RGB565, TEXT("RGB 565 (16 bit)") },
		{ &MEDIASUBTYPE_RGB555, TEXT("RGB 555 (16 bit)") },
		{ &MEDIASUBTYPE_RGB24, TEXT("RGB 24")  },
		{ &MEDIASUBTYPE_RGB32, TEXT("RGB 32")  },
		{ &MEDIASUBTYPE_ARGB32, TEXT("ARGB 32") },
		{ &MEDIASUBTYPE_Overlay, TEXT("Overlay") }
	};

	for(int i = 0; i < countof(SubtypesMap); i++)
		if(!memcmp(SubtypesMap[i].pSubtype, pSubtype, sizeof(GUID)))
			return SubtypesMap[i].stName;

	static TCHAR stSubtypeName[8];

	stSubtypeName[0] = (TCHAR)( pSubtype->Data1 & 0x000000FF);
	stSubtypeName[1] = (TCHAR)((pSubtype->Data1 & 0x0000FF00) >> 0x08);
	stSubtypeName[2] = (TCHAR)((pSubtype->Data1 & 0x00FF0000) >> 0x10);
	stSubtypeName[3] = (TCHAR)((pSubtype->Data1 & 0xFF000000) >> 0x18);
	stSubtypeName[4] = 0;

	if(!_istprint(stSubtypeName[0])) stSubtypeName[0] = TEXT('?');
	if(!_istprint(stSubtypeName[1])) stSubtypeName[1] = TEXT('?');
	if(!_istprint(stSubtypeName[2])) stSubtypeName[2] = TEXT('?');
	if(!_istprint(stSubtypeName[3])) stSubtypeName[3] = TEXT('?');

	return stSubtypeName;
}

void WINAPI FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);

		// Strictly unnecessary but tidier
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	
	if (mt.pUnk != NULL)
	{
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

void WINAPI DeleteMediaType(AM_MEDIA_TYPE *pmt)
{
	if(pmt)
	{
		FreeMediaType(*pmt);
		CoTaskMemFree((PVOID)pmt);
	}
}

//////////////////////////////////////////////////////////////////////////

#define REGISTER_FILTERGRAPH

//////////////////////////////////////////////////////////////////////
// CDevice
//////////////////////////////////////////////////////////////////////

CDevice::CDevice()
{
	m_pXbar = NULL;
	m_pVSC = NULL;
	m_pVPA = NULL;
	m_pVDrp = NULL;
	m_pVdoDec = NULL;

	m_pMC = NULL;
	m_pME = NULL;
	m_pMS = NULL;
	m_pGraph = NULL;
	m_pCapture = NULL;

	m_lDroppedNotBase = 0;
	m_lDroppedBase = 0;

	m_bValid = false;
	m_bHasStreamControl = false;

	m_dwGraphRegister = 0;
}

CDevice::~CDevice()
{
	CloseInterfaces();
}

void CDevice::CloseInterfaces()
{
	// Stop receiving data
    if(m_pMC)
		m_pMC->Stop();

	// Disable event notification before releasing the graph.
	if(m_pME)
		m_pME->SetNotifyWindow(NULL, 0, 0);

#ifdef REGISTER_FILTERGRAPH
	if(m_dwGraphRegister)
	{
		RemoveGraphFromRot(m_dwGraphRegister);
		m_dwGraphRegister = 0;
	}
#endif

	HandleDeviceEvents();

	SAFE_RELEASE(m_pXbar);
	SAFE_RELEASE(m_pVSC);
	SAFE_RELEASE(m_pVPA);
	SAFE_RELEASE(m_pVdoDec);
	SAFE_RELEASE(m_pVDrp);

	// Release DirectShow interfaces
	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pME);
	SAFE_RELEASE(m_pCapture);
	
	try
	{
		SAFE_RELEASE(m_pGraph);
	}
	catch(...)
	{
		// Don't know why...
		// mostly when REGISTER_FILTERGRAPH enabled

		Elvees::Output(Elvees::TError, TEXT("IGraphBuilder::Release() cause an exception"));
	}
}

//////////////////////////////////////////////////////////////////////
// Creating group
//////////////////////////////////////////////////////////////////////

bool CDevice::CreateBaseInterfaces()
{
	HRESULT hr = E_FAIL;

	do
	{
		// Create the filter graph
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
			IID_IGraphBuilder, (void**)&m_pGraph);
		if(FAILED(hr)) break;

		// Create the capture graph builder
		hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
			IID_ICaptureGraphBuilder2, (void**)&m_pCapture);
		if(FAILED(hr)) break;

		// Attach the filter graph to the capture graph
		hr = m_pCapture->SetFiltergraph(m_pGraph);
		if(FAILED(hr)) break;

		// Obtain interfaces for media control
		hr = m_pGraph->QueryInterface(IID_IMediaControl, (void**)&m_pMC);
		if(FAILED(hr)) break;

		hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void**)&m_pME);
		if(FAILED(hr)) break;

		hr = m_pGraph->QueryInterface(IID_IMediaSeeking, (void**)&m_pMS);
		if(FAILED(hr)) break;

		return true;
	}
	while(FALSE);

	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pME);
	SAFE_RELEASE(m_pMS);
	SAFE_RELEASE(m_pGraph);
	SAFE_RELEASE(m_pCapture);

	return false;
}

bool CDevice::CreateFilter(IBaseFilter **ppFilter, LPCTSTR stFilterMoniker)
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

bool CDevice::CreateSourceBaseInterfaces(IBaseFilter *pSource)
{
	CheckPointer(pSource, false);
	CheckPointer(m_pCapture, false);

	HRESULT hr;

	// IAMAnalogVideoDecoder
	hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, pSource,
				IID_IAMAnalogVideoDecoder, (void **)&m_pVdoDec);

	if(FAILED(hr))
		m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pSource,
				IID_IAMAnalogVideoDecoder, (void **)&m_pVdoDec);

	// IAMStreamConfig
    hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, pSource,
				IID_IAMStreamConfig, (void **)&m_pVSC);

	if(FAILED(hr))
		m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pSource,
				IID_IAMStreamConfig, (void **)&m_pVSC);

	// IAMVideoProcAmp
    hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, pSource,
				IID_IAMVideoProcAmp, (void **)&m_pVPA);

	if(FAILED(hr))
		m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pSource,
				IID_IAMVideoProcAmp, (void **)&m_pVPA);

	// IAMCrossbar
    hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, pSource,
				IID_IAMCrossbar, (void **)&m_pXbar);

	if(FAILED(hr))
		m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pSource,
				IID_IAMCrossbar, (void **)&m_pXbar);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////

// Adds a DirectShow filter graph to the Running Object Table,
// allowing GraphEdit to "spy" on a remote filter graph.

HRESULT CDevice::AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    WCHAR wsz[128];
    HRESULT hr;

    if(!pUnkGraph || !pdwRegister)
        return E_POINTER;

    if(FAILED(GetRunningObjectTable(0, &pROT)))
        return E_FAIL;

    swprintf(wsz, L"FilterGraph %08lX pid %08lX\0",
		(DWORD_PTR)pUnkGraph, GetCurrentProcessId());

    hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if(SUCCEEDED(hr))
    {
        // Use the ROTFLAGS_REGISTRATIONKEEPSALIVE to ensure a strong reference
        // to the object.  Using this flag will cause the object to remain
        // registered until it is explicitly revoked with the Revoke() method.
        //
        // Not using this flag means that if GraphEdit remotely connects
        // to this graph and then GraphEdit exits, this object registration 
        // will be deleted, causing future attempts by GraphEdit to fail until
        // this application is restarted or until the graph is registered again.
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, 
                            pMoniker, pdwRegister);
        pMoniker->Release();
    }

    pROT->Release();
    return hr;
}

// Removes a filter graph from the Running Object Table
void CDevice::RemoveGraphFromRot(DWORD pdwRegister)
{
    IRunningObjectTable *pROT;

    if(SUCCEEDED(GetRunningObjectTable(0, &pROT)))
    {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}

IPin* CDevice::GetPin(IBaseFilter* pFilter, int iNum, bool input)
{
	HRESULT hr;
	IPin      *pPin  = NULL;
    IEnumPins *pEnum = NULL;

	CheckPointer(pFilter, NULL);
    
    hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr))
        return NULL;

	CheckPointer(pEnum, NULL);

	ULONG ulFound;
    
    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        PIN_DIRECTION pindir = (PIN_DIRECTION)3;

		if(pPin)
		{
			pPin->QueryDirection(&pindir);
        
			if((input && pindir == PINDIR_INPUT) || (!input && pindir == PINDIR_OUTPUT))
			{
				if(iNum == 0)
				{
					pEnum->Release();
					return pPin;
				}

				iNum--;
			}

			pPin->Release();
		}
    }

	pEnum->Release();
    return NULL;
}

//////////////////////////////////////////////////////////////////////
// CSampleGrabberHelper callback
//////////////////////////////////////////////////////////////////////

void CDevice::OnBufferCB(double fTime, BYTE *pData, long lLen)
{
	Elvees::Output(Elvees::TWarning, TEXT("Unhandled CDevice::OnBufferCB()"));
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

bool CDevice::SetEventHWND(HWND hWnd, long lMsg)
{
	if(m_pME)
	{
		HRESULT hr;

		hr = m_pME->SetNotifyWindow((OAHWND)hWnd, lMsg, (LONG_PTR)this);
		if(SUCCEEDED(hr))
			return true;
	}

	return false;
}

void CDevice::HandleDeviceEvents()
{
	if(IsBadReadPtr(this, sizeof(CDevice)))
		return;

	if(m_pME)
	{
		long evCode, evParam1, evParam2;
				
		while(SUCCEEDED(m_pME->GetEvent(&evCode, &evParam1, &evParam2, 0)))
		{
			m_pME->FreeEventParams(evCode, evParam1, evParam2);

			if((EC_COMPLETE == evCode) || (EC_USERABORT == evCode) || (EC_ERRORABORT == evCode))
				StopDevice();

			DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("D[%ld] Events (0x%X)"),
				GetDeviceID(), evCode));
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

long CDevice::IsSignalLocked()
{
	CheckPointer(m_pVdoDec, -1);

	long lSignal;

	if(m_pVdoDec->get_HorizontalLocked(&lSignal) != S_OK)
		lSignal = -1;

	return lSignal;
}

//////////////////////////////////////////////////////////////////////////
// Create capture graph for device
//////////////////////////////////////////////////////////////////////////

bool CDevice::InitDevice(LPCTSTR stMoniker,
				  long lWidth, long lHeight, long lFrmRate,
				  long lInput, long lVStandard, bool bBWMode)
{
	CheckPointer(stMoniker, false);

	// Check whether it VfW filter 
	if(_tcsncmp(stMoniker, TEXT("@device:cm:"), 11) == 0)
	{
		Elvees::Output(Elvees::TError, TEXT("VfW capture not supported"));
		return false;
	}

	// if dimensions less or equal to 0 then autodetect
	lWidth   = max(0, lWidth);
	lHeight  = max(0, lHeight);

	// if fps less or equal to 0 then format specified fps used
	lFrmRate = max(0, lFrmRate);

	DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("Device (%ld x %ld) FPS: %ld I: %ld VS: %ld CE: %s \"%s\""),
		lWidth, lHeight, lFrmRate,
		lInput, lVStandard, bBWMode ? TEXT("No") : TEXT("Yes"),
		stMoniker));

	// Check input dimensions
	if(lVStandard < 0)
		lVStandard = AnalogVideo_PAL_B;

	HRESULT hr;
	IBaseFilter *pCaptureSource = NULL;
	IBaseFilter *pNullRenderer  = NULL;
	IBaseFilter *pSampleGrabber = NULL;
	ISampleGrabber *pGrab = NULL;

	m_bValid = false;
	m_dwGraphRegister = 0;

	// Close previous filtergraph if any..
	CloseInterfaces();

	do
	{
		if(!CreateBaseInterfaces())
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to create base interfaces"));
			break;
		}

		if(Elvees::IsFileExists(stMoniker))
		{
		#ifdef _UNICODE
			hr = m_pGraph->AddSourceFilter(stMoniker, L"CaptureSource", &pCaptureSource);
		#else
			WCHAR szMoniker[4096];

			MultiByteToWideChar(CP_ACP, 0, stMoniker, lstrlen(stMoniker) + 1,
					szMoniker, countof(szMoniker));

			hr = m_pGraph->AddSourceFilter(szMoniker, L"CaptureSource", &pCaptureSource);
		#endif
		}
		else 
		{
			if(!CreateFilter(&pCaptureSource, stMoniker))
			{
				Elvees::Output(Elvees::TError, TEXT("Fail to create source filter"));
				break;
			}

			hr = m_pGraph->AddFilter(pCaptureSource, L"CaptureSource");
		}

		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to add source to graph hr=0x%08lX"), hr);
			break;
		}

		if(!CreateSourceBaseInterfaces(pCaptureSource))
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to create source base interfaces"));
			break;
		}

		if(!RouteToSignaledPin(lInput))
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to route crossbar"));
			break;
		}

		if(!SetTVFormat(lVStandard))
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to set tv format"));
			break;
		}

		if(!SetCaptureFormat(lWidth, lHeight, lFrmRate))
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to set capture format"));
			break;
		}

		if(!SetVideoProc(bBWMode))
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to set video processing"));
			break;
		}

		// Create SampleGrabber
		//

		hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC,
			IID_IBaseFilter, (void **)&pSampleGrabber);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to create samplegrabber hr=0x%08lX"), hr);
			break;
		}

		hr = m_pGraph->AddFilter(pSampleGrabber, L"SampleGrabber");
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to add samplegrabber to graph hr=0x%08lX"), hr);
			break;
		}

		// Create NullRender
		//

		hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC,
			IID_IBaseFilter, (void **)&pNullRenderer);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to create nullrender hr=0x%08lX"), hr);
			break;
		}

		hr = m_pGraph->AddFilter(pNullRenderer, L"NullRenderer");
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to add nullrender to graph hr=0x%08lX"), hr);
			break;
		}

		// Setting capture data
		//

		hr = pSampleGrabber->QueryInterface(IID_ISampleGrabber, (void **)&pGrab);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to get samplegrabber interface hr=0x%08lX"), hr);
			break;
		}

		AM_MEDIA_TYPE mt;
		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));

		mt.majortype  = MEDIATYPE_Video;
		mt.subtype    = MEDIASUBTYPE_YUY2;
		mt.formattype = FORMAT_VideoInfo;

	#if 0
		if(!bSupportFormat)
		{
			AM_MEDIA_TYPE* pmtConfig = NULL;
			hr = m_pVSC->GetFormat(&pmtConfig);
			if(SUCCEEDED(hr))
			{
				mt.subtype = pmtConfig->subtype;
				DeleteMediaType(pmtConfig);
			}

			// Add converter to graph
			IBaseFilter *pYUY2Encoder = NULL;

			if(CreateFilter(&pYUY2Encoder, TEXT("@device:cm:{33D9A760-90C8-11D0-BD43-00A0C911CE86}\\hfyu")))
			{
				m_pGraph->AddFilter(pYUY2Encoder, L"YUY2 Encoder");
				pYUY2Encoder->Release();
			}
		}
	#endif

		hr = pGrab->SetMediaType(&mt);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to set capture mediatype hr=0x%08lX"), hr);
			break;
		}

		if(!SetCallback(pGrab))
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to set callback interface"));
			break;
		}

	#ifdef REGISTER_FILTERGRAPH
		hr = AddGraphToRot(m_pGraph, &m_dwGraphRegister);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Add graph to ROT failed hr=0x%08lX"), hr);
			m_dwGraphRegister = 0;
		}
	#endif

		// Connect Pins
		//

	#if 0
		IPin* grabIn = GetPin(pSampleGrabber, 0, true);
		IPin* srcOut = NULL;

		hr = m_pCapture->FindPin(pCaptureSource, PINDIR_OUTPUT, &PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Interleaved, TRUE, 0, &srcOut);

		if(FAILED(hr))
			m_pCapture->FindPin(pCaptureSource, PINDIR_OUTPUT, &PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, TRUE, 0, &srcOut);

		hr = m_pGraph->Connect(srcOut, grabIn);

		SAFE_RELEASE(srcOut);
		SAFE_RELEASE(grabIn);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to render source pin hr=0x%X"), hr);
			break;
		}

		IPin* grabOut = GetPin(pSampleGrabber, 0, false);
		IPin* rendIn = GetPin(pNullRenderer, 0, true);

		hr = m_pGraph->Connect(grabOut, rendIn);

		SAFE_RELEASE(grabOut);
		SAFE_RELEASE(rendIn);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to render grabber hr=0x%08lX"), hr);
			break;
		}
	#else
		// use intelligent connect
		hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
				pCaptureSource,	pSampleGrabber, pNullRenderer);

		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TWarning, TEXT("Fail to render Capture-Video pin hr=0x%08lX"), hr);

			hr = m_pCapture->RenderStream(NULL, NULL, pCaptureSource,
				pSampleGrabber, pNullRenderer);
		}

		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to render graph hr=0x%08lX"), hr);
			break;
		}
	#endif

		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
		hr = pGrab->GetConnectedMediaType(&mt);
		if(SUCCEEDED(hr))
		{
			if(!OnImageFormat(HEADER(mt.pbFormat)))
			{
				FreeMediaType(mt);

				Elvees::Output(Elvees::TError, TEXT("Image format not accepted"));
				break;
			}

			FreeMediaType(mt);
		}
		else
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to get captured media type"));
			break;
		}

		if(!PrepareStartCapture(pCaptureSource))
		{
			Elvees::Output(Elvees::TError, TEXT("Fail to pre-start graph"));
			break;
		}

		m_bValid = true;
	}
	while(FALSE);

	SAFE_RELEASE(pCaptureSource);
	SAFE_RELEASE(pSampleGrabber);
	SAFE_RELEASE(pNullRenderer);
	SAFE_RELEASE(pGrab);

#ifndef _DEBUG
	if(!m_bValid)
		CloseInterfaces();
#endif

	return m_bValid;
}

bool CDevice::IsDeviceValid()
{
	return m_bValid;
}

//////////////////////////////////////////////////////////////////////
// Run/ControlStream
//////////////////////////////////////////////////////////////////////

bool CDevice::PrepareStartCapture(IBaseFilter *pSource)
{
	CheckPointer(pSource, false);		// IBaseFilter capture source
	CheckPointer(m_pMC, false);			// IMediaControl
	CheckPointer(m_pCapture, false);	// ICaptureGraphBuilder2

	HRESULT hr;
	REFERENCE_TIME rfTimeStart = MAX_TIME;

#if 0
	// don't capture quite yet...
	hr = m_pCapture->ControlStream(&PIN_CATEGORY_CAPTURE, NULL, NULL,
		&rfTimeStart, NULL, 0, 0);

	m_bHasStreamControl = SUCCEEDED(hr);
#else
	m_bHasStreamControl = false;
#endif

	hr = m_bHasStreamControl ? m_pMC->Run() : m_pMC->Pause();
	if(FAILED(hr))
	{
		m_pMC->Stop();
		return false;
	}

	if(m_bHasStreamControl)
	{
		hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, pSource,
			IID_IAMDroppedFrames, (void **)&m_pVDrp);

		if(FAILED(hr))
			m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pSource,
			IID_IAMDroppedFrames, (void **)&m_pVDrp);
	}

	return true;
}

bool CDevice::StartDevice()
{
	CheckPointer(m_pMC, false);			// IMediaControl
	CheckPointer(m_pCapture, false);	// ICaptureGraphBuilder2

	HRESULT hr;
	REFERENCE_TIME rfTimeStop = MAX_TIME;

	m_lDroppedNotBase = 0;
	m_lDroppedBase = 0;

	hr = m_bHasStreamControl ? m_pMC->Run() : m_pMC->Pause();
	if(FAILED(hr))
	{
		m_pMC->Stop();
		return false;
	}

	if(m_bHasStreamControl)
	{
		// turn the capture pin on now!
		hr = m_pCapture->ControlStream(&PIN_CATEGORY_CAPTURE, NULL,	NULL,
			NULL, &rfTimeStop, 0, 0);
		if(FAILED(hr))
			return false;

		// make note of the current dropped frame counts
		if(m_pVDrp)
		{
			m_pVDrp->GetNumDropped(&m_lDroppedBase);
			m_pVDrp->GetNumNotDropped(&m_lDroppedNotBase);
		}
	}
	else
	{
		if(m_pMS)
		{
			REFERENCE_TIME rtStart = 0, rtStop = MAX_TIME;

			hr = m_pMS->SetPositions(
				&rtStart, AM_SEEKING_AbsolutePositioning, 
				&rtStop, AM_SEEKING_AbsolutePositioning);

		}

		hr = m_pMC->Run();
		if(FAILED(hr))
		{
			// stop parts that started
			m_pMC->Stop();
			return false;
		}
	}

	return true;
}

void CDevice::StopDevice()
{
	if(m_pMC)
	{
		HRESULT hr;
		hr = m_pMC->Stop();

		if(FAILED(hr))
			Elvees::OutputF(Elvees::TError, TEXT("Stop failed hr=0x%X"), hr);
	}
}

bool CDevice::IsDeviceStarted(long msTimeout)
{
	if(!m_pMC)
		return false;

	HRESULT hr;
	OAFilterState state;
	
	hr = m_pMC->GetState(msTimeout, &state);

	//	S_OK Success. 
	//	VFW_S_STATE_INTERMEDIATE The filter graph is still in
	//		transition to the indicated state. 
	//	VFW_S_CANT_CUE The filter graph is paused, but cannot cue data. 
	//	E_FAIL Failure. 

	if(hr == S_OK && state == State_Running)
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

bool CDevice::OnImageFormat(BITMAPINFOHEADER *pbihImg)
{
	if(pbihImg && pbihImg->biCompression == mmioFOURCC('Y','U','Y','2'))
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
// Dropped frames counter
//////////////////////////////////////////////////////////////////////////

long CDevice::GetNumDropped()
{
	long lDropped = 0;

	if(m_pVDrp)
		m_pVDrp->GetNumDropped(&lDropped);

	lDropped -= m_lDroppedBase;

	return lDropped;
}

long CDevice::GetNumNotDropped()
{
	long lDroppedNot = 0;

	if(m_pVDrp)
		m_pVDrp->GetNumNotDropped(&lDroppedNot);

	lDroppedNot -= m_lDroppedNotBase;
	
	if(lDroppedNot < 0)
		lDroppedNot = 0;

	return lDroppedNot;
}

void CDevice::DropFrame()
{
	m_lDroppedBase--;
	m_lDroppedNotBase++;
}

//////////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////////

inline bool CDevice::RouteToSignaledPin(long lPinDesired)
{
	if(!m_pXbar)
	{
		Elvees::Output(Elvees::TInfo, TEXT("No crossbar interface"));
		return true;
	}

	HRESULT hr;
	long lSignal, lPin, lInputPin, lSignaledPin;
	long lXBarOutputCnt, lXBarInputCnt;

	lInputPin = lPinDesired;
	
	hr = m_pXbar->get_PinCounts(&lXBarOutputCnt, &lXBarInputCnt);
	if(FAILED(hr))
	{
		Elvees::OutputF(Elvees::TError, TEXT("Fail to get pin counts hr=0x%08lX"), hr);
		return false;
	}
	
	if(lInputPin >= lXBarInputCnt)
	{
		Elvees::OutputF(Elvees::TWarning, TEXT("Video input=%ld not in range [0, %ld]"), lInputPin, lXBarInputCnt);
		lInputPin   = -1;
		lPinDesired = -1;
	}

	if(lXBarOutputCnt > 0)
	{
		for(lPin = 0, lSignaledPin = -1; lPin < lXBarInputCnt; lPin++)
		{
			hr = m_pXbar->Route(0, lPin); // 0 - Video out pin
			if(FAILED(hr))
			{
				Elvees::OutputF(Elvees::TError, TEXT("Route to pin[%d] failed"), lPin);
				continue;
			}

			Sleep(200); // ~5 frames, time to lock synchronization
			lSignal = IsSignalLocked();

			if(!lSignal)
			{
				if(lPin == lPinDesired)
				{
					DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("No signal on desired pin[%d]"), lPin));
					lInputPin = -1;
				}
			}
			else if(lSignal > 0)
			{
				DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("Signal on pin[%d] detected"), lPin));
				lSignaledPin = lPin;
			}
		}

		if(lSignaledPin < 0)
		{
			Elvees::Output(Elvees::TWarning, TEXT("No signal detected, please plug-in camera"));
			lInputPin = max(0, lPinDesired);
		}
		else if(lInputPin < 0)
		{
			lInputPin = lSignaledPin;
		}
		
		m_pXbar->Route(0, lInputPin);
		DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("Using pin[%d] to capture video"), lInputPin));
	}

	return true;
}

inline bool CDevice::SetTVFormat(long lVStandard)
{
	if(!m_pVdoDec)
	{
		Elvees::Output(Elvees::TInfo, TEXT("No analog video decoder interface"));
		return true;
	}

	if(lVStandard == AnalogVideo_None)
		lVStandard = AnalogVideo_PAL_B;

	HRESULT hr;
	long lAvailableTVFormats = 0;

	hr = m_pVdoDec->get_AvailableTVFormats(&lAvailableTVFormats);
	if(SUCCEEDED(hr))
	{
		if(lVStandard & lAvailableTVFormats)
		{
			hr = m_pVdoDec->put_TVFormat(lVStandard);
			if(SUCCEEDED(hr))
				return true;

			Elvees::OutputF(Elvees::TError, TEXT("Function put_TVFormat(%lX) return 0x%08lX"), lVStandard, hr);
		}
		else
		{
			Elvees::OutputF(Elvees::TError, TEXT("Unsupported video standard %lX(%lX)"), lVStandard, lAvailableTVFormats);
		}
	}
	else
	{
		Elvees::OutputF(Elvees::TError, TEXT("Can't get available TVFormats hr=0x%08lX"), hr);
	}

	return false;
}

inline bool CDevice::SetCaptureFormat(long lWidth, long lHeight, long lFrmRate)
{
	if(!m_pVSC)
	{
		Elvees::Output(Elvees::TInfo, TEXT("Capture format cant be set, using default"));
		return true;
	}

	HRESULT hr;
	long lVStandard;

	// Detect selected format
	lVStandard = AnalogVideo_None;
	if(m_pVdoDec)
	{
		hr = m_pVdoDec->get_TVFormat(&lVStandard);
		if(FAILED(hr))
		{
			Elvees::OutputF(Elvees::TError, TEXT("Fail to get selected TV format hr=0x%08lX"), hr);
			return false;
		}
	}

	bool bDone = false;
	bool bDetectFormat  = false;

	long lWidthCap, lHeightCap;
	REFERENCE_TIME AvgTPF, AvgTPFMin, AvgTPFMax;

	// Set 1000 fps will be adjusted to max value...
	AvgTPF = lFrmRate > 0 ? 10000000I64/lFrmRate : 10000I64;
	lWidthCap = lHeightCap = 0;

	// Enumerate stream capabilities
	AM_MEDIA_TYPE* pmtConfig = NULL;
	int iCount = 0, iSize = 0;
	hr = m_pVSC->GetNumberOfCapabilities(&iCount, &iSize);

	// Check the size to make sure we pass in the correct structure.
	if(SUCCEEDED(hr) && iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
	{
		// Use the video capabilities structure.
		for(int iFormat = 0; iFormat < iCount; iFormat++)
		{
			VIDEO_STREAM_CONFIG_CAPS scc;

			hr = m_pVSC->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
			if(SUCCEEDED(hr))
			{
				// accept only selected format and VideoInfo type

				if( (scc.VideoStandard & lVStandard) == lVStandard &&
					pmtConfig->formattype == FORMAT_VideoInfo &&
					pmtConfig->cbFormat >= sizeof(VIDEOINFOHEADER) &&
					pmtConfig->pbFormat != NULL)
				{
					VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(pmtConfig->pbFormat);

				#ifdef _DEBUG
					Elvees::OutputF(Elvees::TTrace, TEXT("# \'%s\' (%ldx%ld) (%ldx%ld - %ldx%ld) (%ldx%ld)"),
						GetSubtypeName(&pmtConfig->subtype),
						scc.InputSize.cx, scc.InputSize.cy,
						scc.MinOutputSize.cx, scc.MinOutputSize.cy,
						scc.MaxOutputSize.cx, scc.MaxOutputSize.cy,
						scc.MaxFrameInterval > 0 ? long(10000000I64 / scc.MaxFrameInterval) : -1L,
						scc.MinFrameInterval > 0 ? long(10000000I64 / scc.MinFrameInterval) : -1L);
				#endif

					if(pmtConfig->subtype == MEDIASUBTYPE_YUY2)
					{
						if(lWidthCap < scc.InputSize.cx || lHeightCap < scc.InputSize.cy)
						{
							AvgTPFMin = scc.MinFrameInterval;
							AvgTPFMax = scc.MaxFrameInterval;

							lWidthCap  = scc.InputSize.cx;
							lHeightCap = scc.InputSize.cy;

							bDetectFormat = true;
						}

						if(!bDone)
						if( scc.MinOutputSize.cx <= lWidth &&
							scc.MaxOutputSize.cx >= lWidth &&
							scc.MinOutputSize.cy <= lHeight &&
							scc.MaxOutputSize.cy >= lHeight)
						{
							pVih->AvgTimePerFrame = AvgTPF;

							if(pVih->AvgTimePerFrame < scc.MinFrameInterval)
								pVih->AvgTimePerFrame = scc.MinFrameInterval;

							if(pVih->AvgTimePerFrame > scc.MaxFrameInterval)
								pVih->AvgTimePerFrame = scc.MaxFrameInterval;

							pVih->bmiHeader.biWidth  = lWidth;
							pVih->bmiHeader.biHeight = lHeight;
							pVih->bmiHeader.biSizeImage = DIBSIZE(pVih->bmiHeader);

							hr = m_pVSC->SetFormat(pmtConfig);
							if(SUCCEEDED(hr))
								bDone = true;
						}
					}
				}

				DeleteMediaType(pmtConfig);
			}
		}
	}
	else
	{
		Elvees::OutputF(Elvees::TError, TEXT("Fail to get stream capabilities hr=0x%08lX"), hr);
	}

	if(!bDone && bDetectFormat)
	{
		AM_MEDIA_TYPE mt;
		VIDEOINFOHEADER vi;

		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
		ZeroMemory(&vi, sizeof(VIDEOINFOHEADER));

		mt.majortype  = MEDIATYPE_Video;
		mt.subtype    = MEDIASUBTYPE_YUY2;
		mt.formattype = FORMAT_VideoInfo;

		mt.bFixedSizeSamples = TRUE;
		mt.bTemporalCompression = FALSE;
		
		mt.cbFormat = sizeof(VIDEOINFOHEADER);
		mt.pbFormat = reinterpret_cast<BYTE*>(&vi);

		vi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		vi.bmiHeader.biBitCount = 16;
		vi.bmiHeader.biPlanes = 1;
		vi.bmiHeader.biWidth  = lWidthCap;
		vi.bmiHeader.biHeight = lHeightCap;
		vi.bmiHeader.biCompression = mmioFOURCC('Y','U','Y','2');
		vi.bmiHeader.biSizeImage = DIBSIZE(vi.bmiHeader);

		mt.lSampleSize = vi.bmiHeader.biSizeImage;

		vi.dwBitErrorRate = 0;
		vi.AvgTimePerFrame = AvgTPF;

		if(vi.AvgTimePerFrame < AvgTPFMin)
			vi.AvgTimePerFrame = AvgTPFMin;

		if(vi.AvgTimePerFrame > AvgTPFMax)
			vi.AvgTimePerFrame = AvgTPFMax;

		if(vi.AvgTimePerFrame > 0)
			vi.dwBitRate = 8 * mt.lSampleSize * long(10000000I64 / vi.AvgTimePerFrame);

		hr = m_pVSC->SetFormat(&mt);
		if(SUCCEEDED(hr))
			bDone = true;
	}

#ifdef _DEBUG
	hr = m_pVSC->GetFormat(&pmtConfig);
	if(SUCCEEDED(hr))
	{
		if( pmtConfig->formattype == FORMAT_VideoInfo &&
			pmtConfig->cbFormat >= sizeof(VIDEOINFOHEADER) &&
			pmtConfig->pbFormat != NULL)
		{
			VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(pmtConfig->pbFormat);

			Elvees::OutputF(Elvees::TTrace, TEXT("Device format is \'%s\' %ldx%ldx%ld"),
				GetSubtypeName(&pmtConfig->subtype),
				pVih->bmiHeader.biWidth,
				pVih->bmiHeader.biHeight,
				pVih->AvgTimePerFrame > 0 ? long(10000000I64 / pVih->AvgTimePerFrame) : -1L);
		}

		DeleteMediaType(pmtConfig);
	}
#endif

	return bDone;
}

inline bool CDevice::SetVideoProc(bool bBWMode)
{
	if(!m_pVPA)
	{
		Elvees::Output(Elvees::TInfo, TEXT("Device driver don't have video processing"));
		return true;
	}

	struct DevicePropeties
	{
		DWORD	lProperty;
		LPCTSTR	stName;
	};

	DevicePropeties ProtertyMap[] = {
		{ VideoProcAmp_Brightness,	TEXT("Brightness") },
		{ VideoProcAmp_Contrast,	TEXT("Contrast") },
		{ VideoProcAmp_Hue,			TEXT("Hue") },
		{ VideoProcAmp_Saturation,	TEXT("Saturation") },
		{ VideoProcAmp_Sharpness,	TEXT("Sharpness") },
		{ VideoProcAmp_Gamma,		TEXT("Gamma") },
		{ VideoProcAmp_ColorEnable, TEXT("ColorEnable") },
		{ VideoProcAmp_Gain,		TEXT("Gain") },
		{ VideoProcAmp_WhiteBalance, TEXT("WhiteBalance") },
		{ VideoProcAmp_BacklightCompensation, TEXT("Backlight") }
	};

	HRESULT hr;
	long lMin, lMax, lDefault;
	long lSteppingDelta, lCapsFlags;
	long lValue, lFlags;

	for(int i = 0; i < countof(ProtertyMap); i++)
	{
		hr = m_pVPA->GetRange(ProtertyMap[i].lProperty, &lMin, &lMax,
			&lSteppingDelta, &lDefault, &lCapsFlags);

		if(hr == S_OK)
		{
			lValue = lFlags = -1;

			hr = m_pVPA->Get(ProtertyMap[i].lProperty, &lValue, &lFlags);
			if(hr != S_OK)
			{
				Elvees::OutputF(Elvees::TError, TEXT("Unexpected error in IAMVideoProcAmp::Get( \"%s\" ) hr=0x%08lX"),
					ProtertyMap[i].stName, hr);
			}

			if( lFlags == (VideoProcAmp_Flags_Auto | VideoProcAmp_Flags_Manual) )
				lFlags = VideoProcAmp_Flags_Auto;

			DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("# \"%s\" = %ld(%ld) (%ld - %ld, %ld) Flags = %ld(%ld)"),
				ProtertyMap[i].stName,
				lValue, lDefault,
				lMin, lMax, lSteppingDelta,
				lFlags, lCapsFlags));

			if(ProtertyMap[i].lProperty == VideoProcAmp_Saturation)
			{
				m_pVPA->Set(VideoProcAmp_Saturation, bBWMode ? lMin : lDefault, VideoProcAmp_Flags_Manual);
			}
			else if(ProtertyMap[i].lProperty == VideoProcAmp_ColorEnable)
			{
				// in most card do as line about...
				m_pVPA->Set(VideoProcAmp_ColorEnable, bBWMode ? 0 : 1, VideoProcAmp_Flags_Manual);
			}
			else
			{
				m_pVPA->Set(ProtertyMap[i].lProperty, lDefault, lFlags);
			}
		}
		else if(hr == E_PROP_ID_UNSUPPORTED)
		{
			DEBUG_ONLY(Elvees::OutputF(Elvees::TTrace, TEXT("# \"%s\" unsupported"),
				ProtertyMap[i].stName));
		}
		else
		{
			Elvees::OutputF(Elvees::TError, TEXT("Unexpected error in IAMVideoProcAmp::GetRange( \"%s\" ) hr=0x%08lX"),
				ProtertyMap[i].stName, hr);
		}
	}

	return true;
}