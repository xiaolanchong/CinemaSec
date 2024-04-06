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
#include "stdafx.h"
using std::max;
using std::min;

#include <boost/shared_ptr.hpp>
#include "ImageEncoder.h"

using namespace Gdiplus;
//=====================================================================================//
//                                 class ImageEncoder                                  //
//=====================================================================================//
ImageEncoder::ImageEncoder(LONG lQuality) : m_lQuality(lQuality)
{
	bool res = GetEncoderClsid(L"image/jpeg", &m_clsidEncoder);
	if( !res )
	{
		throw ImageEncoderException("Failed to get a suitable encoder");
	}
	m_encoderParameters.Count = 1;
	m_encoderParameters.Parameter[0].Guid = EncoderQuality;
	m_encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	m_encoderParameters.Parameter[0].NumberOfValues = 1;
	m_encoderParameters.Parameter[0].Value = &m_lQuality;
}

ImageEncoder::~ImageEncoder()
{
}

bool ImageEncoder::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return false;  // Failure

	BYTE * RawArray = new BYTE[size];
	if(RawArray == NULL)
		return false;  // Failure
	pImageCodecInfo = (ImageCodecInfo*)RawArray;

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			delete [] RawArray;
			return true;  // Success
		}    
	}
	delete [] RawArray;
	return false;  // Failure
}

void	ImageEncoder::Encode( const BITMAPINFO* pBmpInfo, const void* pBytes, std::vector<BYTE>& EncImage, bool bResize )
{
	Gdiplus::Image* pRawResultImage = Gdiplus::Bitmap::FromBITMAPINFO( pBmpInfo, (void*)pBytes  );
//	Bitmap bmp( pBmpInfo, (void*)pBytes );
	if( !pRawResultImage || pRawResultImage->GetLastStatus() != Gdiplus::Ok) 
		throw ImageEncoderInvalidImage("Invalid image");
	boost::shared_ptr<Gdiplus::Image> pResultImage ( pRawResultImage );
	if(bResize)
	{
		Gdiplus::Image*  pTempThumbnailImage = pResultImage->GetThumbnailImage( pBmpInfo->bmiHeader.biWidth/2, 
																				pBmpInfo->bmiHeader.biHeight/2,
																				NULL, NULL );
		if( pTempThumbnailImage )
		{
			pResultImage.reset();
			pResultImage = boost::shared_ptr<Gdiplus::Image>( pTempThumbnailImage );
		}
	}
	CComPtr<IStream> pStream;
	HRESULT hr = CreateStreamOnHGlobal( 0, TRUE, &pStream  );
	if( hr != S_OK )
		throw ImageEncoderException("System error");
	Gdiplus::Status stat = pResultImage->Save( pStream, &m_clsidEncoder, &m_encoderParameters );
	if( Gdiplus::Ok != stat ) 
	{
		throw ImageEncoderEncodeError("Encoder error");
	}
	HGLOBAL hMem;
	hr = GetHGlobalFromStream( pStream, &hMem );
	if( hr != S_OK )
		throw ImageEncoderException("System error");
	const BYTE* pData = (const BYTE*)GlobalLock( hMem );
	if( !pData ) 
	{
		throw ImageEncoderEncodeError("Memory allocation error");
	}
	size_t nSize = GlobalSize( hMem );
	EncImage.assign(  pData, pData + nSize );
	GlobalUnlock(hMem);
	pStream.Release();
}

IStream*	ImageEncoder::Load( const void* pData, size_t nSize )
{
	HGLOBAL hMem = GlobalAlloc( GMEM_FIXED, nSize );
	memcpy( (void*)hMem, pData, nSize); 
	IStream* pStream;
	HRESULT hr = CreateStreamOnHGlobal( hMem, TRUE, &pStream  );
	if( hr != S_OK )
		throw ImageEncoderException("System error");
	return pStream;
}