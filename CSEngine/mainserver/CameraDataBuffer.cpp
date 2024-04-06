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
#include "stdafx.h"
#include "CameraDataBuffer.h"

//=====================================================================================//
//                               class CameraDataBuffer                                //
//=====================================================================================//
CameraDataBuffer::CameraDataBuffer()
{
}

CameraDataBuffer::~CameraDataBuffer()
{
}

void	CameraDataBuffer::NewRequest(netadr_t NetAddress, const std::vector<int>& CameraNo)
{
	Clear();
	m_NetAddress		= NetAddress;
	m_RequestedCameras	= CameraNo;
}

bool	CameraDataBuffer::IsAllImages()	const
{
	for( size_t i = 0 ; i < m_RequestedCameras.size(); ++i )
	{
		if( m_CameraImages.find( m_RequestedCameras[i] ) == m_CameraImages.end() )
		{
			return false;
		}
	}
	return true;
}

bool	CameraDataBuffer::IsAllData()		const
{
	for( size_t i = 0 ; i < m_RequestedCameras.size(); ++i )
	{
		if( m_CameraData.find( m_RequestedCameras[i] ) == m_CameraData.end() )
		{
			return false;
		}
	}
	return true;
}

void	CameraDataBuffer::SetData(int nCameraNo, const std::vector<BYTE>& DataArr)
{
	if( m_RequestedCameras.end() == std::find( m_RequestedCameras.begin(), m_RequestedCameras.end(), nCameraNo ) )
	{
		throw NoSuchCameraException("Error");
	}
	else
	{
		m_CameraData[ nCameraNo ] = DataArr;
	}
}

void	CameraDataBuffer::SetImage(int nCameraNo, const std::vector<BYTE>& ImageArr)
{
	if( m_RequestedCameras.end() == std::find( m_RequestedCameras.begin(), m_RequestedCameras.end(), nCameraNo ) )
	{
		throw NoSuchCameraException("Error");
	}
	else
	{
		m_CameraImages[ nCameraNo ] = ImageArr;
	}
}

const std::vector<BYTE>&	CameraDataBuffer::GetImage(int nCameraNo) const
{
	if( m_RequestedCameras.end() == std::find( m_RequestedCameras.begin(), m_RequestedCameras.end(), nCameraNo ) )
	{
		throw NoSuchCameraException("Error");
	}
	else
	{
		std::map< int , std::vector<BYTE> > ::const_iterator it = m_CameraImages.find( nCameraNo );
		if( it == m_CameraImages.end() )
		{
			throw NoSuchImageException("Error");
		}
		return it->second;
	}
}

const std::vector<BYTE>&	CameraDataBuffer::GetData(int nCameraNo) const
{
	if( m_RequestedCameras.end() == std::find( m_RequestedCameras.begin(), m_RequestedCameras.end(), nCameraNo ) )
	{
		throw NoSuchCameraException("Error");
	}
	else
	{
		std::map< int , std::vector<BYTE> > ::const_iterator it = m_CameraData.find( nCameraNo );
		if( it == m_CameraData.end() )
		{
			throw NoSuchDataException("Error");
		}
		return it->second;
	}
}

void	CameraDataBuffer::WhatCamerasRequired(std::set<int>& Cameras) const 
{
	Cameras.clear();
	for( size_t i = 0 ; i < m_RequestedCameras.size(); ++i )
	{
		if( m_CameraData.find( m_RequestedCameras[i] ) == m_CameraData.end() ||
			m_CameraImages.find( m_RequestedCameras[i] ) == m_CameraImages.end() )
		{
			Cameras.insert( m_RequestedCameras[i] );
		}
	}
}