// BaseFrameGrabberVfW.h: interface for the BaseFrameGrabberVfW class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ELVEES_BASEFRAMEGRABBERVFW_H__INCLUDED_
#define ELVEES_BASEFRAMEGRABBERVFW_H__INCLUDED_

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

//#define BFG_Y800_OUTPUT

// Base classes
#include "../../CSUtility/utility/base_frame_grabber.h"
#include "../FrameGrabber.h"
#include "IGrabberFactory.h"

// Forward declaration
struct IAVIStream;

//////////////////////////////////////////////////////////////////////////
// BaseFrameGrabberVfW
//////////////////////////////////////////////////////////////////////////

//! \brief ���������� ��������� �������� � ������� VFW, powered by Mofo
//! \version 1.0
//! \date 10-18-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//!
class BaseFrameGrabberVfW
	: public BaseFrameGrabberEx		// old interface
	, public IFileGrabber			// new interface
{
public:
	
	//! ������� � �����������
	//! \param nBitsPerPixel ���/������� ��������� ����������� ��� FOURCC ���� < 32
	//! \param bPerFrame ���������?
	//! \param bLoopback ���������?
	//! \param dwWidth �������� ������ �����������, ������������
	//! \param dwHeight �������� ������ �����������, ������������
	//! \return 
	explicit BaseFrameGrabberVfW(
		int  nBitPerPixel,
		bool bPerFrameMode,
		bool bLoopback,
		DWORD dwWidth = 0L,
		DWORD dwHeight = 0L);

	virtual ~BaseFrameGrabberVfW();

public:	
	//! Create factory
	//! \return ���������� ����������
	static BaseFrameGrabberVfW* CreateGrabber();
	//! ��������� ���� �� �������������
	//! \param stFile ��� �����
	//! \return TRUE - ����������, FALSE - ���
	static BOOL IsFileExists(LPCTSTR stFile);

public:
	// BaseFrameGrabber

	//! ������ ������������
	//! \param lpFilename ��� ����������
	//! \param param ��������� ������������
	//! \return true-�����, ����� ������
	virtual bool Start(LPCTSTR lpFilename, const FrameGrabberParameters& param);
	//! ���������� ������������
	//! \return true-�����, ����� ������
	virtual bool Stop();
	//! �������� ���������
	//! \return true-�����, ����� ������
	virtual bool IsOk() const; 
	//! �������� �� ����� �����
	//! \return true-�����, ����� ������
	virtual bool IsEnd() const;
	//! ������������� ��������� ����
	//! \param pHeader ���������
	//! \param pImage ������ �����������
	//! \param nImgSize ������ �����������
	//! \return true-�����, ����� ������
	virtual bool LockLatestFrame(const BITMAPINFO* &pHeader, const BYTE* &pImage, int &nImgSize);
	//! �������������� ����� LockLatestFrame
	//! \return true-�����, ����� ������
	virtual bool UnlockProcessedFrame();
	//! ���� �� ��������� ����
	//! \return true-�����, ����� ������
	virtual bool HasFreshFrame() const;

	// BaseFrameGrabberEx

	//! ������� ������ ��������
	//! \return ������
	virtual DWORD GetVersion();

	//! ������� ������ �����, �������� �� ������������
	//! \param x ������
	//! \param y ������
	//! \return 0-�����, ����� ������
	virtual DWORD GetSize(DWORD& x, DWORD& y);		// ??? return what
	//! ������� ������� ������� � �����
	//! \param pos ������� ������� � ������ 0.0-������, 1.0-�����
	//! \return 0-�����, ����� ������
	virtual DWORD GetPos(float& pos);				// position in percents

	// IBaseGrabber

	//! ���������� ���������
	virtual void Release();
	//! ������������� ����
	//! \param ppHeader 
	//! \param ppImage 
	//! \param nImgSize 
	//! \return S_OK - success, other - failing
	virtual HRESULT LockFrame(const BITMAPINFO* &ppHeader, const BYTE* &ppImage, int &nImgSize);
	//! �������������� ����
	//! \return S_OK - success, other - failing
	virtual HRESULT UnlockFrame();

	// IFileGrabber

	//! ������� ������ �����������
	//! \param sizeFrame ������ �����������
	//! \return S_OK - success, other - failing 
	virtual HRESULT GetSize(SIZE &sizeFrame);
	//! ������� ����� �������� �����
	//! \param nCurPos ������� �������
	//! \return S_OK - success, other - failing 
	virtual HRESULT GetCurFrame(INT64 &nCurPos);
	//! ������� ���-�� ������ � �����
	//! \param nTotal ���-�� ������ � �����
	//! \return S_OK - success, other - failing 
	virtual HRESULT GetTotalFrames(INT64 &nTotal);
	//! ������� �� �������� ����
	//! \param nCurPos ����� �����
	//! \return S_OK - success, other - failing 
	virtual HRESULT Seek(INT64 nCurPos);
	//! ������� ���-�� fps
	//! \param fFPS frame per second
	//! \return S_OK - success, other - failing 
	virtual HRESULT GetFPS(double &fFPS);

protected:
	// Open avi stream
	bool OpenInterfaces(LPCTSTR lpFilename);
	void CloseInterfaces();

	// Create AVS script file with YUY2 output
	bool CreateScriptFile(LPCTSTR lpFilename);

protected:
	//! ���/������� ��������� ����������� ��� FOURCC ���� < 32
	int   m_nBitPerPixel;
	//! ���������?
	bool  m_bPerFrameMode;
	//! ���������?
	bool  m_bLoopback;
	//! �������� ������ �����������, ������������
	DWORD m_dwDesiredWidth;
	//! �������� ������ �����������, ������������
	DWORD m_dwDesiredHeight;

	//!	���� �������������
	bool m_bInited;

	IAVIStream       *m_pStream;	//!< AVI stream interface
	BITMAPINFOHEADER *m_pbiStream;	//!< first video stream format
	void *m_pFrame;					//!< frame buffer
	long  m_cbFrame;				//!< frame buffer size

	double m_FPS;					//!< Frame per second
	DWORD m_Frame;					//!< Current frame
	DWORD m_FrameStart;				//!< Start
	DWORD m_FrameTotal;				//!< Length

	TCHAR m_stScriptFile[MAX_PATH];	//!< Temporary script file

#ifdef BFG_Y800_OUTPUT
	BITMAPINFOHEADER  m_biY800;
	void *m_pFrameY800;				//!< Y800 frame buffer
	long  m_cbFrameY800;			//!< Y800 frame buffer size

	void YUY2toY800(LPBITMAPINFOHEADER lpbiIn, const void* pSrc, void* pDst);
#endif

private:
	//! Output functions for errors
	//! \param stFormat ������ ��������������
	void Output(LPCTSTR stFormat, ...);

	// No copies do not implement
	BaseFrameGrabberVfW(const BaseFrameGrabberVfW &rhs);
	BaseFrameGrabberVfW &operator=(const BaseFrameGrabberVfW &rhs);
};

#endif // ELVEES_BASEFRAMEGRABBERVFW_H__INCLUDED_
