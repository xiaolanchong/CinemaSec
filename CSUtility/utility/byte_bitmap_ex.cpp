/****************************************************************************
  byte_bitmap_ex.cpp
  ---------------------
  begin     : Aug 2004
  modified  : 20 Sep 2005
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csutility
{

//-------------------------------------------------------------------------------------------------
/** \brief Function loads BMP image as a gray-scaled bitmap with one-byte pixels.

  \param  fname     the name of input file.
  \param  pImage    pointer to the image to be saved.
  \param  bInvertY  if true, then Y-axis will be inverted.
  \return           Ok = true. */
//-------------------------------------------------------------------------------------------------
bool LoadByteImageEx( LPCWSTR fname, Arr2ub * pImage, bool bInvertY )
{
  BITMAPFILEHEADER            bf;
  BITMAPINFOHEADER            bi;
  std::vector<ubyte>  scan;
  bool                        ok = true;
  FILE *                      file = 0;

  try
  {
    ALIB_ASSERT( pImage != 0 );
    file = _wfopen( fname, L"rb" );
    pImage->clear();
    if (file == 0)
      ALIB_THROW( L"Failed to open image file" );
    
    ALIB_ASSERT( fread( &(bf.bfType), sizeof(bf.bfType), 1, file ) == 1 );
    ALIB_ASSERT( fread( &(bf.bfSize), sizeof(bf.bfSize), 1, file ) == 1 );
    ALIB_ASSERT( fread( &(bf.bfReserved1), sizeof(bf.bfReserved1), 1, file ) == 1 );
    ALIB_ASSERT( fread( &(bf.bfReserved2), sizeof(bf.bfReserved2), 1, file ) == 1 );
    ALIB_ASSERT( fread( &(bf.bfOffBits), sizeof(bf.bfOffBits), 1, file ) == 1 );
    ALIB_ASSERT( fread( &bi, sizeof(bi), 1, file ) == 1 );

    if (!((bf.bfType == 0x4D42) && (bi.biCompression == BI_RGB) && ((bi.biBitCount >= 24) || (bi.biBitCount == 8))))
      ALIB_THROW( L"Expects 32-,24-,8-bits uncompressed bitmap" );
    
    if (bi.biBitCount >= 24)                     // true-color bitmap
    {
      int nByte = (bi.biBitCount == 32) ? 4 : 3; // bytes per pixel

      scan.resize( ((nByte*bi.biWidth + 3) & ~3), 0 );
      pImage->resize( bi.biWidth, bi.biHeight );

      // Read file scan by scan.
      for(int y = 0; y < bi.biHeight; y++)
      {
        ubyte * pDst = pImage->row_begin( bInvertY ? (bi.biHeight-1-y) : y );
        ubyte * pSrc = &(*(scan.begin()));

        ALIB_ASSERT( fread( &(*(scan.begin())), (int)(scan.size()*sizeof(ubyte)), 1, file ) == 1);

        for(int k = 0; k < bi.biWidth; k++)
        {
          uint b = pSrc[0];
          uint g = pSrc[1];
          uint r = pSrc[2];
          (*pDst) = (ubyte)((r*306 + g*601 + b*117 + 512) >> 10);
          pDst++;
          pSrc += nByte;
        }
      }
    }
    else if (bi.biBitCount == 8)                 // 8-bits bitmap
    {
      RGBQUAD palette[256];

      scan.resize( ((bi.biWidth + 3) & ~3), 0 );
      pImage->resize( bi.biWidth, bi.biHeight );
      memset( palette, 0, sizeof(palette) );
            
      ALIB_ASSERT( fread( palette, sizeof(palette), 1, file ) == 1 );

      // Read file scan by scan.
      for (int y = 0; y < bi.biHeight; y++)
      {
        ubyte * pDst = pImage->row_begin( bInvertY ? (bi.biHeight-1-y) : y );
        ubyte * pSrc = &(*(scan.begin()));

        ALIB_ASSERT( fread( &(*(scan.begin())), (int)(scan.size()*sizeof(ubyte)), 1, file ) == 1);

        for (int k = 0; k < bi.biWidth; k++)
        {
          RGBQUAD & color = palette[ (uint)(*pSrc) ];
          uint      b = color.rgbBlue;
          uint      g = color.rgbGreen;
          uint      r = color.rgbRed;

          (*pDst) = (ubyte)((r*306 + g*601 + b*117 + 512) >> 10);
          pDst++;
          pSrc++;
        }
      }
    }
  }
  catch (std::exception & e)
  {
    pImage->clear();
    alib::ErrorMessage( L"Failed to load image", L"reason:", (LPCWSTR)CString( e.what() ), fname );
    ok = false;
  }

  if (file != 0)
    fclose( file );
  return ok;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function saves image as a byte-per-pixel bitmap in the file.

  \param  fname     the name of output file.
  \param  pImage    pointer to the image to be saved.
  \param  bInvertY  if true, then Y-axis will be inverted.
  \param  bColor16  if true, then the first 16 palette entries will be filled by system colors.
  \return           Ok = true. */
//-------------------------------------------------------------------------------------------------
bool SaveByteImageEx( LPCWSTR fname, const Arr2ub * pImage, bool bInvertY, bool bColor16 )
{
  bool   ok = true;
  FILE * file = 0;

  try
  {
    ALIB_ASSERT( pImage != 0 );

    BITMAPFILEHEADER           bf;
    BITMAPINFOHEADER           bi;
    RGBQUAD                    palette[256];
    int                        scanLength = ((pImage->width()+3) & ~3);
    std::vector<ubyte> scan( scanLength );

    memset( &bi, 0, sizeof(bi) );
    memset( &bf, 0, sizeof(bf) );

    bi.biSize = sizeof(bi); 
    bi.biWidth = pImage->width(); 
    bi.biHeight = pImage->height(); 
    bi.biPlanes = 1; 
    bi.biBitCount = 8; 
    bi.biCompression = BI_RGB;
    bi.biSizeImage = sizeof(ubyte) * scanLength * bi.biHeight;
    bi.biClrUsed = 256;
    bi.biClrImportant = 256;

    bf.bfType = 0x4D42;
    bf.bfSize = 14 + sizeof(BITMAPINFOHEADER) + sizeof(palette) + bi.biSizeImage;
    bf.bfOffBits = 14 + sizeof(BITMAPINFOHEADER) + sizeof(palette);

    csutility::InitColorGrayPalette( palette, ALIB_LengOf(palette), bColor16 );

    file = _wfopen( fname, L"wb" );
    if (file == 0)
      ALIB_THROW( L"Failed to open image file" );

    ALIB_ASSERT( fwrite( &(bf.bfType), sizeof(bf.bfType), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &(bf.bfSize), sizeof(bf.bfSize), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &(bf.bfReserved1), sizeof(bf.bfReserved1), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &(bf.bfReserved2), sizeof(bf.bfReserved2), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &(bf.bfOffBits), sizeof(bf.bfOffBits), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( &bi, sizeof(bi), 1, file ) == 1 );
    ALIB_ASSERT( fwrite( palette, sizeof(palette), 1, file ) == 1 );

    for (int y = 0; y < bi.biHeight; y++)
    {
      const ubyte * p = pImage->row_begin( bInvertY ? (bi.biHeight-1-y) : y );
      std::copy( p, p+bi.biWidth, scan.begin() );
      ALIB_ASSERT( fwrite( &(*(scan.begin())), (int)(scanLength*sizeof(ubyte)), 1, file ) == 1 );
    }
  }
  catch (std::exception & e)
  {
    alib::ErrorMessage( L"Failed to save image", L"reason:", (LPCWSTR)CString( e.what() ), fname );
    ok = false;
  }

  if (file != 0)
    fclose( file );
  return ok;
}

} // namespace csutility

