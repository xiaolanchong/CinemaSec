// Device.h: interface for the CDevice class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "SmpGrbHlp.h"

// Forward declaration
struct IUnknown;
struct IPin;
struct IAMCrossbar;
struct IAMStreamConfig;
struct IAMVideoProcAmp;
struct IAMAnalogVideoDecoder;
struct IAMDroppedFrames;
struct IBaseFilter;
struct IMediaControl;
struct IMediaEventEx;
struct IMediaSeeking;
struct IGraphBuilder;
struct ICaptureGraphBuilder2;

//////////////////////////////////////////////////////////////////////
// CDevice

//
//	WDM Video Capture Filter
//
//	Filter Interfaces:
//		+IAMAnalogVideoDecoder
//		-IAMCameraControl
//		+IAMDroppedFrames
//		-IAMExtDevice
//		-IAMExtTransport
//		-IAMFilterMiscFlags
//		-IAMTimecodeReader
//		-IAMVideoCompression
//		-IAMVideoControl
//		+IAMVideoProcAmp
//		+IAMStreamConfig
//		+IAMCrossbar

class CDevice : public CSampleGrabberHelper
{
public:
	CDevice();
	virtual ~CDevice();
	
	bool InitDevice(LPCTSTR stMoniker,
		long lWidth, long lHeight, long lFrmRate,
		long lInput, long lVStandard, bool bBWMode);
	
	bool StartDevice();
	void StopDevice();

	bool IsDeviceValid();
	bool IsDeviceStarted(long msTimeout = 0);

	long IsSignalLocked();

	bool SetEventHWND(HWND hWnd, long lMsg);
	void HandleDeviceEvents();

	long GetNumDropped();
	long GetNumNotDropped();

	virtual long GetDeviceID() { return -1; };

public:
	// Static helper functions
	static HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	static void RemoveGraphFromRot(DWORD pdwRegister);

	static IPin* GetPin(IBaseFilter* pFilter, int iNum, bool input);

protected:
	// Ask for accept stream format
	virtual bool OnImageFormat(BITMAPINFOHEADER *pbihImg);

	// Called in context of SampleGrabber filter thread
	virtual void OnBufferCB(double fTime, BYTE *pData, long lLen);

	void DropFrame();

private:
	void CloseInterfaces();
	
	bool CreateBaseInterfaces();
	bool CreateFilter(IBaseFilter **ppFilter, LPCTSTR stFilterMoniker);
	bool CreateSourceBaseInterfaces(IBaseFilter *pSource);

	bool RouteToSignaledPin(long lDesirePin = -1);	// Search for signaled
	bool SetTVFormat(long lVStandard = 16);			// AnalogVideo_PAL_B
	bool SetCaptureFormat(long lWidth, long lHeight, long lFrmRate = 0);
	bool SetVideoProc(bool bBWMode);

	bool PrepareStartCapture(IBaseFilter *pSource);

private:
	IAMCrossbar	     *m_pXbar;
	IAMStreamConfig  *m_pVSC;
	IAMVideoProcAmp  *m_pVPA;
	IAMDroppedFrames *m_pVDrp;
	IAMAnalogVideoDecoder* m_pVdoDec;

	IMediaControl *m_pMC;
	IMediaEventEx *m_pME;
	IMediaSeeking *m_pMS;
	IGraphBuilder *m_pGraph;
	ICaptureGraphBuilder2 *m_pCapture;

	bool m_bValid;
	bool m_bHasStreamControl;

	long m_lDroppedNotBase;
	long m_lDroppedBase;

	DWORD m_dwGraphRegister;

	// No copies do not implement
	CDevice(const CDevice &rhs);
	CDevice &operator=(const CDevice &rhs);
};