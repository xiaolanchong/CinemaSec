//========================= Copyright © 2004, Elvees ==========================
// Author: Eugene V. Gorbachev
// Purpose: CVideoBackground manages video stream from video files(.avi e.g.)
//
//=============================================================================

// VideoBackground.h: interface for the CVideoBackground class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOBACKGROUND_H__5B68548D_DE16_4B09_88A7_C76C2EF58198__INCLUDED_)
#define AFX_VIDEOBACKGROUND_H__5B68548D_DE16_4B09_88A7_C76C2EF58198__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Background.h"
#include <string>
#include <atlbase.h>
#include "../../CSUtility/qedit.h"
#include <dshow.h>
#include "../Common/Exception.h"

#include <vector>
#include "Sync.h"

#define WM_GRAPHNOTIFY		WM_USER+0x100

MACRO_EXCEPTION(VideoException, CommonException);
MACRO_EXCEPTION(VideoSizeException, VideoException);
MACRO_EXCEPTION(VideoFileException, VideoException);
MACRO_EXCEPTION(DirectShowException, VideoFileException);

//-----------------------------------------------------------------------------
// Purpose: Works w/ DShow grabbing samples from stream & copy it to internal buffer
//			also can render on Graphics & CDC
//-----------------------------------------------------------------------------
class CVideoBackground : public CControlBackground  ,
						 private ISampleGrabberCB
{
	long	m_Width;
	long	m_Height;
	CWnd*	m_pWnd;

	STDMETHODIMP_(ULONG) AddRef() { return 2; }
	STDMETHODIMP_(ULONG) Release() { return 1; }
	// Fake out any COM QI'ing
	//
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv)
	{
		{if((ppv)==NULL) return (E_POINTER);}
       
		if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ) 
		{
			*ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
			return NOERROR;
		}    
		return E_NOINTERFACE;
	}

	STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample ){ return 0;}	// don't implement
	STDMETHODIMP BufferCB( double SampleTime, BYTE * pBuffer, long BufferSize );	
	void SetSize(long Width, long Height) { m_Height = Height ; m_Width = Width; }
	void	Grab(CWnd* pWnd, const std::wstring& FileName);

    CComPtr< ISampleGrabber >	pGrabber;
    CComPtr< IBaseFilter >		pSource;
    CComPtr< IGraphBuilder >	pGraph;
    CComPtr< IVideoWindow >		pVideoWindow;

	BITMAPINFO			m_bi;
	std::vector<BYTE>	m_Bits;
	CRITICAL_SECTION	m_cs;
	CSync				m_Sync;

public:
	CVideoBackground(CWnd* pWnd, const std::wstring& FileName);
	virtual ~CVideoBackground();

	virtual void	Draw(Graphics& gr, const CRectF& rc) const;

	void			Stop();
	void			Pause();
	void			Play();

	float			GetPos() const;
	void			SetPos(float Pos); 
	void			Replay();
	void			ProcessMessage();
	bool			IsSeekable();

	OAFilterState	GetState();
	void			Stop2();

	virtual CSize			GetSize() const { return CSize( m_Width, m_Height); }
};

#endif // !defined(AFX_VIDEOBACKGROUND_H__5B68548D_DE16_4B09_88A7_C76C2EF58198__INCLUDED_)
