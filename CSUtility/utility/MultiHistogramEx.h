/****************************************************************************
  MultiHistogramEx.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev, A.Boltnev.
  email     : Alexander.Boltnev@biones.com, Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csutility
{

//=================================================================================================
/** \class MultiHistogramEx.
    \brief Class implements multi-entry histogram. */
//=================================================================================================
template < class VALUE_TYPE, int LAYER_NUMBER, class ADAPTER >
class MultiHistogramEx
{
public:
  struct Entry
  {
    VALUE_TYPE layer[LAYER_NUMBER];
  };

  typedef  std::vector<Entry>                                 EntryArr;
  typedef  MultiHistogramEx<VALUE_TYPE,LAYER_NUMBER,ADAPTER>  this_type;
  typedef  VALUE_TYPE                                         value_type;
  typedef  ADAPTER                                            adapter_type;

private:
  EntryArr     m_histogram;    //!< 
  bool         m_bNormalized;  //!< 
  adapter_type m_adapter;      //!< 

public:

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool FillZero()
{
  for (EntryArr::iterator it = m_histogram.begin(); it != m_histogram.end(); ++it)
  {
    for (int i = 0; i < LAYER_NUMBER; i++)
      (*it).layer[i] = (value_type)0;
  }
  m_bNormalized = false;
  return false;
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
bool Initialize( const adapter_type & a )
{
  m_bNormalized = false;
  if (a.Ready())
  {
    m_histogram.resize( a.HistogramSize() );
    m_adapter = a;
    FillZero();
    return true;
  }
  m_histogram.clear();
  m_adapter.Clear();
  return false;
}

//-------------------------------------------------------------------------------------------------
/** \brief adds value into the Histogram; returns error if cannot add the value. */
//-------------------------------------------------------------------------------------------------
bool AddValue( value_type value, unsigned int layerNo )
{
  if (!m_bNormalized && m_adapter.Ready() && (layerNo < LAYER_NUMBER))
  {
    int i = m_adapter.EntryIndex( value );
    if (i >= 0)
      (m_histogram[i]).layer[layerNo] += (value_type)1;
    return true;
  }
  return false;
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
  ASSERT( m_histogram.size() == source.m_histogram.size() );
  ASSERT( m_adapter == source.m_adapter );
  ASSERT( !m_bNormalized && !(source.m_bNormalized) );

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
  m_histogram   = source.m_histogram;
  m_adapter     = source.m_adapter;   
  m_bNormalized = source.m_bNormalized;
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
      (*it).layer[i] = (value_type)((*it).layer[i] * scale[i]);
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
MultiHistogramEx( const adapter_type & a )
{
  ASSERT( LAYER_NUMBER > 0 );
  ASSERT( std::numeric_limits<value_type>::is_specialized );
  ASSERT( std::numeric_limits<value_type>::is_integer == false );

  Initialize( a );
}

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
MultiHistogramEx()
{
  ASSERT( LAYER_NUMBER > 0 );
  ASSERT( std::numeric_limits<value_type>::is_specialized );
  ASSERT( std::numeric_limits<value_type>::is_integer == false );

  m_histogram.clear();
  m_adapter.Clear();
  m_bNormalized = false;
};

//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
~MultiHistogramEx()
{
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
adapter_type GetAdapter() const
{
  return m_adapter;
}

//-------------------------------------------------------------------------------------------------
/** \brief . */
//-------------------------------------------------------------------------------------------------
void GetLayer( int layerNo, std::vector<value_type> & layer ) const
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
void SetLayer( int layerNo, const std::vector<value_type> & layer )
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
    m_adapter.GetFromStream( strm );
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
    const int NDIGITS = (int)(std::numeric_limits<value_type>::digits10);

    m_adapter.PutToStream( strm ) << std::endl;
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

