/****************************************************************************
  helper.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Function checks whether the header has come from grayscaled of YUY2 bitmap.

  \param  h  the header to be checked.
  \return    non-zero if the header matches expected type. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API bool IsGrayOrYUY2( const BITMAPINFOHEADER & h )
{
  return (((h.biBitCount == 8) && ((h.biCompression == BI_RGB)     ||
                                   (h.biCompression == MY_BI_GRAY) ||
                                   (h.biCompression == MY_BI_Y800)))
           ||
          ((h.biBitCount == 16) && (h.biCompression == MY_BI_YUY2)));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function extracts a path from a file name without trailing backslash: <br>
           "C:\FFF\KKK\LLL\abcd.txt" --> "C:\FFF\KKK\LLL", <br>  "abcd.txt" --> "".

  \param  source  the source full-path file name.
  \param  result  out: extracted path. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API void MyGetFilePath( const StdStr & source, StdStr & result )
{
  unsigned int pos = (int)(source.find_last_of( _T("/\\") ));
  result.erase();
  if (pos != source.npos)
    (result = source).erase( pos );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function constructs a file name with a new extension.

  \param  source  the input file name.
  \param  newExt  the desired new extension.
  \return         the file name with new extension. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API StdStr MyMakeFileNameWithNewExtension( LPCWSTR source, LPCWSTR newExt )
{
  ASSERT( (source != 0) && (newExt != 0) );

  StdStr       fname = source;
  unsigned int pos = (unsigned int)(fname.find_last_of( _T(".\\/") ));

  if ((pos != fname.npos) && (fname[pos] == _T('.')))
    fname.erase( pos );

  (fname += _T(".")) += newExt;
  return fname;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function constructs the name of a file prepared offline, which is
           suitable for all videosequences obtained from the same camera.

  \param  source  the input file name.
  \param  label   the desired new label added at the end of file name or 0 (does nothing).
  \param  ext     the desired new extension or 0 (does nothing).
  \return         the file name with new extension. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API StdStr MyMakeCameraFileName( LPCWSTR source, LPCWSTR label, LPCWSTR ext )
{
  StdStr  fname;
  LPCWSTR p1 = std::max( _tcsrchr( source, _T('\\') ), _tcsrchr( source, _T('/') ) );
  LPCWSTR p2 = _tcsrchr( source, '.' );

  if ((p1 == 0) || (p2 == 0) || (p2 <= p1) || (_tcsnccmp( ++p1, _T("cam"), 3 ) != 0))
    return fname;

  int index = _ttoi( p1 += 3 );
  if (!ALIB_IS_LIMITED( index, 1, MAX_SUBFRAME_NUM ))
    return fname;

  fname.assign( source, ++p1 );
  if (label != 0) (fname += _T('_')) += label;
  if (ext != 0) (fname += _T('.')) += ext; else fname += p2;
  return fname;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function adds the trailing backslash to the path name, if necessary.

  \param  path  the path name to be processed. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API void MyAddTrailingBackslashToPathName( StdStr & path )
{
  int N = (int)(path.size());
  if ((N > 0) && (path[N-1] != ALIB_WIN_BACKSLASH) && (path[N-1] != ALIB_UNIX_BACKSLASH))
    path += ALIB_WIN_BACKSLASH;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes the length of DIB's scan line in bytes.

  \param  p  pointer to DIB's header.
  \return    the length of scan line in bytes. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API int MyGetDibScanSizeInBytes( const BITMAPINFOHEADER * p )
{
  return (p != 0) ? ((((p->biWidth * p->biBitCount) + 31) & (~31)) / 8) : 0;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function computes the size of DIB's image (contents only) in bytes.

  \param  p  pointer to DIB's header.
  \return    the size of image contents. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API int MyGetDibContentsSizeInBytes( const BITMAPINFOHEADER * p )
{
  return (p != 0) ? (MyGetDibScanSizeInBytes( p ) * abs( p->biHeight )) : 0;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function scales an image with positive float entries to fit [0..255] range.

  \param  image  the image to be adjusted. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API void MyAdjustPositiveFloatImageToByteRange( Arr2f & image )
{
  const double         MULTIPLIER = 2.0;
  MeanVarAccum<double> accum;
  double               deviation = 0.0, mean = 0.0, multiplier = 0.0;

  // Compute intensity scale factor.
  for (int i = 0, n = image.size(); i < n; i++)
  {
    accum += image[i];
  }
  accum.statistics( &mean, 0, &deviation );
  multiplier = (deviation > FLT_EPSILON) ? (127.0/(MULTIPLIER*deviation)) : 0.0;

  // Adjust image entries.
  for (int k = 0, m = image.size(); k < m; k++)
  {
    image[k] = (float)(alib::Limit( (image[k]-mean)*multiplier+127.0, 0.0, 255.0 ));
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function converts grayscaled image to color one.

  \param  pHeader  pointer to the header of a bitmap.
  \param  pImage   pointer to the bitmap's content.
  \param  imgSize  bitmap's size in bytes.
  \param  pResult  pointer to the resultant color image.
  \return          Ok = true, otherwise convertion is impossible. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API bool MyGrayImageToQImage( LPCBITMAPINFO pHeader,
                                      LPCUBYTE      pImage,
                                      int           imgSize,
                                      QImage *      pResult )
{
  if ((pHeader == 0) || (pImage == 0) || (pResult == 0) || !IsGrayOrYUY2( pHeader->bmiHeader ))
  {
    pResult->fast_zero();
    return false;
  }

  int nByte = (pHeader->bmiHeader.biCompression == MY_BI_YUY2) ? 2 : 1;
  int W = pHeader->bmiHeader.biWidth * nByte;
  int H = pHeader->bmiHeader.biHeight;

  W = ((W+3)&(~3));
  if (imgSize != (W*H))
  {
    pResult->fast_zero();
    return false;
  }

  pResult->resize( pHeader->bmiHeader.biWidth, pHeader->bmiHeader.biHeight, RGBQUAD(), false );
  for (int y = 0; y < H; y++)
  {
    const ubyte * src = pImage + ((INVERT_AXIS_Y ? (H-1-y) : y) * W);
    RGBQUAD     * dst = pResult->row_begin( y );
    RGBQUAD     * end = pResult->row_end( y );

    while (dst != end)
    {
      (*dst).rgbRed = ((*dst).rgbGreen = ((*dst).rgbBlue = (*src)));
      (*dst).rgbReserved = 0;
      dst += 1;
      src += nByte;
    }
  }
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function converts "float" image to "byte" one.

  \param  source    the source image.
  \param  result    out: resultant image.
  \param  scale     the factor of dimension scaling of resultant image (must be >=1).
  \param  bColor16  true, if the first 16 palette entries are reserved for system colors. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API void MyFloatImageToByteImage( const Arr2f & source, Arr2ub & result, int scale, bool bColor16 )
{
  const double         MULTIPLIER = 2.0;
  MeanVarAccum<double> accum;
  double               deviation = 0.0, mean = 0.0, multiplier = 0.0;

  // Compute intensity scale factor.
  for (int i = 0, n = source.size(); i < n; i++)
  {
    accum += source[i];
  }
  accum.statistics( &mean, 0, &deviation );
  multiplier = (deviation > FLT_EPSILON) ? (127.0/(MULTIPLIER*deviation)) : 0.0;

  // Adjust image entries.
  scale = alib::Limit( scale, 1, 100 );
  if (scale == 1)
  {
    result.resize2( source, false );
    for (int i = 0, n = source.size(); i < n; i++)
    {
      ubyte t = (ubyte)(alib::Limit( alib::Round( (source[i]-mean)*multiplier+127.0 ), 0, 255 ));
      result[i] = (bColor16 ? (std::max<ubyte>( t, 16 )) : t);
    }
  }
  else
  {
    int W = source.width();
    int H = source.height();

    result.resize( W*scale, H*scale, 0, false );
    for (int y = 0; y < H; y++)
    {
      const float * src = source.row_begin( y );
      ubyte       * res = result.row_begin( y*scale );

      for (int x = 0, k = 0; x < W; x++, k += scale)
      {
        ubyte t = (ubyte)(alib::Limit( alib::Round( (src[x]-mean)*multiplier+127.0 ), 0, 255 ));
        std::fill( res+k, res+(k+scale), (bColor16 ? (std::max<ubyte>( t, 16 )) : t) );
      }

      for (int i = 1; i < scale; i++)
        std::copy( res, res+(scale*W), res+(i*scale*W) );
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function converts grayscaled "float" image to color one (all pixels remain grayscaled).

  \param  source  the source image.
  \param  result  out: resultant image. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API void MyFloatImageToColorImage( const Arr2f & source, QImage & result )
{
  const double MULTIPLIER = 2.0;
  double       deviation = 0.0, mean = 0.0, multiplier = 0.0;

  if (!(alib::AreDimensionsEqual( result, source )))
    result.resize2( source, false );

  if (source.empty())
    return;

  // Compute intensity scale factor.
  {
    for (int i = 0, n = source.size(); i < n; i++)
    {
      float t = source[i];
      mean += t;
      deviation += t*t;
    }
    mean /= (double)(source.size());
    deviation = sqrt( deviation/(double)(source.size()) - mean*mean );
    multiplier = (deviation > FLT_EPSILON) ? (128.0/(MULTIPLIER*deviation)) : 0.0;
  }

  // Fill color image. All pixels remain grayscaled.
  {
    for (int i = 0, n = source.size(); i < n; i++)
    {
      RGBQUAD & q = result[i];
      double    f = (source[i] - mean)*multiplier + 128.0;
      int       v;

#ifdef _M_IX86
      __asm {
        fld   f
        fistp v
      }
#else
      v = (int)f;
#endif

      q.rgbBlue = (q.rgbGreen = (q.rgbRed = (ubyte)(ALIB_LIMIT( v, 0, 255 ))));
      q.rgbReserved = (ubyte)0;
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function draws a grayscaled image.

  \param  image     the image to be drawn.
  \param  hdc       device context.
  \param  rect      destination rectangle.
  \param  bInvertY  true, if Y-axis must be inverted.
  \param  bColor16  true, if the first 16 palette entries are reserved for system colors. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API void MyDrawGrayImage( const Arr2ub & image, HDC hdc, CRect rect, bool bInvertY, bool bColor16 )
{
  int W = image.width();
  int H = image.height();

  if (W & 3)
  {
    TRACE( _T("Image width must be aligned by 4 bytes") );
    return;
  }
  if ((hdc == 0) || image.empty())
    return;

  // Make 256-color bitmap header.
  unsigned char buf[ sizeof(BITMAPINFO)+256*sizeof(RGBQUAD) ];
  BITMAPINFO *  pbi = reinterpret_cast<BITMAPINFO*>( &(buf[0]) );

  memset( buf, 0, sizeof(buf) );
  pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  pbi->bmiHeader.biWidth = W;
  pbi->bmiHeader.biHeight = bInvertY ? -H : +H;
  pbi->bmiHeader.biPlanes = 1;
  pbi->bmiHeader.biBitCount = 8;
  pbi->bmiHeader.biCompression = BI_RGB;
  pbi->bmiHeader.biSizeImage = image.size() * sizeof(Arr2ub::value_type);

  // Fill palette.
  MyInitColorGrayPalette( &(pbi->bmiColors[0]), 256, bColor16 );

  // Draw the image.
  int   newMode = ((rect.Width() < W) || (rect.Height() < H)) ? HALFTONE : COLORONCOLOR;
  int   oldMode = ::SetStretchBltMode( hdc, newMode );
  POINT point;

  point.y = (point.x = 0);
  if (newMode == HALFTONE)
    ::SetBrushOrgEx( hdc, 0, 0, &point );

  ::StretchDIBits( hdc, rect.left, rect.top, rect.Width(), rect.Height(),
                   0, 0, W, H, &(*(image.begin())), pbi, DIB_RGB_COLORS, SRCCOPY );

  if (newMode == HALFTONE)
    ::SetBrushOrgEx( hdc, point.x, point.y, 0 );

  ::SetStretchBltMode( hdc, oldMode );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function draws a color image.

  \param  image     the image to be drawn.
  \param  hdc       device context.
  \param  rect      destination rectangle.
  \param  bInvertY  true, if Y-axis must be inverted. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API void MyDrawColorImage( const QImage & image, HDC hdc, CRect rect, bool bInvertY )
{
  int W = image.width();
  int H = image.height();

  if ((hdc == 0) || image.empty())
    return;

  // Make 256-color bitmap header.
  BITMAPINFO bi;
  memset( &bi, 0, sizeof(bi) );
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = W;
  bi.bmiHeader.biHeight = bInvertY ? -H : +H;
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  bi.bmiHeader.biCompression = BI_RGB;
  bi.bmiHeader.biSizeImage = image.size() * sizeof(QImage::value_type);

  // Draw the image.
  int   newMode = ((rect.Width() < W) || (rect.Height() < H)) ? HALFTONE : COLORONCOLOR;
  int   oldMode = ::SetStretchBltMode( hdc, newMode );
  POINT point;

  point.y = (point.x = 0);
  if (newMode == HALFTONE)
    ::SetBrushOrgEx( hdc, 0, 0, &point );

  ::StretchDIBits( hdc, rect.left, rect.top, rect.Width(), rect.Height(),
                   0, 0, W, H, &(*(image.begin())), &bi, DIB_RGB_COLORS, SRCCOPY );

  if (newMode == HALFTONE)
    ::SetBrushOrgEx( hdc, point.x, point.y, 0 );

  ::SetStretchBltMode( hdc, oldMode );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes grayscaled palette with possibility
           to reserve the first 16 entries for color drawings.

  \param  palette   the palette to be initialized.
  \param  size      the size of palette, must be 256.
  \param  bColor16  if true, then the first 16 entries will be filled by system colors. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API void MyInitColorGrayPalette( RGBQUAD * palette, int size, bool bColor16 )
{
  ASSERT( size == 256 );
  if (bColor16)
  {
    for (int k = 0; k < size; k++)
    {
      if (k < 16)
      {
        palette[k] = MyColors16[k];
      }
      else
      {
        (palette[k]).rgbRed = ((palette[k]).rgbGreen = ((palette[k]).rgbBlue = (BYTE)k));
        (palette[k]).rgbReserved = 0;
      }
    }
  }
  else
  {
    for (int k = 0; k < size; k++)
    {
      (palette[k]).rgbRed = ((palette[k]).rgbGreen = ((palette[k]).rgbBlue = (BYTE)k));
      (palette[k]).rgbReserved = 0;
    }
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function ensures that the lowest image entries are not less than 16, so that the first
           16 values could be reserved for system colors.

  \param  image  the image whose entries to be adjusted. */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API void MyAdjustLowestValuesToAllowSystemColors( Arr2ub & image )
{
  for (int i = 0, n = image.size(); i < n; i++)
  {
    if (image[i] < 16)
      image[i] = 16;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function draws the value of a digit into specified image.

  \param  value      the value to be drawn (integer values are supported as well).
  \param  x          abscissa of inscription center.
  \param  y          ordinate of inscription center.
  \param  precision  desired precision of a digit (zero for an integer value).
  \param  image      destination image.
  \param  textColor  the color of inscription.
  \param  backColor  background color (0xFFFFFFFF means full transparency). */
//-------------------------------------------------------------------------------------------------
CS_UTIL_API int MyDrawDigitInQImage( double value, int x, int y, int precision,
                                     QImage & image, COLORREF textColor, COLORREF backColor )
{
  typedef  unsigned char  SYMBOL[10][5];

  char           txt[64], format[32];
  int            fontWidth = 5;
  int            fontHeight = 10;
  int            symbolNum = 0;
  const SYMBOL * symbol = 0;

  sprintf( format, "%%0%d.%df", (precision+2), precision );
  sprintf( txt, format, value );
  symbolNum = (int)(strlen( txt ));
  x -= (fontWidth*symbolNum)/2;
  y -= (fontHeight/2);

  // For all symbols ...
  for (int i = 0; i < symbolNum; i++, x += fontWidth)
  {
    if (!(image.inside( x, y )) || !(image.inside( x+fontWidth-1, y+fontHeight-1 )))
      continue;

    // Get symbol's template.
    if (ALIB_IS_LIMITED( txt[i], '0', '9' ))
    {
      symbol = &(MY_DIGIT5x10[ txt[i]-'0' ]);
    }
    else if (txt[i] == '.')
    {
      symbol = &(MY_POINT5x10);
    }
    else if (txt[i] == '-')
    {
      symbol = &(MY_MINUS5x10);
    }
    else continue;

    // Print a symbol in the image.
    for (int v = 0; v < fontHeight; v++)
    {
      int t = (INVERT_AXIS_Y) ? v : (fontHeight-1-v);

      for (int u = 0; u < fontWidth; u++)
      {
        if ((*symbol)[t][u] != 0)
        {
          RGBQUAD & q = image( u+x, v+y );

          q.rgbRed   = GetRValue( textColor );
          q.rgbGreen = GetGValue( textColor );
          q.rgbBlue  = GetBValue( textColor );
        }
        else if (backColor != 0xFFFFFFFF)
        {
          RGBQUAD & q = image( u+x, v+y );

          q.rgbRed   = GetRValue( backColor );
          q.rgbGreen = GetGValue( backColor );
          q.rgbBlue  = GetBValue( backColor );
        }
      }
    }
  }
  return fontHeight;
}


namespace csutility
{
  // Obsolete.
  bool GrayImageToQImage( LPCBITMAPINFO pHeader,
                          LPCUBYTE      pBytes,
                          QImage *      pImage )
  {
    CopyDIBToColorImage( pHeader, pBytes, pImage );
    return true;
  }
}
