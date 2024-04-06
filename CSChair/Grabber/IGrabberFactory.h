//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   21.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __I_GRABBER_FACTORY_H_INCLUDED_2221272137662688__
#define __I_GRABBER_FACTORY_H_INCLUDED_2221272137662688__

//! GUID ��� ���������� ������� ������������
#define		CLIENT_GRABBER_FACTORY	L"{45EF49AC-BFBE-4E40-ACA5-54F9264331B6}"
//! GUID ��� ��������� ������� ������������
#define		SERVER_GRABBER_FACTORY	L"{A163DB7C-9145-43e5-8052-6B374AACC157}"
//! ��� �������� ��������, for tests only : generate image with time
#define		STUB_GRABBER_FACTORY		L"{3A8B28C9-9655-4F87-8C8F-49C7C34B3A10}"
//! for test only : generate clear gray image
#define		STUB_SERVER_GRABBER_FACTORY	L"{62956F0E-FA72-4dc9-8A70-19461819050A}"


#include "../debug_int.h"
//=====================================================================================//
//                                grabber streams		                               //
//=====================================================================================//
//! ��������� ������ ���������� �����������
struct IBaseGrabber
{
	//! ���������� ���������
	virtual void	Release() = 0;

	//! ������������� ����, ����� ������������� ������� UnlockFrame
	//! �������� ������� �������������
	//! \code
	//! HRESULT hr;
	//! const BITMAPINFO * ppHeader; const BYTE * ppImage; int   nImgSize ;
	//!	hr = pInt->LockFrame( ppHeader, ppImage, nImgSize );
	//! if( hr == S_OK)
	//! {
	//!		// do something with ppImage
	//!		MyProcessImage( ppHeader, ppImage );
	//!		// unlock frame, go to the next frame 
	//!		pInt->UnlockFrame();
	//! }
	//! \endcode
	//! \param	ppHeader	�������� ��������� bmp, ������� �� ������ UnlockFrame
	//! \param	ppImage		������ �����������, ������� �� ������ UnlockFrame
	//! \param	nImgSize	������ ����������� � ������ (��� ��������)
	//! \return	S_OK - �����, ����� ������
	virtual HRESULT LockFrame( const BITMAPINFO * &ppHeader, const BYTE * &ppImage, int   &nImgSize ) = 0;

	//! �������������� ���� ����� LockFrame
	//! \return	S_OK - �����, ����� ������
	virtual HRESULT UnlockFrame()	= 0;
};

//! ��������� ����������� �� ����������
struct IFileGrabber : IBaseGrabber
{
	//! ����� ������ ����������� ��� LockFrame/UnlockFrame
	//! \param	sizeFrame	������ ����������� � ��������
	//! \return	S_OK - �����, ����� ������
	virtual HRESULT GetSize( SIZE& sizeFrame )			= 0;

	//! �������� ����� �������� ����� � ����������
	//! \param	nCurPos		����� �����
	//! \return	S_OK - �����, ����� ������
	virtual HRESULT GetCurFrame( INT64 &nCurPos )		= 0;

	//! �������� ����� ���-�� ������ � ����������
	//! \param	nTotal		����� ���-�� ������
	//! \return	S_OK - �����, ����� ������
	virtual HRESULT GetTotalFrames( INT64 &nTotal )		= 0;

	//! ����������� �� ����
	//! \param	nCurPos		������������� �� ����
	//! \return	S_OK - �����, ����� ������
	virtual HRESULT Seek( INT64 nCurPos )				= 0;

	//! �������� fps �� ��������� �����
	//! \param	fFPS		fps
	//! \return	S_OK - �����, ����� ������
	virtual HRESULT GetFPS(double& fFPS)				= 0;
};

//! ��������� ����������� �� ������
struct ICameraGrabber : IBaseGrabber
{
};

//! ��������� ����������� �� ������
struct IArchiveGrabber : ICameraGrabber
{
	//! ������������� �� ����� 
	//! \param nTime	number of seconds since Jan 1, 1970, ��. _ftime()
	//! \return	S_OK - �����, ����� ������
	virtual HRESULT SeekTo(INT64	nTime)	= 0;	

	//! �������� ������� ����� � ������ ������
	//! \param nTime	number of seconds since Jan 1, 1970, ��. _ftime()
	//! \return	S_OK - �����, ����� ������
	virtual HRESULT	GetTime( INT64 &nTime)	= 0;
};

//=====================================================================================//
//                                struct IGrabberFactory                               //
//=====================================================================================//

//! ��������� ��������� ����������� ������������, Abstract Factory
struct IGrabberFactory
{
	//! ���������� ���������
	virtual void		Release() = 0;

	//! ������������� ����������
	//! \param pDebugInt		��������� �������
	//! \param bLocalVideo		��������� ����� ������� ����� ��� ������� (�� ������������)
	//! \return	S_OK - �����, ����� ������
	virtual HRESULT		Initialize( IDebugOutput* pDebugInt, bool bLocalVideo ) = 0;

	//! �������� ���������� ����������
	//! \param pDebugInt		��������� �������
	//! \return	S_OK - �����, ����� ������
	virtual void		SetDebugOutput( IDebugOutput* pDebugInt ) = 0;

	//! �������� ���������� ������
	//! \param ppFrameGrabber	��������� ������
	//! \param nCameraID		������������� ������ � ��
	//! \return	S_OK - �����, ����� ������
	virtual HRESULT		CreateCameraGrabber( ICameraGrabber** ppFrameGrabber, int nCameraID ) = 0;

	//! �������� ���������� �����
	//! \param ppFrameGrabber	��������� �����
	//! \param szFileName		��� ����������
	//! \param nBitsPerPixel	���-�� ��� �� ������� ���� <= 32, ����� FOURCC ������
	//! \param bPerFrame		���������� �������� (IBaseGrabber::LockFrame/IBaseGrabber::UnlockFrame ���������� �� 1 ���� ������) 
	//! \param bLoopback		������������ (����� ���������� ����� ���� 1�), ��� ������������� ��������� ����� �� ������
	//! \return	S_OK - �����, ����� ������
	virtual HRESULT		CreateFileGrabber(	IFileGrabber** ppFrameGrabber, 
											LPCWSTR  szFileName,
											int nBitsPerPixel, 
											bool bPerFrame, 
											bool bLoopback ) = 0;

	//! �������� ���������� ������
	//! \param ppFrameGrabber	��������� ������
	//! \param nCameraID		������������� ������ � ��, ��� ������� ��������������� �����
	//! \param nStartTime		��������� ����� ������, ��. IArchiveGrabber::GetTime
	//! \return	S_OK - �����, ����� ������
	virtual HRESULT		CreateArchiveGrabber( IArchiveGrabber** ppFrameGrabber, int nCameraID, INT64 nStartTime ) = 0;

};

#endif //__I_GRABBER_FACTORY_H_INCLUDED_2221272137662688__