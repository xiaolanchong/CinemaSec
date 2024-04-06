// BaseFrameGrabberDS.h: interface for the BaseFrameGrabberDS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEFRAMEGRABBERDS_H__18D44C20_04C5_4A98_B84F_AA397926D50E__INCLUDED_)
#define AFX_BASEFRAMEGRABBERDS_H__18D44C20_04C5_4A98_B84F_AA397926D50E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../CSUtility/utility/base_frame_grabber.h"
#include "../FrameGrabber.h"
#include "IGrabberFactory.h"
#include <string>
#include <atlbase.h>
#include <qedit.h>
#include <dshow.h>
#include <vector>

#include "../Common/Exception.h"
#if 0
#include "Subclass.h"
#endif

#define WM_GRAPHNOTIFY		WM_USER+0x100

MACRO_EXCEPTION(VideoException2,		CommonException);
MACRO_EXCEPTION(VideoSizeException2, VideoException2);
MACRO_EXCEPTION(VideoFileException2, VideoException2);
MACRO_EXCEPTION(DirectShowException2, VideoFileException2);

//! \brief DirectShow �������
//! \version 1.0
//! \date 10-19-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class BaseFrameGrabberDS  :	public BaseFrameGrabberEx,  // old interface
							public IFileGrabber			// new interface
{
	//! ���� ��� ���������
	HWND		m_hWndStatic;
	//! �������������� ������� ���������
	WNDPROC		m_fnOldWndProc;
	//! ��������� ������������� COM
	HRESULT		m_hrCoInit;

public:
	//! ������� � ��������� �����������
	//! \param nBitPerPixel ���/������� ���� <32, ����� FOURCC ��������� �������
	//! \param bPerFrameMode ���������� �����?
	//! \param bLoopback ���������?
	BaseFrameGrabberDS( int nBitPerPixel, bool bPerFrameMode , bool bLoopback );
	virtual ~BaseFrameGrabberDS();

	//! ������� ������ �����������
	//! \return ������
	CSize	GetSize() const { return CSize(m_bi.bmiHeader.biWidth, m_bi.bmiHeader.biHeight); }
	//! ������� ��������� DS �����
	//! \return ��������� DS �����
	OAFilterState	GetState() const;
	//! ������� ������� ������� � �����
	//! \return 0.0 - ������, 1.0 - �����
	float			GetPos() const;
	//! ���������� ����
	//! \return true - success, false - failing
	bool	StopGraph();
	//! ��������� ����
	//! \return true - success, false - failing
	bool	PlayGraph();
	//! ������������� ����
	//! \return true - success, false - failing
	bool	PauseGraph();

	//! ���������� �������� ������������
	//! \param r �������� ������������, 1.0 - ����������
	void	SetRate(float r) { m_fRate = r; };

	// BaseFrameGrabber
	//! ������� ���� � ������ ������������
	//! \param name ��� �����
	//! \param param ��������� ������������
	//! \return true - success, false - failing
	virtual bool Start( LPCTSTR name, const FrameGrabberParameters & param ) ;
	//! ���������� ������������
	//! \return true - success, false - failing
	virtual bool Stop() ;
	//! ������� ��������� ������
	//! \return true - ������� ���������, false - ������
	virtual bool IsOk() const ;
	//! ��������� �� ����� �����
	//! \return true - ����� �����, false - ���
	virtual bool IsEnd() const ;
	//! ������������� ������� ���� � ������������� �� ���������
	//! \param pHeader ��������� ����������� 
	//! \param pImage ������ ������������ 
	//! \param imgSize ������ �����������
	//! \return true - success, false - failing
	virtual bool LockLatestFrame( LPCBITMAPINFO & pHeader, LPCUBYTE & pImage, int & imgSize ) ;
	//! ������������� ����
	//! \return true - success, false - failing
	virtual bool UnlockProcessedFrame() ;
	//! ���� �� ����� �����
	//! \return true - ��, false - ���
	virtual bool HasFreshFrame() const ;

	// BaseFrameGrabberEx
	//! ������� ������ ��������
	//! \return ������ ��������
	virtual DWORD	GetVersion()				{ return 1;};
	//! ������� ������ �����������
	//! \param x ������
	//! \param y ������
	//! \return 0 - �����, 1 - ������
	virtual DWORD	GetSize(DWORD& x, DWORD& y) 
	{ 
		CSize sz = GetSize(); 
		x = sz.cx; 
		y = sz.cy;
		return 0;
	};
	//! ������� ������� ������� � �����
	//! \param pos 0.0 - ������, 1.0 - �����
	//! \return 0 - �����, 1 - ������
	virtual DWORD	GetPos( float& pos )		
	{
		pos = GetPos();
		return 0;
	}

	// IFileGrabber
	//!  ������� ������ �����������
	//! \param sizeFrame ������ �����������
	//! \return S_OK - success, other - failing
	virtual HRESULT GetSize( SIZE& sizeFrame )	
	{
		DWORD x , y;
		DWORD res = GetSize(x, y);
		if( res == S_OK ) 
		{
			sizeFrame.cx = x;
			sizeFrame.cy = y;
		}
		return res;
	}
	//!  ������� ������ �����������
	//! \param nCurPos ����� �������� �����
	//! \return S_OK - success, other - failing
	virtual HRESULT GetCurFrame( INT64 &nCurPos );		
	//! ������� ���-�� ������ � �����������
	//! \param &nTotal ���-�� ������
	//! \return S_OK - success, other - failing
	virtual HRESULT GetTotalFrames( INT64 &nTotal )	;	
	//! ������������� �� ����
	//! \param nCurPos ����� �����
	//! \return S_OK - success, other - failing
	virtual HRESULT Seek( INT64 nCurPos )	;
	//! ������� frame per second
	//! \param fFPS fps
	//! \return S_OK - success, other - failing
	virtual HRESULT GetFPS(double& fFPS);
	//! ������������� ����,������� �� ���������
	//! \param pHeader ��������� ����������� �� ��������� ������
	//! \param pImage ������ �����������
	//! \param nImgSize ������ ������
	//! \return S_OK - success, other - failing
	virtual HRESULT LockFrame( const BITMAPINFO * & pHeader, const BYTE * & pImage, int &  nImgSize ) ;
	//! �������������� ����
	//! \return S_OK - success, other - failing
	virtual HRESULT UnlockFrame()	;
	//! ������������
	virtual void	Release() { delete this; }

	//! ���������� ��������� �� DirectShow
	//! \param evCode ��� ���������
	void			ProcessCode( LONG evCode );
protected:
	//! ������ �����������
	long	m_Width;
	//! ������ �����������
	long	m_Height;
	//! ���/�������
	int				m_nBitPerPixel;

	//! ����� ��� BITMAPINFO
	struct BmpEx : BITMAPINFO 
	{
		RGBQUAD rgb[255];
	}	m_bi;

	//! cookie �� ROT
	DWORD	m_dwRegister;
	//! �������� ������������
	float	m_fRate;
	//! ���������?
	bool	m_bPerFrameMode;
	//! ���������?
	bool	m_bLoopback;

	//! ����� ��� �����
	std::vector<BYTE>	m_SampleBuffer;
	//! fps
	double				m_fFPS;

	//! ����� ������� ��������� ��� ��������� ��������� �� DirectShow
	static LRESULT	WindowProc( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp  );
	
	//! 
	//! \param Width 
	//! \param Height 
	void	SetSize(long Width, long Height) { m_Height = Height ; m_Width = Width; }
	//! ������� ���� ��� ������������
	//! \param FileName ��� �����
	void	Grab( const std::wstring& FileName);

	//! for ROT /debug/
	HRESULT AddToRot(IUnknown *pUnkGraph) ;
	//! ������� �� ROT
	void	RemoveFromRot();
	
	//! ������ ��� �������� ����������� �� �����
    CComPtr< ISampleGrabber >	m_pGrabber;
	//! ���������� ����������� �� �����
	CComPtr< IMediaEventEx>		m_pMediaEvent;
	//! ��������� ���������� �����
    CComPtr< IGraphBuilder >	m_pGraph;
	//! ���������� ���������� ����������� �����������
	CComPtr< IVideoFrameStep>	m_pFrameStep;
#ifdef _DEBUG
	//! ���-�� ��������������� ������
	mutable LONG	m_dwLockNumber;
	//! ����� ���������
	mutable DWORD	m_dwTimeTicks;
#endif
};

#endif // !defined(AFX_BASEFRAMEGRABBERDS_H__18D44C20_04C5_4A98_B84F_AA397926D50E__INCLUDED_)
