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

//! класс для сбора данных от ICameraAnalyzer к IHallAnalyzer
class CameraDataBuffer
{
	//! адрес сервера, от которого пришлди данные
	netadr_t							m_NetAddress;
	//! камеры удаленного сервера
	std::vector<int>					m_RequestedCameras;
	//! данные алгоритма по каждой камере
	std::map<int, std::vector<BYTE> >	m_CameraData;
	//! изображения по каждой камере
	std::map<int, std::vector<BYTE> >	m_CameraImages;
public:
	CameraDataBuffer();
	~CameraDataBuffer();

	//! очистить данные
	void Clear(  )
	{
		m_RequestedCameras.clear();
		m_CameraData.clear();
		m_CameraImages.clear();
	}

	//! создать запрос для компьютера
	//! \param NetAddress адрес удаленного сервера
	//! \param CameraNo список запрашиваемых камер
	void	NewRequest(netadr_t NetAddress, const std::vector<int>& CameraNo);

	//! вернуть запрашиваемые камеры
	//! \return идентификаторы
	const std::vector<int>&	GetCameras() const { return m_RequestedCameras;}

	//! получены ли все изображения по камерам
	//! \return да/нет
	bool	IsAllImages()	const;

	//! получены ли все данные по камерам
	//! \return да/нет
	bool	IsAllData()		const;

	//! получены ли данные и изображения по камерам
	//! \return да/нет
	bool	IsAllRequired() const { return IsAllImages() && IsAllData();}

	//! по каким камерам требуется информация
	//! \param Cameras идентификаторы камер
	void	WhatCamerasRequired(std::set<int>& Cameras) const ;

	//! установить данные по камере
	//! \param nCameraNo идентификатор камеры
	//! \param DataArr двоичные данные от алгоритма
	void	SetData(int nCameraNo, const std::vector<BYTE>& DataArr);

	//! установить изображение по камере
	//! \param nCameraNo идентификатор камеры
	//! \param ImageArr изобоажение в формате JPEG
	void	SetImage(int nCameraNo, const std::vector<BYTE>& ImageArr);

	//! получить изображение от камеры
	//! \param nCameraNo идентификатор камеры
	//! \return изображение в формате JPEG
	const std::vector<BYTE>&	GetImage(int nCameraNo) const;

	//! получить данные от камеры
	//! \param nCameraNo идентификатор камеры
	//! \return данные
	const std::vector<BYTE>&	GetData(int nCameraNo) const;

	//! вернуть адрес сервера
	//! \return сетевой адрес
	netadr_t					GetNetAddress( ) const { return m_NetAddress;};
};

#endif //__CAMERA_DATA_BUFFER_H_INCLUDED_2821566035470171__
