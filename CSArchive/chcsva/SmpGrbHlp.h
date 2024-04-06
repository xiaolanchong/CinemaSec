// SmpGrbHlp.h: interface for the CSampleGrabberHelper class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

// Forward declaration
struct IMediaSample;
struct ISampleGrabber;
struct ISampleGrabberCB;

//////////////////////////////////////////////////////////////////////
// CSampleGrabberHelper
//////////////////////////////////////////////////////////////////////

class CSampleGrabberHelper
{
public:
	CSampleGrabberHelper();
	virtual ~CSampleGrabberHelper();

	// Called in context of SampleGrabber filter thread
	virtual void OnSampleCB(double fTime, IMediaSample *pSample);
	virtual void OnBufferCB(double fTime, BYTE *pData, long lLen);

protected:
	// WhichMethodToCallback 0: Call OnSampleCB, 1: Call OnBufferCB.
	bool SetCallback(ISampleGrabber *pSG,
			long WhichMethodToCallback = 1,
			bool bBufferSamples = false,
			bool bOneShot = false);
	
private:
	// Create Callback interface
	ISampleGrabberCB* CreateSampleGrabberCB();

	// No copies do not implement
	CSampleGrabberHelper(const CSampleGrabberHelper &rhs);
	CSampleGrabberHelper &operator=(const CSampleGrabberHelper &rhs);
};
