/****************************************************************************
  LogBimodalHistogramAdapter.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev.
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csutility
{

//=================================================================================================
/** \class LogBimodalHistogramAdapter.
    \brief Class-adapter makes a linear transformation of input values
           being placed into histogram.

  If 'bFitBounds' is nonzero, then those values that fell out of histogram range will be placed
  either in the first or in the last histogram's entry, otherwise they will be thrown away. */
//=================================================================================================
class /*CS_UTIL_API*/ LogBimodalHistogramAdapter
{
private:
  double m_A;           //!< first coefficient in the formula: y = A*ln(B+x)
  double m_B;           //!< second coefficient in the formula: y = A*ln(B+x)
  double m_minValue;    //!< min. expected value
  double m_maxValue;    //!< max. expected value
  int    m_histoSize;   //!< desired histogram size
  bool   m_bReady;      //!< flag of object's readiness
  bool   m_bFitBounds;  //!< if nonzero, then values will be adjusted to histogram's margins

public:
  LogBimodalHistogramAdapter();

  void Clear();
  bool Initialize( double mean1, double dev1, double mean2, double dev2, int histoSize, bool bFitBounds = true );

  //LogBimodalHistogramAdapter & operator=( const LogBimodalHistogramAdapter & source );
  bool operator==( const LogBimodalHistogramAdapter & second );

  std::ostream & PutToStream( std::ostream & strm ) const;
  std::istream & GetFromStream( std::istream & strm );

  double GetA() const { return m_A; }
  double GetB() const { return m_B; }

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
    return ((int)(m_A * log( m_B + x ) + 0.5));
  }
};

} // namespace csutility

