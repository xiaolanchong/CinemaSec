/****************************************************************************
  byte_bitmap.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//void MyInitColorGrayPalette( RGBQUAD * palette, int size, bool bColor16 );

//=================================================================================================
/** \brief Analog of Windows RGBQUAD structure. */
//=================================================================================================
#ifdef WIN32
struct RgbQuad : public RGBQUAD
{
#else
struct RgbQuad
{
  unsigned char rgbBlue; 
  unsigned char rgbGreen; 
  unsigned char rgbRed; 
  unsigned char rgbReserved;
#endif // WIN32

  RgbQuad()
  {
    assert( sizeof(unsigned char) == 1 );
  }
};


//-------------------------------------------------------------------------------------------------
/** \brief Function loads BMP image as a gray-scaled bitmap with one-byte pixels.

  \param  fname     the name of input file.
  \param  image     the destination image.
  \param  bInvertY  if true, then Y-axis will be inverted.
  \return           Ok = true. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API bool LoadByteImage( LPCWSTR fname, Arr2ub & image, bool bInvertY )
{
  BITMAPFILEHEADER            bf;
  BITMAPINFOHEADER            bi;
  std::vector<unsigned char>  scan;
  bool                        ok = true;
  FILE *                      file = 0;
  
  try
  {
    file = _tfopen( fname, _T("rb") );
    image.clear();
    if (file == 0)
      ALIB_THROW( _T("Failed to open image file") );
    
    ALIB_ASSERT( fread( &(bf.bfType), sizeof(bf.bfType), 1, file ) == 1 );
    ALIB_ASSERT( fread( &(bf.bfSize), sizeof(bf.bfSize), 1, file ) == 1 );
    ALIB_ASSERT( fread( &(bf.bfReserved1), sizeof(bf.bfReserved1), 1, file ) == 1 );
    ALIB_ASSERT( fread( &(bf.bfReserved2), sizeof(bf.bfReserved2), 1, file ) == 1 );
    ALIB_ASSERT( fread( &(bf.bfOffBits), sizeof(bf.bfOffBits), 1, file ) == 1 );
    ALIB_ASSERT( fread( &bi, sizeof(bi), 1, file ) == 1 );

    if (!((bf.bfType == 0x4D42) && (bi.biCompression == BI_RGB) && ((bi.biBitCount >= 24) || (bi.biBitCount == 8))))
      ALIB_THROW( _T("Expects 32-,24-,8-bits uncompressed bitmap") );
    
    if (bi.biBitCount >= 24)                     // true-color bitmap
    {
      int nByte = (bi.biBitCount == 32) ? 4 : 3; // bytes per pixel

      scan.resize( ((nByte*bi.biWidth + 3) & ~3), 0 );
      image.resize( bi.biWidth, bi.biHeight );

      // Read file scan by scan.
      for(int y = 0; y < bi.biHeight; y++)
      {
        unsigned char * pDst = image.row_begin( bInvertY ? (bi.biHeight-1-y) : y );
        unsigned char * pSrc = &(*(scan.begin()));

        ALIB_ASSERT( fread( &(*(scan.begin())), (int)(scan.size()*sizeof(unsigned char)), 1, file ) == 1);

        for(int k = 0; k < bi.biWidth; k++)
        {
          unsigned int b = pSrc[0];
          unsigned int g = pSrc[1];
          unsigned int r = pSrc[2];
          (*pDst) = (unsigned char)((r*306 + g*601 + b*117 + 512) >> 10);
          pDst++;
          pSrc += nByte;
        }
      }
    }
    else if (bi.biBitCount == 8)                 // 8-bits bitmap
    {
      RgbQuad palette[256];

      scan.resize( ((bi.biWidth + 3) & ~3), 0 );
      image.resize( bi.biWidth, bi.biHeight );
      memset( palette, 0, sizeof(palette) );
            
      ALIB_ASSERT( fread( palette, sizeof(palette), 1, file ) == 1 );

      // Read file scan by scan.
      for (int y = 0; y < bi.biHeight; y++)
      {
        unsigned char * pDst = image.row_begin( bInvertY ? (bi.biHeight-1-y) : y );
        unsigned char * pSrc = &(*(scan.begin()));

        ALIB_ASSERT( fread( &(*(scan.begin())), (int)(scan.size()*sizeof(unsigned char)), 1, file ) == 1);

        for (int k = 0; k < bi.biWidth; k++)
        {
          RgbQuad &    color = palette[ (unsigned int)(*pSrc) ];
          unsigned int b = color.rgbBlue;
          unsigned int g = color.rgbGreen;
          unsigned int r = color.rgbRed;

          (*pDst) = (unsigned char)((r*306 + g*601 + b*117 + 512) >> 10);
          pDst++;
          pSrc++;
        }
      }
    }
  }
  catch (std::exception & e)
  {
    image.clear();
    alib::ErrorMessage( _T("Failed to load image"), _T("reason:"), (LPCWSTR)CString( e.what() ), fname );
    ok = false;
  }
  catch (...)
  {
    image.clear();
    alib::ErrorMessage( _T("Failed to load image"), ALIB_UNSUPPORTED_EXCEPTION, fname );
    ok = false;
  }

  if (file != 0)
    fclose( file );
  return ok;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function saves image as a byte-per-pixel bitmap in the file.

  \param  fname     the name of output file.
  \param  image     the image to be saved.
  \param  bInvertY  if true, then Y-axis will be inverted.
  \param  bColor16  if true, then the first 16 palette entries will be filled by system colors.
  \return           Ok = true. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API bool SaveByteImage( LPCWSTR fname, const Arr2ub & image, bool bInvertY, bool bColor16 )
{
  bool   ok = true;
  FILE * file = 0;

  try
  {
    BITMAPFILEHEADER           bf;
    BITMAPINFOHEADER           bi;
    RgbQuad                    palette[256];
    int                        scanLength = ((image.width()+3) & ~3);
    std::vector<unsigned char> scan( scanLength );

    memset( &bi, 0, sizeof(bi) );
    memset( &bf, 0, sizeof(bf) );

    bi.biSize = sizeof(bi); 
    bi.biWidth = image.width(); 
    bi.biHeight = image.height(); 
    bi.biPlanes = 1; 
    bi.biBitCount = 8; 
    bi.biCompression = BI_RGB;
    bi.biSizeImage = sizeof(unsigned char) * scanLength * bi.biHeight;
    bi.biClrUsed = 256;
    bi.biClrImportant = 256;

    bf.bfType = 0x4D42;
    bf.bfSize = 14 + sizeof(BITMAPINFOHEADER) + sizeof(palette) + bi.biSizeImage;
    bf.bfOffBits = 14 + sizeof(BITMAPINFOHEADER) + sizeof(palette);

/*
    for (int k = 0; k < 256; k++)
    {
      (palette[k]).rgbRed = ((palette[k]).rgbGreen = ((palette[k]).rgbBlue = (unsigned char)k));
    }
*/
    MyInitColorGrayPalette( palette, ALIB_LengOf(palette), bColor16 );

    file = _tfopen( fname, _T("wb") );
    if (file == 0)
      ALIB_THROW( _T("Failed to open image file") );

    ALIB_ASSERT( fwrite( &(bf.bfType), sizeof(bf.bfType), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &(bf.bfSize), sizeof(bf.bfSize), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &(bf.bfReserved1), sizeof(bf.bfReserved1), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &(bf.bfReserved2), sizeof(bf.bfReserved2), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &(bf.bfOffBits), sizeof(bf.bfOffBits), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &bi, sizeof(bi), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( palette, sizeof(palette), 1, file ) == 1 );

    for (int y = 0; y < bi.biHeight; y++)
    {
      const unsigned char * p = image.row_begin( bInvertY ? (bi.biHeight-1-y) : y );
      std::copy( p, p+bi.biWidth, scan.begin() );
      ALIB_ASSERT( fwrite( &(*(scan.begin())), (int)(scanLength*sizeof(unsigned char)), 1, file ) == 1 );
    }
  }
  catch (std::exception & e)
  {
    alib::ErrorMessage( _T("Failed to save image"), _T("reason:"), (LPCWSTR)CString( e.what() ), fname );
    ok = false;
  }
  catch (...)
  {
    alib::ErrorMessage( _T("Failed to save image"), ALIB_UNSUPPORTED_EXCEPTION, fname );
    ok = false;
  }

  if (file != 0)
    fclose( file );
  return ok;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function makes negative image from specified one.

  \param  image  the image to be saved. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API void Negate( Arr2ub & image )
{
  for (Arr2ub::iterator it = image.begin(); it != image.end(); ++it)
  {
    (*it) = (unsigned char)(UCHAR_MAX - (*it));
  }
}


