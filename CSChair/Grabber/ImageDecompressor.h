//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Image helper decompressor for non-rgb formats (primarily for YUY2),
//	use msyuv.dll
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 22.03.2005
//                                                                                      //
//======================================================================================//
#ifndef _IMAGE_DECOMPRESSOR_4303259393947202_
#define _IMAGE_DECOMPRESSOR_4303259393947202_
//======================================================================================//
//                               class ImageDecompressor                                //
//======================================================================================//
#include <BOOST/shared_ptr.hpp>
#include "../COMMON/Exception.h"

MACRO_EXCEPTION( ImageDecompressorException, CommonException ) 

#ifndef DIBSIZE
// DIBSIZE calculates the number of bytes required by an image

#define WIDTHBYTES(bits) ((DWORD)(((bits)+31) & (~31)) / 8)
#define DIBWIDTHBYTES(bi) (DWORD)WIDTHBYTES((DWORD)(bi).biWidth * (DWORD)(bi).biBitCount)
#define _DIBSIZE(bi) (DIBWIDTHBYTES(bi) * (DWORD)(bi).biHeight)
#define DIBSIZE(bi) ((bi).biHeight < 0 ? (-1)*(_DIBSIZE(bi)) : _DIBSIZE(bi))
#endif

//! \brief преобразование изображения из различных форматов с сохранением размеров с помощью VFW
//! \version 1.0
//! \date 03-22-2005
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \todo 
//! \bug 
//!
class ImageDecompressor
{
	//! дескриптор кодека
	HIC					m_hDec;
	//! заголовок входного изображения
	BITMAPINFOHEADER	m_bihIn, 
	//! заголовок выходного изображения
						m_bihOut;
public:
//! получить заголовок исходного изображения
//! \return исходный заголовок
	const BITMAPINFOHEADER&	GetSrcFormat() const { return m_bihIn; }

//! получить заголовок требуемого изображения
//! \return конечный заголовок
	const BITMAPINFOHEADER&	GetDstFormat() const { return m_bihOut; }

//! создать декомпрессор
//! \param bihIn входной  формат
//! \param dwWidth ширины изображения
//! \param dwHeight высота изображения
//! \param wBitCount кол-во бит на пиксель в выходном изображении
	ImageDecompressor(const BITMAPINFOHEADER& bihIn, DWORD dwWidth, DWORD dwHeight, WORD wBitCount )
	{
		memcpy( &m_bihIn, &bihIn, sizeof( BITMAPINFOHEADER) );
		memset( &m_bihOut, 0, sizeof(BITMAPINFOHEADER) );
		m_bihOut.biSize			= sizeof( BITMAPINFOHEADER );
		m_bihOut.biWidth		= dwWidth;
		m_bihOut.biHeight		= dwHeight;
		m_bihOut.biBitCount		= wBitCount;
		m_bihOut.biPlanes		= 1;
		m_bihOut.biCompression	= BI_RGB;
		m_bihOut.biSizeImage	= DIBSIZE( m_bihOut );

		m_hDec		= ICOpen( ICTYPE_VIDEO, m_bihIn.biCompression/*, &m_bihIn, &m_bihOut */, ICMODE_DECOMPRESS );

		if(!m_hDec || ICDecompressQuery(m_hDec, &m_bihIn, &m_bihOut) != ICERR_OK)
		{
			if(m_hDec)
				ICClose(m_hDec);

			m_hDec = ICLocate(ICTYPE_VIDEO, m_bihIn.biCompression, &m_bihIn, &m_bihOut, ICMODE_DECOMPRESS);
		}

		if( !m_hDec ) throw ImageDecompressorException("Error");
		LRESULT dwRes = ::ICDecompressBegin(m_hDec, &bihIn, &m_bihOut);
		if( dwRes != ICERR_OK )
		{
			::ICClose(m_hDec);
			throw ImageDecompressorException("Error");
		}
	}

	~ImageDecompressor()
	{
		::ICDecompressEnd(m_hDec);
		::ICClose(m_hDec);
	}

	//! преобразование изображения
	//! \param pSrcImage исходное изображение
	//! \param DstImage выходное изображение
	//! \return true - успех, false - ошибка преобразования
	bool	Decompress( const BYTE* pSrcImage, std::vector<BYTE>& DstImage)
	{
		if( !m_hDec ) return false;
		DstImage.resize( m_bihOut.biSizeImage );
		DWORD dwRes = ICDecompress( m_hDec, 0, &m_bihIn, (void*)pSrcImage, &m_bihOut, &DstImage[0]  );
		return dwRes == ICERR_OK;
	}
};

#endif // _IMAGE_DECOMPRESSOR_4303259393947202_