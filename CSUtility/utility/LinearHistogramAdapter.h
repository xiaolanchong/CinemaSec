/****************************************************************************
  LinearHistogramAdapter.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev.
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csutility
{

//=================================================================================================
/** \class LinearHistogramAdapter.
    \brief Class-adapter makes a linear transformation of input values
           being placed into histogram.

  If 'bFitBounds' is nonzero, then those values that fell out of histogram range will be placed
  either in the first or in the last histogram's entry, otherwise they will be thrown away. */
//=================================================================================================
class /*CS_UTIL_API*/ LinearHistogramAdapter
{
private:
  double m_minValue;     //!< min. expected value
  double m_maxValue;     //!< max. expected value
  double m_scale;        //!< scale factor of linear transformation
  int    m_histoSize;    //!< desired histogram size
  bool   m_bReady;       //!< flag of object's readiness
  bool   m_bFitBounds;   //!< if nonzero, then values will be adjusted to histogram's margins

public:
  LinearHistogramAdapter();

  void Clear();
  bool Initialize( double minValue, double maxValue, int histoSize, bool bFitBounds = true );

  //LinearHistogramAdapter & operator=( const LinearHistogramAdapter & source );
  bool operator==( const LinearHistogramAdapter & second );

  std::ostream & PutToStream( std::ostream & strm ) const;
  std::istream & GetFromStream( std::istream & strm );

  bool Ready() const
  {
    return m_bReady;
  }

  int HistogramSize() const
  {
    return m_histoSize;
  }

  template< class VALUE_TYPE >
  int EntryIndex( VALUE_TYPE v ) const
  {
    ASSERT( m_bReady );
    double x = (double)v;
    if (m_bFitBounds)
    {
      x = ALIB_LIMIT( x, m_minValue, m_maxValue );
    }
    else if (!ALIB_IS_LIMITED( x, m_minValue, m_maxValue ))
    {
      return (-1);
    }
    return ((int)((v - m_minValue) * m_scale + 0.5));
  }
};

} // namespace csutility

