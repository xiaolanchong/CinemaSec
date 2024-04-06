//=====================================================================================//
//                                                                                     //
//                                       CSChair                                       //
//                                                                                     //
//                              Copyright by ElVEES 2005                               //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Description:                                                                      //
//                                                                                     //
//   -------------------------------------------------------------------------------   //
//   Author: Eugene Gorbachev                                                          //
//   Date:   01.02.2005                                                                //
//                                                                                     //
//=====================================================================================//
#include "stdafx.h"
#include "MemoryBackground.h"

//=====================================================================================//
//                               class MemoryBackground                                //
//=====================================================================================//
namespace MemoryBackground
{
	
struct OffsetPtr
{
	size_t	m_Offset;
	size_t	m_Size;

	BOOL Read( void* pOut, size_t Off, size_t ElNum, const BYTE*& pData )
	{
		m_Offset += Off;
		if( m_Offset > m_Size ) ALIB_THROW( _T( "Wrong image size" )); 
		memcpy( pOut, pData, Off );
		pData += Off;
		return ElNum > 0;
	}

	OffsetPtr(size_t sz):m_Offset(0), m_Size(sz) {}
};

//-------------------------------------------------------------------------------------------------
/** \brief Function loads an image with 'float' pixels from a 32-bits-per-pixel bitmap file.

\param  fname     the name of input file.
\param  image     the destination image.
\param  bInvertY  if true, then Y-axis will be inverted.
\return           Ok = true. */
//-------------------------------------------------------------------------------------------------
bool MemoryLoadFloatImage( const BYTE* pData, size_t DataLen, Arr2f & image, bool bInvertY )
{
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	bool             ok = true;

	try
	{
		OffsetPtr op(DataLen);
		image.clear();

		ASSERT( (sizeof(float) == sizeof(DWORD)) && (sizeof(float) == 4) );
		ALIB_ASSERT( op.Read( &(bf.bfType), sizeof(bf.bfType), 1, pData ) == 1 );
		ALIB_ASSERT( op.Read( &(bf.bfSize), sizeof(bf.bfSize), 1, pData ) == 1 );
		ALIB_ASSERT( op.Read( &(bf.bfReserved1), sizeof(bf.bfReserved1), 1, pData ) == 1 );
		ALIB_ASSERT( op.Read( &(bf.bfReserved2), sizeof(bf.bfReserved2), 1, pData ) == 1 );
		ALIB_ASSERT( op.Read( &(bf.bfOffBits), sizeof(bf.bfOffBits), 1, pData ) == 1 );
		ALIB_ASSERT( op.Read( &bi, sizeof(bi), 1, pData ) == 1 );

		if (!((bf.bfType == 0x4D42) && (bi.biCompression == BI_RGB) && ((bi.biBitCount >= 24) || (bi.biBitCount == 8))))
			ALIB_THROW( _T("Expects 32-,24-,8-bits uncompressed bitmap") );
		image.resize( bi.biWidth, bi.biHeight );

		// Read file scan by scan.
		for (int y = 0; y < bi.biHeight; y++)
		{
			float * p = image.row_begin( bInvertY ? (bi.biHeight-1-y) : y );
			ALIB_ASSERT( op.Read( p, 4*bi.biWidth, 1, pData ) == 1);
		}

		// Check pixels.
		for (int i = 0, n = image.size(); i < n; i++)
		{
			if (_isnan( (double)(image[i]) ) || !_finite( (double)(image[i]) ))
				ALIB_THROW( _T("Image is not the one with 'float' pixels") );
		}
	}
	catch (std::exception & /*e*/)
	{
		image.clear();
	//	alib::ErrorMessage( _T("Failed to load image"), _T("reason:"), (LPCTSTR)CString( e.what() ), fname );
		ok = false;
	}
	catch (...)
	{
		image.clear();
	//	alib::ErrorMessage( _T("Failed to load image"), ALIB_UNSUPPORTED_EXCEPTION, fname );
		ok = false;
	}

	return ok;
}


static BOOL WriteSimulate(const void* pOut, size_t Off, size_t ElNum, std::vector<BYTE>& pData )
{
	pData.insert( pData.end(), (BYTE*) pOut, (BYTE*) pOut + Off * ElNum );
	return TRUE;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function saves an image with 'float' pixels as a 32-bits-per-pixel bitmap file.

\param  fname     the name of output file.
\param  image     the image to be saved.
\param  bInvertY  if true, then Y-axis will be inverted.
\return           Ok = true. */
//-------------------------------------------------------------------------------------------------
bool MemorySaveFloatImage( std::vector<BYTE>& Arr, const Arr2f & image, bool bInvertY )
{
	bool   ok = true;
//	OffsetPtr op;
	std::vector<BYTE> & file = Arr;

	try
	{
		BITMAPFILEHEADER bf;
		BITMAPINFOHEADER bi;

		memset( &bi, 0, sizeof(bi) );
		memset( &bf, 0, sizeof(bf) );

		bi.biSize = sizeof(bi); 
		bi.biWidth = image.width();
		bi.biHeight = image.height(); 
		bi.biPlanes = 1; 
		bi.biBitCount = 32; 
		bi.biCompression = BI_RGB;
		bi.biSizeImage = 4 * bi.biWidth * bi.biHeight;

		bf.bfType = 0x4D42;
		bf.bfSize = 14 + sizeof(BITMAPINFOHEADER) + bi.biSizeImage;
		bf.bfOffBits = 14 + sizeof(BITMAPINFOHEADER);

		// Check pixels.
		for (int i = 0, n = image.size(); i < n; i++)
		{
			if (_isnan( (double)(image[i]) ))
				ALIB_THROW( _T( "Image is not the one with 'float' pixels" ));
		}

		ASSERT( (sizeof(float) == sizeof(DWORD)) && (sizeof(float) == 4) );
		ALIB_ASSERT( WriteSimulate( &(bf.bfType), sizeof(bf.bfType), 1, file ) == 1 );
		ALIB_ASSERT( WriteSimulate( &(bf.bfSize), sizeof(bf.bfSize), 1, file ) == 1 );
		ALIB_ASSERT( WriteSimulate( &(bf.bfReserved1), sizeof(bf.bfReserved1), 1, file ) == 1 );
		ALIB_ASSERT( WriteSimulate( &(bf.bfReserved2), sizeof(bf.bfReserved2), 1, file ) == 1 );
		ALIB_ASSERT( WriteSimulate( &(bf.bfOffBits), sizeof(bf.bfOffBits), 1, file ) == 1 );
		ALIB_ASSERT( WriteSimulate( &bi, sizeof(bi), 1, file ) == 1 );

		for (int y = 0; y < bi.biHeight; y++)
		{
			const float * p = image.row_begin( bInvertY ? (bi.biHeight-1-y) : y );
			ALIB_ASSERT( WriteSimulate( p, 4*bi.biWidth, 1, file ) == 1 );
		}
	}
	catch (std::exception & e)
	{
		alib::ErrorMessage( _T("Failed to save image"), _T("reason:"), (LPCTSTR)CString( e.what() ) );
		ok = false;
	}
	catch (...)
	{
		alib::ErrorMessage( _T("Failed to save image"), ALIB_UNSUPPORTED_EXCEPTION );
		ok = false;
	}

	return ok;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function loads the static background (empty hall) from a file.

\param  fileName    the name of source file.
\param  background  array of destination backgrounds of all subframes.
\param  nSubFrames  the number of subframes.
\return             Ok = true. */
//-------------------------------------------------------------------------------------------------
static bool MemoryLoadBackgroundImage( const std::vector<BYTE>& Data, Arr2f * background, int nSubFrames )
{
	if (!ALIB_IS_LIMITED( nSubFrames, 1, MAX_SUBFRAME_NUM ))
		return false;

	Arr2f image;
	if (!MemoryLoadFloatImage( &Data[0], Data.size(), image, INVERT_AXIS_Y ))
		return false;

	int W = image.width();
	int H = image.height()/nSubFrames;

	// Weak check for consistency.
	if ((W == 0) || (H == 0) || (((image.height())%nSubFrames) != 0))
		return false;

	// Obtain background subframes from the large image.
	for (int k = 0; k < nSubFrames; k++)
	{
		(background[k]).resize( W, H );
		std::copy( image.begin()+(k*W*H), image.begin()+((k+1)*W*H), (background[k]).begin() );
	}
	return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function saves accumulated background (empty hall) to a file.

\param  fileName     the name of destination file.
\param  backgrounds  array of source backgrounds of all subframes.
\return              Ok = true. */
//-------------------------------------------------------------------------------------------------
static bool MemorySaveBackgroundImage( LPCTSTR fileName, const FltImgArr & backgrounds )
{
	if ((fileName == 0) || !(alib::IsLimited( (int)(backgrounds.size()), 1, MAX_SUBFRAME_NUM )))
		return false;

	int   W = (backgrounds[0]).width();
	int   H = (backgrounds[0]).height();
	Arr2f image( W, H*(int)(backgrounds.size()) );

	// Copy background subframes to their positions in the large image.
	for (int k = 0; k < (int)(backgrounds.size()); k++)
	{
		if (!(alib::AreDimensionsEqual( backgrounds[0], backgrounds[k] )))
			return false;

		std::copy( (backgrounds[k]).begin(), (backgrounds[k]).end(), image.begin()+(k*W*H) );
	}

	return SaveFloatImage( fileName, image, INVERT_AXIS_Y );
}	
	
}
