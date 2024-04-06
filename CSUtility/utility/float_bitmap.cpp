/****************************************************************************
  float_bitmap.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Function loads an image with 'float' pixels from a 32-bits-per-pixel bitmap file.

  \param  fname            the name of input file.
  \param  image            the destination image.
  \param  doDownwardAxisY  if true, then Y-axis will be directed downward.
  \return                  Ok = true. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API bool LoadFloatImage( LPCWSTR fname, Arr2f & image, bool doDownwardAxisY )
{
  BITMAPFILEHEADER bf;
  BITMAPINFOHEADER bi;
  bool             ok = true;
  FILE *           file = 0;

  try
  {
    file = _tfopen( fname, _T("rb") );
    image.clear();
    if (file == 0)
      ALIB_THROW( _T("Failed to open image file") );

    ASSERT( (sizeof(float) == sizeof(DWORD)) && (sizeof(float) == 4) );
    ALIB_ASSERT( fread( &(bf.bfType), sizeof(bf.bfType), 1, file ) == 1 );
    ALIB_ASSERT( fread( &(bf.bfSize), sizeof(bf.bfSize), 1, file ) == 1 );
    ALIB_ASSERT( fread( &(bf.bfReserved1), sizeof(bf.bfReserved1), 1, file ) == 1 );
    ALIB_ASSERT( fread( &(bf.bfReserved2), sizeof(bf.bfReserved2), 1, file ) == 1 );
    ALIB_ASSERT( fread( &(bf.bfOffBits), sizeof(bf.bfOffBits), 1, file ) == 1 );
    ALIB_ASSERT( fread( &bi, sizeof(bi), 1, file ) == 1 );

    if (!((bf.bfType == 0x4D42) && (bi.biCompression == BI_RGB) && (bi.biBitCount == 32)))
      ALIB_THROW( _T("Expects 32-bits uncompressed bitmap") );
    image.resize( bi.biWidth, bi.biHeight );

    // Read file scan by scan.
    for (int y = 0; y < bi.biHeight; y++)
    {
      float * p = image.row_begin( doDownwardAxisY ? (bi.biHeight-1-y) : y );
      ALIB_ASSERT( fread( p, 4*bi.biWidth, 1, file ) == 1);
    }

    // Check pixels (weak test).
    for (int i = 0, n = image.size(); i < n; i++)
    {
      if (_isnan( (double)(image[i]) ) || !_finite( (double)(image[i]) ))
        ALIB_THROW( _T("Image is not the one with 'float' pixels") );
    }
  }
  catch (std::exception & e)
  {
    image.clear();
    alib::ErrorMessage( _T("Failed to load image"), _T("reason:"), (LPCWSTR)CString( e.what() ), fname );
    ok = false;
  }

  if (file != 0)
    fclose( file );
  return ok;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function saves an image with 'float' pixels as a 32-bits-per-pixel bitmap file.

  \param  fname            the name of output file.
  \param  image            the image to be saved.
  \param  doDownwardAxisY  if true, then Y-axis will be directed downward.
  \return                  Ok = true. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API bool SaveFloatImage( LPCWSTR fname, const Arr2f & image, bool doDownwardAxisY )
{
  bool   ok = true;
  FILE * file = 0;

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
        ALIB_THROW( _T("Image is not the one with 'float' pixels") );
    }

    file = _tfopen( fname, _T("wb") );
    if (file == 0)
      ALIB_THROW( _T("Failed to open image file") );

    ASSERT( (sizeof(float) == sizeof(DWORD)) && (sizeof(float) == 4) );
    ALIB_ASSERT( fwrite( &(bf.bfType), sizeof(bf.bfType), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &(bf.bfSize), sizeof(bf.bfSize), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &(bf.bfReserved1), sizeof(bf.bfReserved1), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &(bf.bfReserved2), sizeof(bf.bfReserved2), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &(bf.bfOffBits), sizeof(bf.bfOffBits), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &bi, sizeof(bi), 1, file ) == 1 );

    for (int y = 0; y < bi.biHeight; y++)
    {
      const float * p = image.row_begin( doDownwardAxisY ? (bi.biHeight-1-y) : y );
      ALIB_ASSERT( fwrite( p, 4*bi.biWidth, 1, file ) == 1 );
    }
  }
  catch (std::exception & e)
  {
    alib::ErrorMessage( _T("Failed to save image"), _T("reason:"), (LPCWSTR)CString( e.what() ), fname );
    ok = false;
  }

  if (file != 0)
    fclose( file );
  return ok;
}

