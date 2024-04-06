/****************************************************************************
  inline_functions.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csutility
{

//-------------------------------------------------------------------------------------------------
/** \brief Function checks equality of dimensions of two images.

  \param  i  pointer to an image of arbitrary type.
  \param  h  pointer to a bitmap header.
  \return    nonzero if dimensions are equal. */
//-------------------------------------------------------------------------------------------------
template< class IMAGE >
bool AreDimensionsEqual( const IMAGE * i, const BITMAPINFOHEADER * h )
{
  return ((i != 0) && (h != 0) && (i->width() == h->biWidth) && (i->height() == abs( h->biHeight )));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function examines directions of axis Y of a DIB and tells whether the axis Y must
           inverted or not.

  In our case, compressed DIBs have axis Y directed <b>downward</b> unless DIB's height
  explicitly set negative. In contrast, uncompressed DIBs have axis Y directed <b>upward</b>
  unless the height explicitly set negative.

  \param  pHeader  pointer to the DIB header.
  \return          non-zero if axis Y directed downward. */
//-------------------------------------------------------------------------------------------------
inline bool DoInvertAxisY( const BITMAPINFOHEADER * pHeader )
{
  if (pHeader->biCompression != BI_RGB)
  {
    return ((pHeader->biHeight > 0) ? !(DOWNWARD_AXIS_Y) : DOWNWARD_AXIS_Y);
  }
  else // if (pHeader->biCompression == BI_RGB)
  {
    return ((pHeader->biHeight > 0) ? DOWNWARD_AXIS_Y : !(DOWNWARD_AXIS_Y));
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function dynamically casts a pointer of input type to the pointer of output type.

  \param  p1   pointer of input type.
  \param  pp2  address of pointer of output type. */
//-------------------------------------------------------------------------------------------------
template< class INTYPE, class OUTTYPE >
void DynamicCastPtr( INTYPE * p1, OUTTYPE ** pp2 ) throw(...)
{
  ASSERT( pp2 != 0 );
  (*pp2) = dynamic_cast<OUTTYPE*>( p1 );
  ALIB_ASSERT( (*pp2) != 0 );
}

} // namespace csutility

