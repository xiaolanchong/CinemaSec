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

//! gdiplus ��������������� � JPEG
class ImageEncoder
{
	//! �������������
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

	//! ��������� ������
	Gdiplus::EncoderParameters m_encoderParameters;
	//! ������� GUID ����������
	//! \param format 
	//! \param pClsid 
	//! \return �����/������
	bool GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

	//! �������� ������
	LONG	m_lQuality;
	//! GUID ����������
	CLSID	m_clsidEncoder;
public:
	ImageEncoder(LONG lQuality = 30);
	//! ����������� �����������
	//! \param pBmpImfo �������� ���������
	//! \param pBytes �������� ������
	//! \param EncImage �������� �����������
	//! \param bResize ����� ������� ����������� � 2 ����?
	void		Encode( const BITMAPINFO* pBmpImfo, const void* pBytes, std::vector<BYTE>& EncImage, bool bResize = false);
	//! �������� IStream �� ����� �������
	//! \param pData ������ �����������
	//! \param nSize ������ ������
	//! \return ��������� IStream
	IStream*	Load( const void* pData, size_t nSize ); 
	virtual ~ImageEncoder();
};

#endif //__IMAGE_ENCODER_H_INCLUDED_1655218564033627__