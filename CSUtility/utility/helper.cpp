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

namespace csutility
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function checks whether the header has come from grayscaled of YUY2 bitmap.

  \param  h  the header to be checked.
  \return    non-zero if the header matches expected type. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool IsGrayOrYUY2( const BITMAPINFOHEADER * p )
{
  if (p == 0)
    return false;

  return (((p->biBitCount == 8) && ((p->biCompression == BI_RGB)     ||
                                    (p->biCompression == MY_BI_GRAY) ||
                                    (p->biCompression == MY_BI_Y800)))
          ||
          ((p->biBitCount == 16) && (p->biCompression == MY_BI_YUY2)));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function computes the length of DIB's scan line in bytes.

  \param  p  pointer to DIB's header.
  \return    the length of scan line in bytes. */
///////////////////////////////////////////////////////////////////////////////////////////////////
int GetDibScanSizeInBytes( const BITMAPINFOHEADER * p )
{
  return ((p != 0) ? ((((p->biWidth * p->biBitCount) + 31) & (~31)) / 8) : 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function computes the size of DIB's image (contents only) in bytes.

  \param  p  pointer to DIB's header.
  \return    the size of image contents. */
///////////////////////////////////////////////////////////////////////////////////////////////////
int GetDibContentsSizeInBytes( const BITMAPINFOHEADER * p )
{
  return ((p != 0) ? (csutility::GetDibScanSizeInBytes( p ) * abs( p->biHeight )) : 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function converts DIB to color image.

  Direction of axis Y of destination image will be selected according to
  DOWNWARD_AXIS_Y flag and DIB's height sign.

  \param  pHeader  pointer to the header of a bitmap.
  \param  pBytes   pointer to the bitmap's content.
  \param  pImage   pointer to the resultant color image. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void CopyDIBToColorImage( LPCBITMAPINFO pHeader, LPCUBYTE pBytes, QImage * pImage )
{
  if ((pHeader == 0) || (pBytes == 0) || (pImage == 0) || !IsGrayOrYUY2( &(pHeader->bmiHeader) ))
  {
    if (pImage != 0) pImage->fast_zero();
    return;
  }

  bool bInvertY = csutility::DoInvertAxisY( &(pHeader->bmiHeader) );
  int  nByte = pHeader->bmiHeader.biBitCount/8;
  int  W = csutility::GetDibScanSizeInBytes( &(pHeader->bmiHeader) );
  int  H = abs( pHeader->bmiHeader.biHeight );

  pImage->resize( pHeader->bmiHeader.biWidth, H, RGBQUAD(), false );
  for (int y = 0; y < H; y++)
  {
    const ubyte * src = pBytes + ((bInvertY ? (H-1-y) : y) * W);
    RGBQUAD     * dst = pImage->row_begin( y );
    RGBQUAD     * end = pImage->row_end( y );

    while (dst != end)
    {
      (*dst).rgbRed = ((*dst).rgbGreen = ((*dst).rgbBlue = (*src)));
      (*dst).rgbReserved = 0;
      dst += 1;
      src += nByte;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function converts DIB to grayscaled image.

  Direction of axis Y of destination image will be selected according to
  DOWNWARD_AXIS_Y flag and DIB's height sign.

  \param  pHeader  pointer to the header of a bitmap.
  \param  pBytes   pointer to the bitmap's content.
  \param  pImage   pointer to the resultant color image. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void CopyDIBToGrayImage( LPCBITMAPINFO pHeader, LPCUBYTE pBytes, Arr2ub * pImage )
{
  if ((pHeader == 0) || (pBytes == 0) || (pImage == 0) || !IsGrayOrYUY2( &(pHeader->bmiHeader) ))
  {
    if (pImage != 0) pImage->fast_zero();
    return;
  }

  bool bInvertY = csutility::DoInvertAxisY( &(pHeader->bmiHeader) );
  int  scanW = csutility::GetDibScanSizeInBytes( &(pHeader->bmiHeader) );
  int  W = pHeader->bmiHeader.biWidth;
  int  H = abs( pHeader->bmiHeader.biHeight );

  pImage->resize( W, H, ubyte(), false );
  for (int y = 0; y < H; y++)
  {
    const ubyte * src = pBytes + ((bInvertY ? (H-1-y) : y) * scanW);
    ubyte       * dst = pImage->row_begin( y );

    switch (pHeader->bmiHeader.biCompression)
    {
      case BI_RGB:
      case MY_BI_Y800:
      case MY_BI_GRAY:
      {
        for (int i = 0; i < W; i++)
          dst[i] = src[i];
      }
      break;

      case MY_BI_YUY2:
      {
        for (int i = 0; i < W; i++)
          dst[i] = src[i<<1];
      }
      break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function converts DIB to float image.

  Direction of axis Y of destination image will be selected according to
  DOWNWARD_AXIS_Y flag and DIB's height sign.

  \param  pHeader  pointer to the header of a bitmap.
  \param  pBytes   pointer to the bitmap's content.
  \param  pImage   pointer to the resultant color image. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void CopyDIBToFloatImage( LPCBITMAPINFO pHeader, LPCUBYTE pBytes, Arr2f * pImage )
{
  if ((pHeader == 0) || (pBytes == 0) || (pImage == 0) || !IsGrayOrYUY2( &(pHeader->bmiHeader) ))
  {
    if (pImage != 0) pImage->fast_zero();
    return;
  }

  bool bInvertY = csutility::DoInvertAxisY( &(pHeader->bmiHeader) );
  int  nByte = pHeader->bmiHeader.biBitCount/8;
  int  scanW = csutility::GetDibScanSizeInBytes( &(pHeader->bmiHeader) );
  int  H = abs( pHeader->bmiHeader.biHeight );

  pImage->resize( pHeader->bmiHeader.biWidth, H, 0.0f, false );
  for (int y = 0; y < H; y++)
  {
    const ubyte * src = pBytes + ((bInvertY ? (H-1-y) : y) * scanW);
    float       * dst = pImage->row_begin( y );
    float       * end = pImage->row_end( y );

    while (dst != end)
    {
      (*dst++) = (float)(*src);
      src += nByte;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function converts grayscaled image to the color one.

  \param  pSource  pointer to the source grayscaled image.
  \param  pImage   pointer to the resultant color image. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void CopyGrayImageToQImage( const Arr2ub * pSource, QImage * pImage )
{
  if ((pSource == 0) || (pImage == 0))
  {
    if (pImage != 0) pImage->fast_zero();
    return;
  }

  pImage->resize2( *pSource, false );
  for (int i = 0, n = pImage->size(); i < n; i++)
  {
    RGBQUAD & dst = (*pImage)[i];
    dst.rgbRed = (dst.rgbGreen = (dst.rgbBlue = (*pSource)[i]));
    dst.rgbReserved = 0;
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function converts grayscaled "float" image to color one (all pixels remain grayscaled).

  \param  pSource  pointer to the source grayscaled image.
  \param  pImage   pointer to the resultant color image. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void CopyFloatImageToColorImage( const Arr2f * pSource, QImage * pImage )
{
  const double MULTIPLIER = 2.0;
  double       deviation = 0.0, mean = 0.0, scale = 0.0;

  if ((pSource == 0) || (pImage == 0))
  {
    if (pImage != 0) pImage->fast_zero();
    return;
  }

  if (!(alib::AreDimensionsEqual( *pImage, *pSource )))
    pImage->resize2( *pSource, false );

  if (pSource->empty())
    return;

  // Compute intensity scale factor.
  {
    for (int i = 0, n = pSource->size(); i < n; i++)
    {
      float t = (*pSource)[i];
      mean += t;
      deviation += t*t;
    }
    mean /= (double)(pSource->size());
    deviation = sqrt( fabs( deviation/(double)(pSource->size()) - mean*mean ) );
    scale = (deviation > FLT_EPSILON) ? ((UCHAR_MAX/2.0)/(MULTIPLIER*deviation)) : 0.0;
  }

  // Fill color image. All pixels remain grayscaled.
  {
    for (int i = 0, n = pSource->size(); i < n; i++)
    {
      RGBQUAD & q = (*pImage)[i];
      double    f = ((*pSource)[i] - mean) * scale + (UCHAR_MAX/2.0);
      int       v;

#ifdef _M_IX86
      __asm {
        fld   f
        fistp v
      }
#else
      v = (int)f;
#endif

      q.rgbBlue = (q.rgbGreen = (q.rgbRed = (ubyte)(ALIB_LIMIT( v, 0, UCHAR_MAX ))));
      q.rgbReserved = (ubyte)0;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function draws a color image.

  \param  pImage  pointer to the image to be drawn.
  \param  hdc     device context.
  \param  rect    destination rectangle. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void DrawColorImage( const QImage * pImage, HDC hdc, CRect rect )
{
  if ((hdc == 0) || (pImage == 0) || pImage->empty())
    return;

  int W = pImage->width();
  int H = pImage->height();

  // Make 256-color bitmap header.
  BITMAPINFO bi;
  memset( &bi, 0, sizeof(bi) );
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = W;
  bi.bmiHeader.biHeight = DOWNWARD_AXIS_Y ? -H : +H;
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  bi.bmiHeader.biCompression = BI_RGB;
  bi.bmiHeader.biSizeImage = pImage->size() * sizeof(QImage::value_type);

  // Draw the image.
  int   newMode = ((rect.Width() < W) || (rect.Height() < H)) ? HALFTONE : COLORONCOLOR;
  int   oldMode = ::SetStretchBltMode( hdc, newMode );
  POINT point;

  point.y = (point.x = 0);
  if (newMode == HALFTONE)
    ::SetBrushOrgEx( hdc, 0, 0, &point );

  ::StretchDIBits( hdc, rect.left, rect.top, rect.Width(), rect.Height(),
    0, 0, W, H, &(*(pImage->begin())), &bi, DIB_RGB_COLORS, SRCCOPY );

  if (newMode == HALFTONE)
    ::SetBrushOrgEx( hdc, point.x, point.y, 0 );

  ::SetStretchBltMode( hdc, oldMode );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function initializes grayscaled palette and reserves,
           if necessary, the first 16 entries for color drawings.

  \param  palette   the palette to be initialized.
  \param  size      the size of palette, must be 256.
  \param  bColor16  if true, then the first 16 entries will be filled by system colors. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void InitColorGrayPalette( RGBQUAD * palette, int size, bool bColor16 )
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


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function draws a grayscaled image.

  \param  image     the image to be drawn.
  \param  hdc       device context.
  \param  rect      destination rectangle.
  \param  bInvertY  nonzero, if Y-axis must be inverted.
  \param  bColor16  nonzero, if the first 16 palette entries were reserved for system colors. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void DrawGrayImage( const Arr2ub * pImage, HDC hdc, CRect rect, bool bInvertY, bool bColor16 )
{
  if (pImage == 0)
    return;

  int W = pImage->width();
  int H = pImage->height();

  if (W & 3)
  {
    TRACE( _T("Image width must be aligned by 4 bytes") );
    return;
  }
  if ((hdc == 0) || pImage->empty())
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
  pbi->bmiHeader.biSizeImage = pImage->size() * sizeof(Arr2ub::value_type);

  // Fill palette.
  csutility::InitColorGrayPalette( &(pbi->bmiColors[0]), 256, bColor16 );

  // Draw the image.
  int   newMode = ((rect.Width() < W) || (rect.Height() < H)) ? HALFTONE : COLORONCOLOR;
  int   oldMode = ::SetStretchBltMode( hdc, newMode );
  POINT point;

  point.y = (point.x = 0);
  if (newMode == HALFTONE)
    ::SetBrushOrgEx( hdc, 0, 0, &point );

  ::StretchDIBits( hdc, rect.left, rect.top, rect.Width(), rect.Height(),
                   0, 0, W, H, &(*(pImage->begin())), pbi, DIB_RGB_COLORS, SRCCOPY );

  if (newMode == HALFTONE)
    ::SetBrushOrgEx( hdc, point.x, point.y, 0 );

  ::SetStretchBltMode( hdc, oldMode );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function draws the value of a digit into specified image.

  \param  text       the text to be printed.
  \param  x          abscissa of inscription's center or top-left corner.
  \param  y          ordinate of inscription's center or top-left corner.
  \param  bCenter    if nonzero, then (x,y) is inscription's center, otherwise its top-left corner.
  \param  pImage     pointer to destination image.
  \param  textColor  the color of inscription.
  \param  backColor  background color (0xFFFFFFFF means full transparency). */
///////////////////////////////////////////////////////////////////////////////////////////////////
void DrawDigits5x10( const char * text, int x, int y, bool bCenter,
                     QImage * pImage, COLORREF textColor, COLORREF backColor )
{
  typedef  unsigned char  SYMBOL[10][5];

  if ((text == 0) || (pImage == 0))
    return;

  const int      fontWidth = 5;
  const int      fontHeight = 10;
  int            symbolNum = (int)(strlen( text ));
  const SYMBOL * symbol = 0;

  if (bCenter)
  {
    x -= (fontWidth*symbolNum)/2;
    y -= (fontHeight/2);
  }

  // For all symbols ...
  for (int i = 0; i < symbolNum; i++, x += fontWidth)
  {
    if (!(pImage->inside( x, y )) || !(pImage->inside( x+fontWidth-1, y+fontHeight-1 )))
      continue;

    // Get symbol's template.
    if (ALIB_IS_LIMITED( text[i], '0', '9' ))
    {
      symbol = &(MY_DIGIT5x10[ text[i]-'0' ]);
    }
    else if (text[i] == '.')
    {
      symbol = &(MY_POINT5x10);
    }
    else if (text[i] == '-')
    {
      symbol = &(MY_MINUS5x10);
    }
    else continue;

    // Print a symbol in the image.
    for (int v = 0; v < fontHeight; v++)
    {
      int t = (DOWNWARD_AXIS_Y ? v : (fontHeight-1-v));

      for (int u = 0; u < fontWidth; u++)
      {
        if ((*symbol)[t][u] != 0)
        {
          RGBQUAD & q = (*pImage)( u+x, v+y );

          q.rgbRed   = GetRValue( textColor );
          q.rgbGreen = GetGValue( textColor );
          q.rgbBlue  = GetBValue( textColor );
        }
        else if (backColor != MY_TRANSPARENT)
        {
          RGBQUAD & q = (*pImage)( u+x, v+y );

          q.rgbRed   = GetRValue( backColor );
          q.rgbGreen = GetGValue( backColor );
          q.rgbBlue  = GetBValue( backColor );
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function adds the trailing backslash to the path name if necessary.

  \param  pPath  pointer the path name to be processed. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void AddTrailingBackslashToPathName( std::wstring * pPath )
{
  if (pPath != 0)
  {
    int N = (int)(pPath->size());
    if ((N > 0) && ((*pPath)[N-1] != '\\') && ((*pPath)[N-1] != '/'))
      (*pPath) += '\\';
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function repeatedly smoothes in-place an image by the mask (1,2,1)
           alternating horizontal and vertical directions.

  \param  image       pointer to an image to be processed.
  \param  nRepeat     the number of incremental smoothing repetitions.
  \param  tempBuffer  pointer to the external storage of intermediate data or null. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void RepeatedlySmoothImage( Arr2f * pImage, int nRepeat, UByteArr * pTempBuffer )
{
  if (pImage == 0)
    return;

  int      W = pImage->width();
  int      H = pImage->height();
  UByteArr buffer;

  if ((W < 2) || (H < 2))
    return;

  pTempBuffer = (pTempBuffer == 0) ? &buffer : pTempBuffer;
  if ((int)(pTempBuffer->size()) < (3*W*sizeof(float)))
    pTempBuffer->resize( 3*W*sizeof(float) );

  // Repeat smoothing several times.
  while (--nRepeat >= 0)
  {
    float * img = pImage->begin();
    float * row = img;
    float * pre = reinterpret_cast<float*>( &(*(pTempBuffer->begin())) ) + W*0;
    float * cur = reinterpret_cast<float*>( &(*(pTempBuffer->begin())) ) + W*1;
    float * nxt = reinterpret_cast<float*>( &(*(pTempBuffer->begin())) ) + W*2;

    // Read and smooth the first row.
    for (int x = 1; x < (W-1); x++)
    {
      cur[x] = (pre[x] = row[x-1] + row[x] + row[x] + row[x+1]);
    }
    cur[ 0 ] = (pre[ 0 ] = row[ 0 ] + row[ 0 ] + row[ 0 ] + row[ 1 ]);
    cur[W-1] = (pre[W-1] = row[W-2] + row[W-1] + row[W-1] + row[W-1]);

    // Go along the image row by row...
    for (int y = 0; y < H; y++)
    {
      // Read and smooth the next row.
      if ((y+1) < H)
      {
        row = (img+W);
        for(int x = 1; x < (W-1); x++)
        {
          nxt[x] = row[x-1] + row[x] + row[x] + row[x+1];
        }
        nxt[ 0 ] = row[ 0 ] + row[ 0 ] + row[ 0 ] + row[ 1 ];
        nxt[W-1] = row[W-2] + row[W-1] + row[W-1] + row[W-1];
      }
      else nxt = cur;

      // Smooth the image in vertical direction scaling result appropriately.
      for (int x = 0; x < W; x++)
      {
        img[x] = (float)((pre[x] + cur[x] + cur[x] + nxt[x])*(1.0/16.0));
      }

      // Cyclically swap row pointers.
      float * tmp = pre;   pre = cur;   cur = nxt;   nxt = tmp;
      img += W;
    }

    ASSERT( img == pImage->end() );
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function saves true-color image in a bitmap file.

  \param  fname     the name of destination file.
  \param  pImage    image to be stored.
  \param  bInvertY  nonzero flag forces inversion of axis Y.
  \return           Ok == true. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool SaveQImage( LPCWSTR fname, const QImage * pImage, bool bInvertY )
{
  BITMAPFILEHEADER bf;
  BITMAPINFOHEADER bi;
  UByteArr         scan( ((3*pImage->width()+3) & (~3)), 0 );

  try
  {
    ALIB_ASSERT( (pImage != 0) && !(pImage->empty()) );
    alib::BinaryFile io( fname, false );

    memset( &bf, 0, sizeof(bf) );
    memset( &bi, 0, sizeof(bi) );

    bi.biSize = sizeof(bi); 
    bi.biWidth = pImage->width(); 
    bi.biHeight = pImage->height(); 
    bi.biPlanes = 1; 
    bi.biBitCount = 24;
    bi.biCompression = 0;
    bi.biSizeImage = (int)(pImage->height()*scan.size()*sizeof(UByteArr::value_type));

    bf.bfType = 0x4D42;
    bf.bfSize = 14 + sizeof(BITMAPINFOHEADER) + bi.biSizeImage;
    bf.bfReserved1 = 0;
    bf.bfReserved2 = 0;
    bf.bfOffBits = 14 + sizeof(BITMAPINFOHEADER);

    io.write( &(bf.bfType), sizeof(bf.bfType) );
    io.write( &(bf.bfSize), sizeof(bf.bfSize) );
    io.write( &(bf.bfReserved1), sizeof(bf.bfReserved1) );
    io.write( &(bf.bfReserved2), sizeof(bf.bfReserved2) );
    io.write( &(bf.bfOffBits), sizeof(bf.bfOffBits) );
    io.write( &bi, sizeof(bi) );

    for(int y = 0; y < bi.biHeight; y++)
    {
      const RGBQUAD * s = pImage->row_begin( bInvertY ? (bi.biHeight-1-y) : y );
      ubyte         * d = &(*(scan.begin()));

      for(int k = 0; k < bi.biWidth; k++)
      {
        *d++ = s->rgbBlue;
        *d++ = s->rgbGreen;
        *d++ = s->rgbRed;
        s++;
      }
      io.write( &(*(scan.begin())), (int)(scan.size()) );
    }
  }
  catch (std::exception &)
  {
    return false;
  }
  return true;
}

} // namespace csutility

