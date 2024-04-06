//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	�������� ������������� �����������
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 19.08.2005
//                                                                                      //
//======================================================================================//
#ifndef _FILE_GRABBER_FACTORY_6400758542943037_
#define _FILE_GRABBER_FACTORY_6400758542943037_

#include "IGrabberFactory.h"

//======================================================================================//
//                               class FileGrabberFactory                               //
//======================================================================================//

//! \brief ������� �������� ��������
//! \version 1.0
//! \date 10-18-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)Eugene
//! \todo 
//! \bug 
//!
class FileGrabberFactory
{
protected:
	FileGrabberFactory();
	virtual ~FileGrabberFactory();

	//! ������� ��� ���������� �����������
	MyDebugOutputImpl				m_Debug;

	//! ������� �������� ������� � ������� VFW
	//! \param ppFrameGrabber �������� �������
	//! \param szFileName ��� ����� �����
	//! \param nBitsPerPixel ���/������� ��������� ����������� ��� FOURCC ���� < 32
	//! \param bPerFrame ���������?
	//! \param bLoopback ���������?
	//! \return S_OK - success, other - failing
	HRESULT		CreateNormalFileGrabber(	IFileGrabber** ppFrameGrabber, 
											LPCWSTR  szFileName,
											int nBitsPerPixel, 
											bool bPerFrame, 
											bool bLoopback ) ;

	//! ������� �������� ������� � ������� AVD ������������� �� CSUtility
	//! \param szFileName ��� ����� �����
	//! \param nBitsPerPixel ���/������� ��������� �����������, ������������, ������ 24
	//! \param bPerFrame ���������?
	//! \param bLoopback ���������?
	//! \return S_OK - success, other - failing
	HRESULT		CreateAVDFileGrabber(	IFileGrabber** ppFrameGrabber, 
										LPCWSTR  szFileName,
										int nBitsPerPixel, 
										bool bLoopback ) ;

	//! ������������� �� ���������� �� CreateNormalFileGrabber ��� CreateAVDFileGrabber
	//! \param ppFrameGrabber �������� �������
	//! \param szFileName ��� ����� �����
	//! \param nBitsPerPixel ���/������� ��������� ����������� ��� FOURCC ���� < 32
	//! \param bPerFrame ���������?
	//! \param bLoopback ���������?
	//! \return S_OK - success, other - failing
	HRESULT		CreateFileGrabber(	IFileGrabber** ppFrameGrabber, 
									LPCWSTR  szFileName,
									int nBitsPerPixel, 
									bool bPerFrame, 
									bool bLoopback ) ;

};

#endif // _FILE_GRABBER_FACTORY_6400758542943037_