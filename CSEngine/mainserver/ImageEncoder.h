//=====================================================================================//
//                                                                                     //
//                                      CinemaSec                                      //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
/*
		Helper image class for encoding with GDI+
                                                                                       */
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   07.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#ifndef __IMAGE_ENCODER_H_INCLUDED_1655218564033627__
#define __IMAGE_ENCODER_H_INCLUDED_1655218564033627__

#include <gdiplus.h>
#include "../../CSChair/common/Exception.h"

// last header
#include "../../CSChair/public/memleak.h"

MACRO_EXCEPTION(ImageEncoderException, CommonException)
MACRO_EXCEPTION(ImageEncoderInvalidImage, ImageEncoderException)
MACRO_EXCEPTION(ImageEncoderEncodeError, ImageEncoderException)
//=====================================================================================//
//                                 class ImageEncoder                                  //
//=====================================================================================//

//! gdiplus конвертирование в JPEG
class ImageEncoder
{
	//! инициализация
	class GdiPlusInit
	{
		//! cookie
		ULONG_PTR					gdiplusToken;
	public:
		GdiPlusInit()
		{
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			// Initialize GDI+.
			Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		}
		~GdiPlusInit()
		{
			Gdiplus::GdiplusShutdown(gdiplusToken);
		}
	}	m_GdiInit;

	//! параметры сжатия
	Gdiplus::EncoderParameters m_encoderParameters;
	//! вернуть GUID конвертора
	//! \param format 
	//! \param pClsid 
	//! \return успех/ошибка
	bool GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

	//! качество сжатия
	LONG	m_lQuality;
	//! GUID конвертора
	CLSID	m_clsidEncoder;
public:
	ImageEncoder(LONG lQuality = 30);
	//! конвертация изображения
	//! \param pBmpImfo исходный заголовок
	//! \param pBytes исходные данные
	//! \param EncImage выходное изображение
	//! \param bResize сжать размеры изображения в 2 раза?
	void		Encode( const BITMAPINFO* pBmpImfo, const void* pBytes, std::vector<BYTE>& EncImage, bool bResize = false);
	//! получить IStream на сыром массиве
	//! \param pData данные изображения
	//! \param nSize размер данных
	//! \return обернутый IStream
	IStream*	Load( const void* pData, size_t nSize ); 
	virtual ~ImageEncoder();
};

#endif //__IMAGE_ENCODER_H_INCLUDED_1655218564033627__