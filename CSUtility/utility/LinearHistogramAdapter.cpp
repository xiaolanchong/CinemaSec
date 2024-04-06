/****************************************************************************
  LinearHistogramAdapter.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev.
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csutility
{

const char LINEAR_HISTOGRAM_ADAPTER[] = "Linear_Histogram_Adapter";

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
LinearHistogramAdapter::LinearHistogramAdapter()
{
  Clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function . */
///////////////////////////////////////////////////////////////////////////////////////////////////
void LinearHistogramAdapter::Clear()
{
  m_minValue = 0.0;
  m_maxValue = 0.0;
  m_scale = 0.0;
  m_histoSize = 0;
  m_bReady = false;
  m_bFitBounds = true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function . */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LinearHistogramAdapter::Initialize( double minValue,
                                         double maxValue, int histoSize, bool bFitBounds )
{
  double delta = (maxValue - minValue);
  if ((histoSize > 0) && (delta > (FLT_MIN/FLT_EPSILON)))
  {
    m_minValue   = minValue;
    m_maxValue   = maxValue;
    m_scale      = (double)(histoSize-1)/delta;
    m_histoSize  = histoSize;
    m_bReady     = true;
    m_bFitBounds = bFitBounds;
    return true;
  }
  Clear();
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function . */
///////////////////////////////////////////////////////////////////////////////////////////////////
/*
LinearHistogramAdapter & LinearHistogramAdapter::operator=( const LinearHistogramAdapter & source )
{
  m_minValue   = source.m_minValue;
  m_maxValue   = source.m_maxValue;
  m_scale      = source.m_scale;
  m_histoSize  = source.m_histoSize;
  m_bReady     = source.m_bReady;
  m_bFitBounds = source.m_bFitBounds;
  return (*this);
}
*/


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function . */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LinearHistogramAdapter::operator==( const LinearHistogramAdapter & second )
{
  return ((m_minValue   == second.m_minValue)  &&
          (m_maxValue   == second.m_maxValue)  &&
          (m_scale      == second.m_scale)     &&
          (m_histoSize  == second.m_histoSize) &&
          (m_bReady     == second.m_bReady)    &&
          (m_bFitBounds == second.m_bFitBounds ));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function . */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::ostream & LinearHistogramAdapter::PutToStream( std::ostream & strm ) const
{
  strm << LINEAR_HISTOGRAM_ADAPTER << std::endl
       << m_minValue               << std::endl
       << m_maxValue               << std::endl
       << m_scale                  << std::endl
       << m_histoSize              << std::endl
       << m_bReady                 << std::endl
       << m_bFitBounds             << std::endl;
  return strm;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function . */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::istream & LinearHistogramAdapter::GetFromStream( std::istream & strm )
{
  std::string type;
  strm >> type
       >> m_minValue
       >> m_maxValue
       >> m_scale
       >> m_histoSize
       >> m_bReady
       >> m_bFitBounds;
  ALIB_ASSERT( type == LINEAR_HISTOGRAM_ADAPTER );
  return strm;
}

} // namespace csutility

