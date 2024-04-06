/****************************************************************************
  types.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

typedef  const unsigned char    * LPCUBYTE;
typedef  const BITMAPINFO       * LPCBITMAPINFO;
typedef  Arr2D<RGBQUAD,RGBQUAD>   QImage;
typedef  Arr2D<RGBQUAD,RGBQUAD>   AImage;  // synonym here
typedef  const BITMAPINFOHEADER * LPCDIB;

typedef  std::vector<Arr2f>       FltImgArr;
typedef  std::vector<FltImgArr>   FltImgChainArr;
typedef  std::vector<__int8>      Int8Arr;
typedef  std::vector<__int64>     Int64Arr;
typedef  std::vector<Arr2ub>      ByteImgArr;

//=================================================================================================
/** \struct PixCol.
    \brief  Point and its color. */
//=================================================================================================
struct PixCol : public CPoint
{
  COLORREF color;

  PixCol() : color(RGB(0,0,0))
  {
    x = (y = 0);
  }

  bool integer() const
  {
    return true;
  }

  PixCol & operator=( const Vec2i & p )
  {
    x = p.x;
    y = p.y;
    color = RGB(0,0,0);
    return (*this);
  }
};

typedef  std::vector<PixCol>  PixColArr;

