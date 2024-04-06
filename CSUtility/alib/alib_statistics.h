/***************************************************************************
  alib_statistics.h
  ---------------------
  begin     : 1998
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com, aaahaaah@hotmail.com
 ***************************************************************************/

#pragma once

//=================================================================================================
/** \class MeanAccum.
    \brief Simple accumulator that is used to compute the mean. */
//=================================================================================================
template< class T >
class MeanAccum
{
private:
  T   sum;         //!< the sum of values
  int num;         //!< the number of values

public:
//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
MeanAccum() : sum((T)0), num(0)
{
  ASSERT( std::numeric_limits<T>::is_specialized );
}

//-------------------------------------------------------------------------------------------------
/** \brief Function prepares this object for computation. */
//-------------------------------------------------------------------------------------------------
void reset()
{
  sum = (T)0;
  num = 0;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function adds new value to the accumulator. */
//-------------------------------------------------------------------------------------------------
void operator+=( T value )
{
  sum += value;
  ++num;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function adds specified accumulator to this one. */
//-------------------------------------------------------------------------------------------------
void operator+=( const MeanAccum<T> & a )
{
  if (&a != this)
  {
    sum += a.sum;
    num += a.num;
  }
}

//-------------------------------------------------------------------------------------------------
/** \brief Function returns the mean value. */
//-------------------------------------------------------------------------------------------------
double mean() const
{
  return (num > 0) ? ((double)sum/(double)num) : 0.0;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function returns accumulated value. */
//-------------------------------------------------------------------------------------------------
double accumulator() const
{
  return sum;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function returns counter's value. */
//-------------------------------------------------------------------------------------------------
int count() const
{
  return num;
}

};


//=================================================================================================
/** \class MeanVarAccum.
    \brief Simple accumulator that is used to compute the mean and variance. */
//=================================================================================================
template< class T >
class MeanVarAccum
{
private:
  T   sum;         //!< the sum of values
  T   sqSum;       //!< the sum of squared values
  int num;         //!< the number of values

public:
//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
MeanVarAccum() : sum((T)0), sqSum((T)0), num(0)
{
  ASSERT( std::numeric_limits<T>::is_specialized );
}

//-------------------------------------------------------------------------------------------------
/** \brief Function prepares this object for computation. */
//-------------------------------------------------------------------------------------------------
void reset()
{
  sum = (T)0;
  sqSum = (T)0;
  num = 0;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function adds new value to the accumulator. */
//-------------------------------------------------------------------------------------------------
void operator+=( T value )
{
  sum += value;
  sqSum += value*value;
  ++num;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function adds specified accumulator to this one. */
//-------------------------------------------------------------------------------------------------
void operator+=( const MeanVarAccum<T> & a )
{
  if (&a != this)
  {
    sum   += a.sum;
    sqSum += a.sqSum;
    num   += a.num;
  }
}

//-------------------------------------------------------------------------------------------------
/** \brief Function computes second order statistics from the accumulated data.

\param  pMean  out: the pointer to the storage that receives mean value.
\param  pVar   out: the pointer to the storage that receives variance.
\param  pDev   out: the pointer to the storage that receives deviation (square root of variance).
\return        Ok = true. */
//-------------------------------------------------------------------------------------------------
bool statistics( double * pMean, double * pVar, double * pDev ) const
{
  bool   ok = false;
  double mean = 0.0;
  double var = 0.0;

  if (num > 0)
  {
    mean = sum/num;
    var = sqSum/num - mean*mean;

    if (var < 0.0)
      mean = (var = 0.0);  // perhaps overflow had happened
    else
      ok = true;
  }

  if (pMean != 0) (*pMean) = mean;
  if (pVar  != 0) (*pVar)  = fabs( var );
  if (pDev  != 0) (*pDev)  = sqrt( fabs( var ) );
  return ok;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function returns counter's value. */
//-------------------------------------------------------------------------------------------------
int count() const
{
  return num;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function loads statistics from a file. */
//-------------------------------------------------------------------------------------------------
bool Load( std::istream & strm )
{
  if (strm.good())
  {
    strm >> num;
    strm >> sum;
    strm >> sqSum;
    if (strm.good() || strm.eof())
      return true;
  }
  reset();
  return false;
}

//-------------------------------------------------------------------------------------------------
/** \brief Function saves statistics to a file. */
//-------------------------------------------------------------------------------------------------
bool Save( std::ostream & strm ) const
{
  if (strm.good())
  {
    const int NDIGITS = (int)(std::numeric_limits<T>::digits10);
    strm << num << std::endl;
    strm << std::setprecision( NDIGITS ) << sum << std::endl;
    strm << std::setprecision( NDIGITS ) << sqSum << std::endl;
  }
  return (strm.good());
}

};


//>>>>> OBSOLETE
namespace alib
{

//-------------------------------------------------------------------------------------------------
/** \brief Function calculates simple statistics from the input data:
           mean, deviation and maximal residual.

  \param  F           begin iterator of the input data.
  \param  L           end iterator of the input data.
  \param  bCalcMean   if true, then the mean will be calculated, otherwise it will be set to zero.
  \param  bQuadratic  if true, then compute deviation as a root-mean-square deflection,
                      otherwise as a mean deflection.
  \param  pMean       out: pointer to the receiver of mean value or 0.
  \param  pDev        out: pointer to the receiver of deviation value or 0.
  \param  pMaxRes     out: pointer to the receiver of maximal residual value or 0. */
//-------------------------------------------------------------------------------------------------
template< class IT >
void CalcSimpleStatistics( IT F, IT L, bool bCalcMean, bool bQuadratic,
                           double * pMean = 0, double * pDev = 0, double * pMaxRes = 0 )
{
  double mean = 0.0, dev = 0.0, res = 0.0;
  int    N = 0;

  if (bCalcMean)
  {
    for(IT it = F; it != L; ++it)
    {
      mean += (*it);
      ++N;
    }
    mean = (N > 0) ? (mean/N) : 0.0;
  }

  for(N = 0; F != L; ++F)
  {
    double t = fabs( (*F) - mean );
    dev += (bQuadratic ? (t*t) : t);
    res = std::max( res, t );
    ++N;
  }
  dev = (N > 1) ? (bQuadratic ? (sqrt( dev/(N-1) )) : (dev/(N-1))) : 0.0;

  if (pMean != 0)
    *pMean = mean;

  if (pDev != 0)
    *pDev = dev;

  if (pMaxRes != 0)
    *pMaxRes = res;
}

} // namespace alib
//>>>>> OBSOLETE


