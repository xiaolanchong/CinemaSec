//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright� ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Processing image from camera for the photo statistics
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 21.07.2005
//                                                                                      //
//======================================================================================//
#ifndef _CAMERA_IMAGE_4908865686668454_
#define _CAMERA_IMAGE_4908865686668454_

#include <boost\shared_ptr.hpp>

#define USE_JPEG
#ifdef USE_JPEG
#include "ImageEncoder.h"
#endif

#include <vector>
#include <utility>
#include "..\..\CSChair\Grabber\ImageDecompressor.h"

//======================================================================================//
//                                  class CameraImage                                   //
//======================================================================================//

//! ����� ��������� �����������
struct CameraImage
{
	//! ��������� �����������
	enum 
	{
		//! ���������
		is_none,
		//! ���������
		is_requested,
		//! ���������
		is_processing,
		//! �����
		is_ready
	};

	//! ������������ �����������
	std::auto_ptr<ImageDecompressor>	m_pDecompressor;
	//! �������� �����������
	std::vector<BYTE>					m_DecompressedImage;
	//! ���������
	BITMAPINFO							m_biTemp;
	//! ���������
	LONG		m_is;
	CameraImage() : m_is(is_none){}

	//! ��������� �����������
	void RequestImage()		{ InterlockedExchange( &m_is, is_requested ); /*InterlockedExchange( &m_WasRequested, 1L );*/	}

	//! ��������� ���������
	void EndProcessing()	{ InterlockedExchange( &m_is, is_ready);		}

	//! ��������� �����������
	//! \return ��/���
	bool IsRequired()	const	{ return m_is == is_requested;	}

	//! ����������� ������?
	//! \return ��/���
	bool IsReady()		const	{ return m_is == is_ready;		}

	//! ���������� ����������� (������)
	//! \param pBmpInfo ���������
	//! \param pBytes ������
	//! \return �����/������
	bool Fill( const BITMAPINFO* pBmpInfo, const void* pBytes );

	//! ���������� ����������� � ������� ��������� (������)
	//! \param CamImage ������
	//! \return �����/������
	bool Fill( const QImage& CamImage );

	//! ������� �����������
	//! \return �����������
	const std::vector<BYTE>& GetImage() const { return m_ImageBuffer; }

	//! ����� ��� �����������
	std::vector<BYTE>						m_ImageBuffer;
	~CameraImage() 
	{
	}
};


#endif // _CAMERA_IMAGE_4908865686668454_