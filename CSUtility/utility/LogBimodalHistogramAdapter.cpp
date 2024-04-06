/****************************************************************************
  LogBimodalHistogramAdapter.cpp
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

const char LOG_BIMODAL_HISTOGRAM_ADAPTER[] = "Log_Bimodal_Histogram_Adapter";

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
LogBimodalHistogramAdapter::LogBimodalHistogramAdapter()
{
  Clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function . */
///////////////////////////////////////////////////////////////////////////////////////////////////
void LogBimodalHistogramAdapter::Clear()
{
  m_A = 0.0;
  m_B = 0.0;
  m_minValue = 0.0;
  m_maxValue = 0.0;
  m_histoSize = 0;
  m_bReady = false;
  m_bFitBounds = true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function . */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LogBimodalHistogramAdapter::Initialize( double mean1, double dev1,
                                             double mean2, double dev2,
                                             int histoSize, bool bFitBounds )
{
  const double MULT = 3.5;
  if ((histoSize > 0) && (dev1 > (FLT_MIN/FLT_EPSILON)) && (dev2 > (FLT_MIN/FLT_EPSILON)))
  {
    m_minValue   = std::min( mean1-MULT*dev1, mean2-MULT*dev2 );
    m_maxValue   = std::max( mean1+MULT*dev1, mean2+MULT*dev2 );
    m_B          = 1.0 - m_minValue;
    m_A          = (double)(histoSize-1) / log( m_B + m_maxValue );
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
LogBimodalHistogramAdapter & LogBimodalHistogramAdapter::operator=( const LogBimodalHistogramAdapter & source )
{
  m_A          = source.m_A;
  m_B          = source.m_B;
  m_minValue   = source.m_minValue;
  m_maxValue   = source.m_maxValue;
  m_histoSize  = source.m_histoSize;
  m_bReady     = source.m_bReady;
  m_bFitBounds = source.m_bFitBounds;
  return (*this);
}
*/


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function . */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LogBimodalHistogramAdapter::operator==( const LogBimodalHistogramAdapter & second )
{
  return ((m_A          == second.m_A)         &&
          (m_B          == second.m_B)         &&
          (m_minValue   == second.m_minValue)  &&
          (m_maxValue   == second.m_maxValue)  &&
          (m_histoSize  == second.m_histoSize) &&
          (m_bReady     == second.m_bReady)    &&
          (m_bFitBounds == second.m_bFitBounds ));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function . */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::ostream & LogBimodalHistogramAdapter::PutToStream( std::ostream & strm ) const
{
  strm << LOG_BIMODAL_HISTOGRAM_ADAPTER << std::endl
       << m_A                           << std::endl
       << m_B                           << std::endl
       << m_minValue                    << std::endl
       << m_maxValue                    << std::endl
       << m_histoSize                   << std::endl
       << m_bReady                      << std::endl
       << m_bFitBounds                  << std::endl;
  return strm;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function . */
///////////////////////////////////////////////////////////////////////////////////////////////////
std::istream & LogBimodalHistogramAdapter::GetFromStream( std::istream & strm )
{
  std::string type;
  strm >> type
       >> m_A
       >> m_B
       >> m_minValue
       >> m_maxValue
       >> m_histoSize
       >> m_bReady
       >> m_bFitBounds;
  ALIB_ASSERT( type == LOG_BIMODAL_HISTOGRAM_ADAPTER );
  return strm;
}

} // namespace csutility

