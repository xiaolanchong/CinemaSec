/****************************************************************************
  multi_histogram.h
  ---------------------
  begin     : Aug 2004
  author(s) : A.Boltnev, Albert Akhriev
  email     : Alexander.Boltnev@biones.com, Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csutility
{

enum LayerName
{
  CCT_EMPTY = 0,    //!< chair contents type - empty
  CCT_HUMAN,        //!< chair contents type - human
  CCT_CLOTH         //!< chair contents type - clothes
};

//=================================================================================================
/** \class MultiHistogram.
    \brief Class implements multi-entry histogram. */
//=================================================================================================
template < class VALUE_TYPE, int LAYER_NUMBER >
class MultiHistogram
{
public:
  struct Entry
  {
    VALUE_TYPE layer[LAYER_NUMBER];
  };

  typedef  std::vector<Entry>                       EntryArr;
  typedef  MultiHistogram<VALUE_TYPE,LAYER_NUMBER>  this_type;

private:
  EntryArr   m_histogram;    //!< 
  VALUE_TYPE m_minValue;     //!< 
  VALUE_TYPE m_maxValue;     //!< 
  VALUE_TYPE m_interval;     //!< 
  bool       m_bNormalized;  //!< 

public:

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool FillZero()
{
  for (EntryArr::iterator it = m_histogram.begin(); it != m_histogram.end(); ++it)
  {
    for (int i = 0; i < LAYER_NUMBER; i++)
      (*it).layer[i] = (VALUE_TYPE)0;
  }
  m_bNormalized = false;
  return false;
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
void Initialize( VALUE_TYPE minValue, VALUE_TYPE maxValue, int nIntervals )
{
  ASSERT( (nIntervals > 0) && (minValue < maxValue) );
  m_histogram.resize( nIntervals );
  FillZero();
  this->m_minValue = minValue;
  this->m_maxValue = maxValue;
  this->m_interval = (VALUE_TYPE)((maxValue - minValue) / (double)nIntervals);
  ASSERT( m_interval > (FLT_MIN/(FLT_EPSILON*FLT_EPSILON)) );
  m_bNormalized = false;
}

//-------------------------------------------------------------------------------------------------
/** \brief adds value into the Histogram; returns error if cannot add the value. */
//-------------------------------------------------------------------------------------------------
bool AddValue( VALUE_TYPE value, unsigned int layerNo )
{
  if (m_bNormalized || m_histogram.empty())
    return false;
  if ((value < m_minValue) || (value > m_maxValue) || (layerNo >= (unsigned int)LAYER_NUMBER))
    return false;

  int e = (int)((value - m_minValue) / m_interval);
  e = std::min<int>( e, (int)(m_histogram.size())-1 );
  e = std::max<int>( e, 0 );
  (m_histogram[e]).layer[layerNo] += (VALUE_TYPE)1;
  return true;
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
int Size() const
{
  return (int)(m_histogram.size());
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
const Entry & operator [] ( unsigned int index ) const
{
  ASSERT( m_bNormalized && (index < (unsigned int)(m_histogram.size())) );
  return m_histogram[index];
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
this_type & operator += ( const this_type & source )
{
  ASSERT( this->m_histogram.size() == source.m_histogram.size() );
  ASSERT( fabs( this->m_minValue - source.m_minValue ) < 3.0*std::numeric_limits<VALUE_TYPE>::epsilon() );
  ASSERT( fabs( this->m_maxValue - source.m_maxValue ) < 3.0*std::numeric_limits<VALUE_TYPE>::epsilon() );
  ASSERT( fabs( this->m_interval - source.m_interval ) < 3.0*std::numeric_limits<VALUE_TYPE>::epsilon() );
  ASSERT( !m_bNormalized );
  ASSERT( !(source.m_bNormalized) );

  for (size_t e = 0; e < m_histogram.size(); e++)
  {
    const Entry & src = source.m_histogram[e];
    Entry       & dst = m_histogram[e];

    for (int i = 0; i < LAYER_NUMBER; i++)
      dst.layer[i] += src.layer[i];
  }
  return (*this);
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
this_type & operator = ( const this_type & source )
{
  this->m_histogram   = source.m_histogram;
  this->m_minValue    = source.m_minValue;   
  this->m_maxValue    = source.m_maxValue;   
  this->m_interval    = source.m_interval;   
  this->m_bNormalized = source.m_bNormalized;
  return (*this);
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
void Normilize()
{
  EntryArr::iterator it;
  long double        scale[LAYER_NUMBER];

  ASSERT( !m_bNormalized );
  std::fill( scale, scale+LAYER_NUMBER, long double(0) );

  for (it = m_histogram.begin(); it != m_histogram.end(); ++it)
  {
    for (int i = 0; i < LAYER_NUMBER; i++)
      scale[i] += (*it).layer[i];
  }

  for (int i = 0; i < LAYER_NUMBER; i++)
    scale[i] = (scale[i] > 0.5) ? (1.0/scale[i]) : 0.0;

  for (it = m_histogram.begin(); it != m_histogram.end(); ++it)
  {
    for (int i = 0; i < LAYER_NUMBER; i++)
      (*it).layer[i] = (VALUE_TYPE)((*it).layer[i] * scale[i]);
  }
  m_bNormalized = true;
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool IsNormalized() const
{
  return m_bNormalized;
}

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
MultiHistogram( VALUE_TYPE minValue, VALUE_TYPE maxValue, int nIntervals )
{
  ASSERT( LAYER_NUMBER > 0 );
  ASSERT( std::numeric_limits<VALUE_TYPE>::is_specialized );
  ASSERT( std::numeric_limits<VALUE_TYPE>::is_integer == false );

  Initialize( minValue, maxValue, nIntervals );
}

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
MultiHistogram()
{
  ASSERT( LAYER_NUMBER > 0 );
  ASSERT( std::numeric_limits<VALUE_TYPE>::is_specialized );
  ASSERT( std::numeric_limits<VALUE_TYPE>::is_integer == false );

  m_histogram.clear();
  m_minValue = (VALUE_TYPE)0;
  m_maxValue = (VALUE_TYPE)0;
  m_interval = (VALUE_TYPE)0;
  m_bNormalized = false;
};

//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
~MultiHistogram()
{
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
void GetLayer( int layerNo, std::vector<VALUE_TYPE> & layer ) const
{
  ALIB_ASSERT( (0 <= layerNo) && (layerNo < LAYER_NUMBER) );
  layer.resize( m_histogram.size() );
  for (int i = 0, n = (int)(layer.size()); i < n; i++)
  {
    layer[i] = (m_histogram[i]).layer[layerNo];
  }
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
void SetLayer( int layerNo, const std::vector<VALUE_TYPE> & layer )
{
  ALIB_ASSERT( (0 <= layerNo) && (layerNo < LAYER_NUMBER) );
  ALIB_ASSERT( layer.size() == m_histogram.size() );
  for (int i = 0, n = (int)(layer.size()); i < n; i++)
  {
    (m_histogram[i]).layer[layerNo] = layer[i];
  }
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool Load( std::istream & strm )
{
  if (strm.good())
  {
    strm >> m_minValue;
    strm >> m_maxValue;
    strm >> m_interval;
    strm >> m_bNormalized;

    int size = 0;
    strm >> size;
    if ((size < 0) || (size > SHRT_MAX))
      return FillZero();
    m_histogram.resize( size );

    for (EntryArr::iterator it = m_histogram.begin(); it != m_histogram.end(); ++it)
    {
      size_t index;
      strm >> index;
      for (int i = 0; i < LAYER_NUMBER; i++) strm >> (*it).layer[i];
    }
  }
  return ((strm.good() || strm.eof()) ? true : FillZero());
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool Save( std::ostream & strm ) const
{
  if (strm.good())
  {
    const int NDIGITS = (int)(std::numeric_limits<VALUE_TYPE>::digits10);

    strm << std::setprecision( NDIGITS ) << m_minValue << std::endl;
    strm << std::setprecision( NDIGITS ) << m_maxValue << std::endl;
    strm << std::setprecision( NDIGITS ) << m_interval << std::endl;
    strm << m_bNormalized << std::endl;

    strm << (int)(m_histogram.size()) << std::endl << std::endl;
    int index = 0;
    for (EntryArr::const_iterator it = m_histogram.begin(); it != m_histogram.end(); ++it)
    {
      strm << std::setw(7) << index++;
      for (int i = 0; i < LAYER_NUMBER; i++)
        strm << "  " << std::setw( NDIGITS+10 ) << std::setprecision( NDIGITS ) << (*it).layer[i];
      strm << std::endl;
    }
    strm << std::endl;
  }
  return (strm.good());
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool SaveInGNUPlotFormat( const char * fname, const char * label[LAYER_NUMBER], const char * func = 0 ) const
{
  if (fname == 0) 
    return false;

  std::fstream file( fname, std::ios::trunc | std::ios::out );
  if (!(file.good()))
    return false;

  file << "plot";
  if (func != 0)
    file << " " << func << ",";

  for (int i = 0; i < LAYER_NUMBER; i++)
  {
    file << " '-' title \"" << ((label[i] != 0) ? label[i] : "noname")
         << "\"" << (((i+1) < LAYER_NUMBER) ? "," : "");
  }
  file << std::endl;

  for (int i = 0; i < LAYER_NUMBER; i++)
  {
    int index = 0;
    for (EntryArr::const_iterator it = m_histogram.begin(); it != m_histogram.end(); ++it)
    {
      file << (index++) << "   " << ((*it).layer[i]) << std::endl;
    }
    file << "e" << std::endl;
  }
  return (file.good());
}

};

} // namespace csutility

