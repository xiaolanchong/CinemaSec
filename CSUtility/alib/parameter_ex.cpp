#include "stdafx.h"

#ifdef ALIB_CHECK_MEM_LEAKAGE
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace paramlib
{

#ifdef WIN32
#define  MY_ERROR_MESSAGE_FUNCTION(t)  AfxMessageBox( t )
#else
#define  MY_ERROR_MESSAGE_FUNCTION(t)  _tprintf( t )
#endif

const wchar_t ERR_FILE[] = _T("File's structure isn't valid");
const wchar_t TABSPACE[] = _T("\t ");
const wchar_t SPACE[] = _T("  ");
const wchar_t VALUE[] = _T("value");
const wchar_t DEFAULT[] = _T("default");
const wchar_t SET[] = _T("set");
const wchar_t RANGE[] = _T("range");
const wchar_t ILLEGAL_VALUE[] = _T("\nIllegal value is used to initialize parameter:");
const wchar_t ILLEGAL_DEFAULT[] = _T("\nIllegal default value of parameter:");
const wchar_t ILLEGAL_SET[] = _T("\nIllegal set of values of parameter:");
const wchar_t DEFAULT_INSTEAD[] = _T("\nDefault value will be used instead.");

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function converts specified character to the uppercase one. */
///////////////////////////////////////////////////////////////////////////////////////////////////
inline wchar_t CharToUpperCase( wchar_t c )
{
  return (wchar_t)(((_T('a') <= c) && (c <= _T('z'))) ? ((c-_T('a'))+_T('A')) : c);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of BaseParamField
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
BaseParamField::BaseParamField()
: m_next(0), m_name(), m_comment(), m_type(NOTHING), m_id(-1)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function nothing actually copies, just checks the fields for similarity. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void BaseParamField::copy( const BaseParamField & source )
{
  source;
  assert( m_name.compare( source.m_name ) == 0 );
  assert( m_comment.compare( source.m_comment ) == 0 );
  assert( m_type == source.m_type );
  assert( m_id == source.m_id );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function connects this field with other ones of the specified section.
    \param  ps  pointer to the section that owns this field.  */
///////////////////////////////////////////////////////////////////////////////////////////////////
void BaseParamField::init( const wchar_t * name, const wchar_t * comment,
                           BaseParamSection * parent, int * id )
{
  assert( (name != 0) && (comment != 0) );
  assert( (parent != 0) || (m_type == BaseParamField::SECTION) );
  assert( m_type != BaseParamField::NOTHING );

  if (parent != 0)
  {
    assert( parent->m_type == BaseParamField::SECTION );

    if (parent->m_first == 0)
    {
      parent->m_first = this;
      parent->m_last = this;
    }
    else
    {
      parent->m_last->m_next = this;
      parent->m_last = this;
      parent->m_last->m_next = 0;
    }
  }

  m_name = name;
  m_comment = comment;

  // Replace quotation by apostrophe.
  for (size_t pos = 0; (pos = m_comment.find( _T('\"'), pos )) != m_comment.npos; pos++)
    m_comment[pos] = _T('\'');
  
#ifdef WIN32
  assert( (id != 0) || (m_type == BaseParamField::SECTION) );
  if (id != 0)
  { // Most types require one identifier for parameter itself and another one for comment control.
    m_id = (*id);
    (*id) += (m_type == BaseParamField::BOOLEAN) ? 1 : 2;
  }
#else
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function sets default value of this parameters. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void BaseParamField::set_default()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function loads this object from a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool BaseParamField::load( std::basic_iostream<wchar_t> & strm )
{
  if (!(strm.good()))
    return false;

  std::basic_string<wchar_t>  t;
  bool                      bValidType = false;

  strm >> t;
  if (t == _T("name"))
  {
    strm >> t;
    if (t == m_name)
    {
      strm >> t;
      if (t == _T("comment"))
      {
        strm >> t;
        if (!(t.empty()) && (t[0] == _T('\"')))
        {
          while (!(strm.eof()) && !(t.empty()) && (t[t.size()-1] != _T('\"')))
            strm >> t;

          strm >> t;
          if (t == _T("type"))
          {
            strm >> t;
            switch(m_type)
            {
              case SECTION : bValidType = (t == _T("SECTION"));   break;
              case INTEGER : bValidType = (t == _T("INTEGER"));   break;
              case FLOAT   : bValidType = (t == _T("FLOAT"));     break;
              case BOOLEAN : bValidType = (t == _T("BOOLEAN"));   break;
              case CHAR_SET: bValidType = (t == _T("CHAR_SET"));  break;
              case INT_SET : bValidType = (t == _T("INT_SET"));   break;
              default      : bValidType = false;                  break;
            }

            if (bValidType)
            {
              strm >> t;
              if (t == _T("id"))
              {
                int i;
                strm >> i;
                return (strm.good());
              }
            }
          }
        }
      }
    }
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function saves this object in a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool BaseParamField::save( std::basic_iostream<wchar_t> & strm ) const
{
  if (!(strm.good()))
    return false;

  strm << std::endl << std::endl;
  strm << _T("name") << TABSPACE << m_name << std::endl;
  strm << _T("comment") << TABSPACE << _T("\"") << m_comment << _T("\"") << std::endl;
  switch (m_type)
  {
    case SECTION : strm << _T("type") << TABSPACE << _T("SECTION")  << std::endl;  break;
    case INTEGER : strm << _T("type") << TABSPACE << _T("INTEGER")  << std::endl;  break;
    case FLOAT   : strm << _T("type") << TABSPACE << _T("FLOAT")    << std::endl;  break;
    case BOOLEAN : strm << _T("type") << TABSPACE << _T("BOOLEAN")  << std::endl;  break;
    case CHAR_SET: strm << _T("type") << TABSPACE << _T("CHAR_SET") << std::endl;  break;
    case INT_SET : strm << _T("type") << TABSPACE << _T("INT_SET")  << std::endl;  break;
    default      : strm << _T("type") << TABSPACE << _T("NOTHING")  << std::endl;  assert(0);  break;
  }
  strm << _T("id") << TABSPACE << m_id << std::endl;
  return (strm.good());

/*
  wchar_t buf[128];

  text += ALIB_ENDL;
  text += ALIB_ENDL;
  text += _T("name");
  text += TABSPACE;
  text += m_name;
  text += ALIB_ENDL;
  text += _T("comment");
  text += TABSPACE;
  text += _T("\"");
  text += m_comment;
  text += _T("\"");
  text += ALIB_ENDL;
  text += _T("type");
  text += TABSPACE;
  switch (m_type)
  {
    case SECTION : text += _T("SECTION");  break;
    case INTEGER : text += _T("INTEGER");  break;
    case FLOAT   : text += _T("FLOAT");    break;
    case BOOLEAN : text += _T("BOOLEAN");  break;
    case CHAR_SET: text += _T("CHAR_SET"); break;
    case INT_SET : text += _T("INT_SET");  break;
    default      : text += _T("NOTHING");  assert(0);
  }
  text += ALIB_ENDL;
  text += _T("id");
  text += TABSPACE;
  text += _itot( m_id, buf, 10 );
  text += ALIB_ENDL;
  return true;
*/
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of BaseParamSection
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
BaseParamSection::BaseParamSection() : m_owner(0), m_first(0), m_last(0)
{
  m_type = BaseParamField::SECTION;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function recursively copies this section and all its subsections from the source one. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void BaseParamSection::copy( const BaseParamSection & source )
{
  BaseParamField * dst = m_first;
  BaseParamField * src = source.m_first;

  BaseParamField::copy( source );
  while (dst != 0)
  {
    assert( (dst != m_last) || (src == source.m_last) );

    dst->copy( *src );
    dst = dst->next();
    src = src->next();
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function initializes this section.

  \param  name     name of this section.
  \param  comment  comment on this section.
  \param  owner    parent section (owner) of this section or 0 (root section).
  \param  id       identifier of this section or -1. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void BaseParamSection::init( const wchar_t * name, const wchar_t * comment,
                             BaseParamSection * parent, int * id )
{
  m_owner = parent;
  m_type = BaseParamField::SECTION;
  BaseParamField::init( name, comment, parent, id );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function sets default values of all parameters of this section and its subsections. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void BaseParamSection::set_default()
{
  assert( m_type != BaseParamField::NOTHING );

  for (BaseParamField * p = m_first; p != 0; p = p->next())
  {
    p->set_default();
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function loads this object from a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool BaseParamSection::load( std::basic_iostream<wchar_t> & strm )
{
  assert( m_type == BaseParamField::SECTION );

  for (BaseParamField * p = m_first; p != 0; p = p->next())
  {
    if (!(strm.good()) || !(p->load( strm )))
    {
      if (m_owner == 0)   // Is root section?
        set_default();

      return false;
    }
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function saves this object in a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool BaseParamSection::save( std::basic_iostream<wchar_t> & strm ) const
{
  assert( m_type == BaseParamField::SECTION );

  for (BaseParamField * p = m_first; p != 0; p = p->next())
  {
    if (!(strm.good()) || !(p->save( strm )))
      return false;
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of IntParam
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
IntParam::IntParam() : m_value(0), m_default(0), m_min(0), m_max(0)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function initializes this parameter.

  \param  name     name of this parameter.
  \param  comment  comment on this parameter.
  \param  def      default value.
  \param  min      minimal value.
  \param  max      maximal value.
  \param  parent   parent section (owner) of this parameter.
  \param  id       identifier of this field or -1. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void IntParam::init( const wchar_t * name, const wchar_t * comment,
                     int def, int min, int max, BaseParamSection * parent, int * id )
{
  m_value = ( m_default = def );
  m_min = std::min( min, max );
  m_max = std::max( min, max );
  m_type = BaseParamField::INTEGER;
  BaseParamField::init( name, comment, parent, id );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function sets default value of this parameters. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void IntParam::set_default()
{
  m_value = m_default;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function copies source object into this one. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void IntParam::copy( const IntParam & source )
{
  BaseParamField::copy( source );
  assert( m_default == source.m_default );
  assert( m_min == source.m_min );
  assert( m_max == source.m_max );
  m_value = source.m_value;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function loads this object from a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool IntParam::load( std::basic_iostream<wchar_t> & strm )
{
  if (!(BaseParamField::load( strm )))
    return false;

  int                      d, mn, mx;
  std::basic_string<wchar_t> t;

  strm >> t;
  if (t == VALUE)
  {
    strm >> m_value >> t;
    if (t == DEFAULT)
    {
      strm >> d >> t;
      if (t == RANGE)
      {
        strm >> mn >> mx;
        return (strm.good() && (d == m_default) && (mn == m_min) && (mx == m_max));
      }
    }
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function saves this object in a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool IntParam::save( std::basic_iostream<wchar_t> & strm ) const
{
  if (!(BaseParamField::save( strm )))
    return false;

  strm << VALUE   << TABSPACE << m_value   << std::endl;
  strm << DEFAULT << TABSPACE << m_default << std::endl;
  strm << RANGE   << TABSPACE << m_min << SPACE << m_max << std::endl;
  return (strm.good());

/*
  wchar_t buf[128];

  text += VALUE;
  text += TABSPACE;
  text += _itot( m_value, buf, 10 );
  text += ALIB_ENDL;

  text += DEFAULT;
  text += TABSPACE;
  text += _itot( m_default, buf, 10 );
  text += ALIB_ENDL;

  text += RANGE;
  text += TABSPACE;
  text += _itot( m_min, buf, 10 );
  text += SPACE;
  text += _itot( m_max, buf, 10 );
  text += ALIB_ENDL;
  return true;
*/
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of FltParam
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
FltParam::FltParam() : m_value(0.0f), m_default(0.0f), m_min(0.0f), m_max(0.0f)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function initializes this parameter.

  \param  name     name of this parameter.
  \param  comment  comment on this parameter.
  \param  def      default value.
  \param  min      minimal value.
  \param  max      maximal value.
  \param  parent   parent section (owner) of this parameter.
  \param  id       identifier of this field or -1. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void FltParam::init( const wchar_t * name, const wchar_t * comment,
                     float def, float min, float max, BaseParamSection * parent, int * id )
{
  m_value = ( m_default = def );
  m_min = std::min( min, max );
  m_max = std::max( min, max );
  m_type = BaseParamField::FLOAT;
  BaseParamField::init( name, comment, parent, id );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function sets default value of this parameter. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void FltParam::set_default()
{
  m_value = m_default;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function copies source object into this one. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void FltParam::copy( const FltParam & source )
{
  BaseParamField::copy( source );
  assert( m_default == source.m_default );
  assert( m_min == source.m_min );
  assert( m_max == source.m_max );
  m_value = source.m_value;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function loads this object from a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool FltParam::load( std::basic_iostream<wchar_t> & strm )
{
  if (!(BaseParamField::load( strm )))
    return false;

  float                     d, mn, mx;
  std::basic_string<wchar_t>  t;

  strm >> t;
  if (t == VALUE)
  {
    strm >> m_value >> t;
    if (t == DEFAULT)
    {
      strm >> d >> t;
      if (t == RANGE)
      {
        strm >> mn >> mx;
        return (strm.good() && (d == m_default) && (mn == m_min) && (mx == m_max));
      }
    }
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function saves this object in a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool FltParam::save( std::basic_iostream<wchar_t> & strm ) const
{
  if (!(BaseParamField::save( strm )))
    return false;

  strm << VALUE   << TABSPACE << m_value   << std::endl;
  strm << DEFAULT << TABSPACE << m_default << std::endl;
  strm << RANGE   << TABSPACE << m_min << SPACE << m_max << std::endl;
  return (strm.good());

/*
  wchar_t buf[128];

  text += VALUE;
  text += TABSPACE;
  text += _itot( m_value, buf, 10 );
  text += ALIB_ENDL;

  text += DEFAULT;
  text += TABSPACE;
  text += _itot( m_default, buf, 10 );
  text += ALIB_ENDL;

  text += RANGE;
  text += TABSPACE;
  text += _itot( m_min, buf, 10 );
  text += SPACE;
  text += _itot( m_max, buf, 10 );
  text += ALIB_ENDL;
  return true;
*/
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of BlnParam
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
BlnParam::BlnParam() : m_value(false), m_default(false)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function initializes this parameter.

  \param  name     name of this parameter.
  \param  comment  comment on this parameter.
  \param  def      default value.
  \param  parent   parent section (owner) of this parameter.
  \param  id       identifier of this field or -1. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void BlnParam::init( const wchar_t * name, const wchar_t * comment, bool def,
                     BaseParamSection * parent, int * id )
{
  m_value = (m_default = def);
  m_type = BaseParamField::BOOLEAN;
  BaseParamField::init( name, comment, parent, id );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function sets default value of this parameter. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void BlnParam::set_default()
{
  m_value = m_default;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function copies source object into this one. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void BlnParam::copy( const BlnParam & source )
{
  BaseParamField::copy( source );
  assert( m_default == source.m_default );
  m_value = source.m_value;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function loads this object from a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool BlnParam::load( std::basic_iostream<wchar_t> & strm )
{
  if (!(BaseParamField::load( strm )))
    return false;

  int                       v, d;
  std::basic_string<wchar_t>  t;

  strm >> t;
  if (t == VALUE)
  {
    strm >> v >> t;
    if (t == DEFAULT)
    {
      strm >> d;
      m_value = (v != 0);
      return (strm.good() && ((d != 0) == m_default));
    }
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function saves this object in a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool BlnParam::save( std::basic_iostream<wchar_t> & strm ) const
{
  if (!(BaseParamField::save( strm )))
    return false;

  strm << VALUE   << TABSPACE << (int)(m_value   ? 1 : 0) << std::endl;
  strm << DEFAULT << TABSPACE << (int)(m_default ? 1 : 0) << std::endl;
  return (strm.good());
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of ChrParam
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
ChrParam::ChrParam() : m_value(0), m_default(0), m_set()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function sets new value of this parameter. */
///////////////////////////////////////////////////////////////////////////////////////////////////
wchar_t ChrParam::operator=( wchar_t newValue )
{
  assert( m_type != BaseParamField::NOTHING );

  newValue = CharToUpperCase( newValue );
  if (m_set.find( newValue ) == std::basic_string<wchar_t>::npos)
  {
    std::basic_string<wchar_t> txt;
    txt = ILLEGAL_VALUE;
    txt += m_name;
    txt += DEFAULT_INSTEAD;
    MY_ERROR_MESSAGE_FUNCTION( txt.c_str() );
    newValue = m_default;
  }
  return (m_value = newValue);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function initializes this parameter.

  \param  name        name of this parameter.
  \param  comment     comment on this parameter.
  \param  def         default value.
  \param  setOfChars  set of admissible characters.
  \param  parent      parent section (owner) of this parameter.
  \param  id          identifier of this field or -1. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void ChrParam::init( const wchar_t * name, const wchar_t * comment,
                     wchar_t def, const wchar_t * setOfChars,
                     BaseParamSection * parent, int * id )
{
  if ((setOfChars == 0) || (_tcslen(setOfChars) == 0))
  {
    std::basic_string<wchar_t> txt;
    txt = ILLEGAL_SET;
    txt += m_name;
    MY_ERROR_MESSAGE_FUNCTION( txt.c_str() );
    m_set = _T("ERROR");
  }
  else m_set = setOfChars;

  _tcsupr( const_cast<wchar_t*>( m_set.c_str() ) );
  if (std::find( m_set.begin(), m_set.end(), def ) == m_set.end())
  {
    std::basic_string<wchar_t> txt;
    txt = ILLEGAL_DEFAULT;
    txt += m_name;
    MY_ERROR_MESSAGE_FUNCTION( txt.c_str() );
    def = _T('?');
  }

  m_value = (m_default = (def = CharToUpperCase( def )));
  m_type = BaseParamField::CHAR_SET;
  BaseParamField::init( name, comment, parent, id );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function sets default value of this parameter. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void ChrParam::set_default()
{
  m_value = m_default;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function copies source object into this one. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void ChrParam::copy( const ChrParam & source )
{
  BaseParamField::copy( source );
  assert( m_default == source.m_default );
  assert( m_set.compare( source.m_set ) == 0 );
  m_value = source.m_value;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function loads this object from a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ChrParam::load( std::basic_iostream<wchar_t> & strm )
{
  if (!(BaseParamField::load( strm )))
    return false;

  wchar_t                     d;
  std::basic_string<wchar_t>  t;

  strm >> t;
  if (t == VALUE)
  {
    strm >> m_value >> t;
    if (t == DEFAULT)
    {
      strm >> d >> t;
      if (t == SET)
      {
        strm >> t;
        return (strm.good() && (d == m_default) && (t == m_set));
      }
    }
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function saves this object in a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ChrParam::save( std::basic_iostream<wchar_t> & strm ) const
{
  if (!(BaseParamField::save( strm )))
    return false;

  strm << VALUE   << TABSPACE << m_value   << std::endl;
  strm << DEFAULT << TABSPACE << m_default << std::endl;
  strm << SET     << TABSPACE << m_set << std::endl;
  return (strm.good());
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of IntSetParam
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
IntSetParam::IntSetParam() : m_value(0), m_default(0), m_set()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function sets new value of this parameter. */
///////////////////////////////////////////////////////////////////////////////////////////////////
int IntSetParam::operator=( int newValue )
{
  assert( m_type == BaseParamField::INT_SET );

  if (std::find( m_set.begin(), m_set.end(), newValue ) == m_set.end())
  {
    std::basic_string<wchar_t> txt;
    txt = ILLEGAL_VALUE;
    txt += m_name;
    txt += DEFAULT_INSTEAD;
    MY_ERROR_MESSAGE_FUNCTION( txt.c_str() );
    newValue = m_default;
  }
  return (m_value = newValue);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function initializes this parameter.

  \param  name       name of this parameter.
  \param  comment    comment on this parameter.
  \param  def        default value.
  \param  setOfInts  set of admissible integers.
  \param  setSize    size of the set.
  \param  parent     parent section (owner) of this parameter.
  \param  id         identifier of this field or -1. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void IntSetParam::init( const wchar_t * name, const wchar_t * comment,
                        int def, const int * setOfInts, int setSize,
                        BaseParamSection * parent, int * id )
{
  if ((setOfInts == 0) || (setSize <= 0))
  {
    std::basic_string<wchar_t> txt;
    txt = ILLEGAL_SET;
    txt += m_name;
    MY_ERROR_MESSAGE_FUNCTION( txt.c_str() );
    m_set.push_back( 0 );
  }
  else m_set.assign( setOfInts, setOfInts+setSize );

  if (std::find( m_set.begin(), m_set.end(), def ) == m_set.end())
  {
    std::basic_string<wchar_t> txt;
    txt = ILLEGAL_DEFAULT;
    txt += m_name;
    MY_ERROR_MESSAGE_FUNCTION( txt.c_str() );
    def = 0;
  }

  m_value = ( m_default = def );
  m_type = BaseParamField::INT_SET;
  BaseParamField::init( name, comment, parent, id );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function sets default value of this parameter. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void IntSetParam::set_default()
{
  m_value = m_default;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function copies source object into this one. */
///////////////////////////////////////////////////////////////////////////////////////////////////
void IntSetParam::copy( const IntSetParam & src )
{
  BaseParamField::copy( src );
  assert( m_default == src.m_default );
  assert( m_set.size() == src.m_set.size() );
  assert( (std::mismatch( m_set.begin(), m_set.end(), src.m_set.begin() )).first == m_set.end() );
  m_value = src.m_value;
}
 

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function loads this object from a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool IntSetParam::load( std::basic_iostream<wchar_t> & strm )
{
  if (!(BaseParamField::load( strm )))
    return false;

  int                       n, d, s;
  std::basic_string<wchar_t>  t;

  strm >> t;
  if (t == VALUE)
  {
    strm >> m_value >> t;
    if (t == DEFAULT)
    {
      strm >> d >> t;
      if (t == SET)
      {
        for (n = 0; n < (int)(m_set.size()); n++)
        {
          strm >> s;
          if (s != m_set[n])
            break;
        }
        return (strm.good() && (d == m_default) && (n == (int)(m_set.size())));
      }
    }
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function saves this object in a stream. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool IntSetParam::save( std::basic_iostream<wchar_t> & strm ) const
{
  if (!(BaseParamField::save( strm )))
    return false;

  strm << VALUE   << TABSPACE << m_value   << std::endl;
  strm << DEFAULT << TABSPACE << m_default << std::endl;
  strm << SET     << TABSPACE;

  for (int i = 0; i < (int)(m_set.size()); i++)
    strm << m_set[i] << SPACE;

  strm << std::endl;
  return (strm.good());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#undef MY_ERROR_MESSAGE_FUNCTION

} // namespace paramlib


