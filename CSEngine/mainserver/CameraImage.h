//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
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

//! класс обработки изображения
struct CameraImage
{
	//! состояние изображения
	enum 
	{
		//! начальное
		is_none,
		//! запрошено
		is_requested,
		//! обработка
		is_processing,
		//! готов
		is_ready
	};

	//! декомпрессор изображения
	std::auto_ptr<ImageDecompressor>	m_pDecompressor;
	//! раздатое изображение
	std::vector<BYTE>					m_DecompressedImage;
	//! заголовок
	BITMAPINFO							m_biTemp;
	//! состояние
	LONG		m_is;
	CameraImage() : m_is(is_none){}

	//! запросить изображение
	void RequestImage()		{ InterlockedExchange( &m_is, is_requested ); /*InterlockedExchange( &m_WasRequested, 1L );*/	}

	//! закончить обработку
	void EndProcessing()	{ InterlockedExchange( &m_is, is_ready);		}

	//! требуется изображение
	//! \return да/нет
	bool IsRequired()	const	{ return m_is == is_requested;	}

	//! изображение готово?
	//! \return да/нет
	bool IsReady()		const	{ return m_is == is_ready;		}

	//! установить изображение (сжатие)
	//! \param pBmpInfo заголовок
	//! \param pBytes данные
	//! \return успех/ошибка
	bool Fill( const BITMAPINFO* pBmpInfo, const void* pBytes );

	//! установить изображение в формате алгоритма (сжатие)
	//! \param CamImage данные
	//! \return успех/ошибка
	bool Fill( const QImage& CamImage );

	//! вернуть изображение
	//! \return изображение
	const std::vector<BYTE>& GetImage() const { return m_ImageBuffer; }

	//! буфер для изображения
	std::vector<BYTE>						m_ImageBuffer;
	~CameraImage() 
	{
	}
};


#endif // _CAMERA_IMAGE_4908865686668454_