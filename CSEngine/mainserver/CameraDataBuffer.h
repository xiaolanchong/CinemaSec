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
//   Date:   01.03.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __CAMERA_DATA_BUFFER_H_INCLUDED_2821566035470171__
#define __CAMERA_DATA_BUFFER_H_INCLUDED_2821566035470171__

//=====================================================================================//
//                               class CameraDataBuffer                                //
//=====================================================================================//

#include "../IPC/NetAddr.h"
#include "../../CSChair/COMMON/Exception.h"

MACRO_EXCEPTION( CameraDataBufferException, CommonException )
MACRO_EXCEPTION( NoSuchDataException,		CameraDataBufferException )
MACRO_EXCEPTION( NoSuchImageException,		CameraDataBufferException )
MACRO_EXCEPTION( NoSuchCameraException,		CameraDataBufferException )

//! ����� ��� ����� ������ �� ICameraAnalyzer � IHallAnalyzer
class CameraDataBuffer
{
	//! ����� �������, �� �������� ������� ������
	netadr_t							m_NetAddress;
	//! ������ ���������� �������
	std::vector<int>					m_RequestedCameras;
	//! ������ ��������� �� ������ ������
	std::map<int, std::vector<BYTE> >	m_CameraData;
	//! ����������� �� ������ ������
	std::map<int, std::vector<BYTE> >	m_CameraImages;
public:
	CameraDataBuffer();
	~CameraDataBuffer();

	//! �������� ������
	void Clear(  )
	{
		m_RequestedCameras.clear();
		m_CameraData.clear();
		m_CameraImages.clear();
	}

	//! ������� ������ ��� ����������
	//! \param NetAddress ����� ���������� �������
	//! \param CameraNo ������ ������������� �����
	void	NewRequest(netadr_t NetAddress, const std::vector<int>& CameraNo);

	//! ������� ������������� ������
	//! \return ��������������
	const std::vector<int>&	GetCameras() const { return m_RequestedCameras;}

	//! �������� �� ��� ����������� �� �������
	//! \return ��/���
	bool	IsAllImages()	const;

	//! �������� �� ��� ������ �� �������
	//! \return ��/���
	bool	IsAllData()		const;

	//! �������� �� ������ � ����������� �� �������
	//! \return ��/���
	bool	IsAllRequired() const { return IsAllImages() && IsAllData();}

	//! �� ����� ������� ��������� ����������
	//! \param Cameras �������������� �����
	void	WhatCamerasRequired(std::set<int>& Cameras) const ;

	//! ���������� ������ �� ������
	//! \param nCameraNo ������������� ������
	//! \param DataArr �������� ������ �� ���������
	void	SetData(int nCameraNo, const std::vector<BYTE>& DataArr);

	//! ���������� ����������� �� ������
	//! \param nCameraNo ������������� ������
	//! \param ImageArr ����������� � ������� JPEG
	void	SetImage(int nCameraNo, const std::vector<BYTE>& ImageArr);

	//! �������� ����������� �� ������
	//! \param nCameraNo ������������� ������
	//! \return ����������� � ������� JPEG
	const std::vector<BYTE>&	GetImage(int nCameraNo) const;

	//! �������� ������ �� ������
	//! \param nCameraNo ������������� ������
	//! \return ������
	const std::vector<BYTE>&	GetData(int nCameraNo) const;

	//! ������� ����� �������
	//! \return ������� �����
	netadr_t					GetNetAddress( ) const { return m_NetAddress;};
};

#endif //__CAMERA_DATA_BUFFER_H_INCLUDED_2821566035470171__
