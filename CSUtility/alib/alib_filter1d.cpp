#include "stdafx.h"

#ifdef ALIB_CHECK_MEMORY_LEAKAGE
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace alib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of base filter class.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
/** \brief Contructor. */
//-------------------------------------------------------------------------------------------------
Filter1D::Filter1D()
{
  clear();
  m_type = NO_TYPE;
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
Filter1D::~Filter1D()
{
  clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function clears this object. */
//-------------------------------------------------------------------------------------------------
void Filter1D::clear()
{
  m_filter.clear();
  m_param = 0.0;
  m_bCreated = false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function creates filter according to specified parameter value.
  \param  param  filter's parameter.
  \return        Ok = true; */
//-------------------------------------------------------------------------------------------------
bool Filter1D::create(double param)
{
  param = validate_parameter( param );
  if (!(m_filter.empty()) && (fabs(param - m_param) < FLT_EPSILON))
    return true;

  clear();
  m_param = param;
  m_filter.resize( calculate_filter_size() );

  double step = 0.01;
  double halfStep = 0.5*step;
  double scale = 0.0;
  int    k, size = (int)(m_filter.size());

  for(k = 0; k < size; k++)
  {
    double x1 = k-0.5;
    double x2 = k+0.5-FLT_EPSILON;
    double sum = 0.0;

    for(double x = x1; x < x2; x += step)
    {
      double t = kernel( x+halfStep );
      sum += t;
      if (k > 0 || x >= 0.0)
        scale += fabs(t);
    }
    m_filter[k] = (float)sum;
  }

  scale = (scale > FLT_EPSILON) ? (normalization_constant()/scale) : 0.0;
  for(k = 0; k < size; k++)
  {
    m_filter[k] *= (float)scale;
  }
  return !(m_filter.empty());
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of 1D Gauss filter.
///////////////////////////////////////////////////////////////////////////////////////////////////

GaussFilter1D::GaussFilter1D()
{
  clear();
  m_type = GAUSS;
}


int GaussFilter1D::calculate_filter_size() const
{
  return std::max<int>( 2, (int)ceil( 3.0*m_param ) );
}


double GaussFilter1D::validate_parameter(double param) const
{
  return ALIB_LIMIT( param, 0.25, 100.0 );
}


double GaussFilter1D::normalization_constant() const
{
  return 0.5;
}


double GaussFilter1D::kernel(double x) const
{
  x /= m_param;
  return exp( -0.5*x*x );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of 1D Gauss filter with shrinkage compensation.
///////////////////////////////////////////////////////////////////////////////////////////////////

GaussCompensFilter1D::GaussCompensFilter1D()
{
  clear();
  m_type = GAUSS_COMPENSATED;
}


int GaussCompensFilter1D::calculate_filter_size() const
{
  return std::max<int>( 2, (int)ceil( 4.1*m_param ) );
}


double GaussCompensFilter1D::validate_parameter(double param) const
{
  return /*(27.0/16.0)*/ ALIB_LIMIT( param, 0.5, 100.0 );
}


double GaussCompensFilter1D::normalization_constant() const
{
  return 0.5709158135;
}


double GaussCompensFilter1D::kernel(double x) const
{
  double t = x/m_param;
  return (3.0-t*t)*exp( -0.5*t*t );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of 1D Gauss differentiating filter.
///////////////////////////////////////////////////////////////////////////////////////////////////

GaussDiffFilter1D::GaussDiffFilter1D()
{
  clear();
  m_type = GAUSS_DIFF;
}


int GaussDiffFilter1D::calculate_filter_size() const
{
  return std::max<int>( 2, (int)ceil(3.0*m_param) );
}


double GaussDiffFilter1D::validate_parameter(double param) const
{
  return ALIB_LIMIT( param, 0.25, 100.0 );
}


double GaussDiffFilter1D::normalization_constant() const
{
  return ( 1.0/(m_param*sqrt( 2.0*ALIB_PI )) );
}


double GaussDiffFilter1D::kernel(double x) const
{
  x /= m_param;
  return ( x*exp( -0.5*x*x ) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of 1D Gauss curvature filter.
///////////////////////////////////////////////////////////////////////////////////////////////////

GaussCurvatureFilter1D::GaussCurvatureFilter1D()
{
  clear();
  m_type = GAUSS_CURVATURE;
}


int GaussCurvatureFilter1D::calculate_filter_size() const
{
  return std::max<int>( 2, (int)ceil( 6.0*m_param ) );
}


double GaussCurvatureFilter1D::validate_parameter(double param) const
{
  return ALIB_LIMIT( param, 0.25, 100.0 );
}


double GaussCurvatureFilter1D::normalization_constant() const
{
  return (0.4839414489/ALIB_SQR( m_param ));
}


double GaussCurvatureFilter1D::kernel(double x) const
{
  x /= m_param;
  x = x*x;
  return ((x-1.0)*exp( -0.5*x ));
}

} // namespace alib
