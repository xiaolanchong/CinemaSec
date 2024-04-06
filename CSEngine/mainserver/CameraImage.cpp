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



#include "stdafx.h"
#include <algorithm>
using std::max;
using std::min;
#include "CameraImage.h"

//======================================================================================//
//                                  class CameraImage                                   //
//======================================================================================//

bool CameraImage::Fill( const BITMAPINFO* pBmpInfo, const void* pBytes )
{
	if( pBmpInfo->bmiHeader.biCompression != BI_RGB)
	{
		if(  !m_pDecompressor.get() )
		{
			const BITMAPINFOHEADER& bih = pBmpInfo->bmiHeader;
			try
			{
				m_pDecompressor = std::auto_ptr<ImageDecompressor> ( new ImageDecompressor( bih, bih.biWidth, bih.biHeight, 24 ) );
				memcpy( &m_biTemp.bmiHeader, &m_pDecompressor->GetDstFormat(), sizeof(BITMAPINFOHEADER) );
			}
			catch(ImageDecompressorException)
			{
				//pThis->m_pDebug.PrintW(IDebugOutput::mt_error, L"Camera %d - Failed to create ImageDecompressor");
				return false;
			}
		}
		if( m_pDecompressor.get() )
		{
			bool res = m_pDecompressor->Decompress( (const BYTE*)pBytes, m_DecompressedImage );
			if(!res)
			{
				//pThis->m_pDebug.PrintW(IDebugOutput::mt_error, L"Camera %d - Failed to decompress w/ ImageDecompressor");
				return false;
			}
			pBytes		= &m_DecompressedImage[0];
			pBmpInfo	= &m_biTemp;
		}
	}
#ifdef USE_JPEG
	try
	{
		ImageEncoder	ImgEnc;
		ImgEnc.Encode( pBmpInfo, pBytes, m_ImageBuffer );
	}
	catch ( ImageEncoderException& )
	{
		m_ImageBuffer.clear();
		return false;
	}
#else
	m_ImageBuffer.clear();
	m_ImageBuffer.insert( m_ImageBuffer.end(), (const BYTE*)pBmpInfo, (const BYTE*)pBmpInfo + sizeof(BITMAPINFOHEADER) );
	int nBitCount = pBmpInfo->bmiHeader.biBitCount;
	if( nBitCount <= 8 && nBitCount > 0)
	{
		int nImageSize = pBmpInfo->bmiHeader.biSizeImage;
		m_ImageBuffer.insert( m_ImageBuffer.end(), (const BYTE*)pBmpInfo->bmiColors, (const BYTE*)pBmpInfo->bmiColors + sizeof(RGBQUAD) * (1 << nBitCount)  );
	}
	m_ImageBuffer.insert( m_ImageBuffer.end(), (const BYTE*)pBytes, (const BYTE*)pBytes + pBmpInfo->bmiHeader.biSizeImage );
#endif
	EndProcessing();
	return true;
}

bool CameraImage::Fill( const QImage& CamImage )
{
#ifdef USE_JPEG
	try
	{
		BITMAPINFO		bi; 
		memset( &bi, 0, sizeof(BITMAPINFO) );
		BITMAPINFOHEADER& bih = bi.bmiHeader;
		bih.biBitCount		= 32;
		bih.biCompression	= BI_RGB;
		bih.biHeight		= CamImage.height();
		bih.biWidth			= CamImage.width();
		bih.biSize			= sizeof(BITMAPINFOHEADER);
		bih.biSizeImage		= CamImage.size() * sizeof(RGBQUAD)/sizeof(BYTE);
		bih.biPlanes		= 1;

		QImage newImage( CamImage );
		if( DOWNWARD_AXIS_Y )
			for( int i=0; i < CamImage.height()/2; ++i )
			{
				int nOff1 = CamImage.width()*i;
				int nOff2 = (CamImage.height() - i - 1)*CamImage.width();
				memcpy( &newImage[ nOff1 ], &CamImage[ nOff2 ],	sizeof(RGBQUAD) * CamImage.width() );
				memcpy( &newImage[ nOff2 ], &CamImage[ nOff1 ],	sizeof(RGBQUAD) * CamImage.width() );
			}

		const void*		pBytes = &newImage[0];
		ImageEncoder	ImgEnc;
		ImgEnc.Encode( &bi, pBytes, m_ImageBuffer, true );
	}
	catch ( ImageEncoderException& )
	{
		m_ImageBuffer.clear();
		return false;
	}
	EndProcessing();
	return true;
#else
#error "use only jpeg compression"
#endif
}