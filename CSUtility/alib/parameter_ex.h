#pragma once

namespace paramlib
{

class BaseParamSection;

//=================================================================================================
/** \class BaseParamField.
    \brief Basic type for any parameter or section of parameters. */
//=================================================================================================
class /*ALIB_API*/ BaseParamField
{
  friend class BaseParamSection;

private: // prohibit copy operator and constructor
  BaseParamField & operator=( const BaseParamField & ) { return (*this); }
  BaseParamField( const BaseParamField & ) {};

public:
  enum FieldType
  {
    NOTHING  = 0,       //!< uninitialized
    SECTION  = 1,       //!< field of section type
    INTEGER  = 2,       //!< parameter of integer type
    FLOAT    = 3,       //!< parameter of float type
    BOOLEAN  = 4,       //!< parameter of boolean type
    CHAR_SET = 5,       //!< set of characters
    INT_SET  = 6,       //!< set of integers
  };   

protected:
  BaseParamField *           m_next;        //!< pointer to the next parameter of a section or 0
  std::basic_string<wchar_t> m_name;        //!< name of this parameter
  std::basic_string<wchar_t> m_comment;     //!< comment on this parameter
  FieldType                  m_type;        //!< type of this parameter
  int                        m_id;          //!< identifier of this parameter

protected:
  virtual void copy( const BaseParamField & source );
  virtual void init( const wchar_t * name, const wchar_t * comment, BaseParamSection * parent, int * id );

public:
  BaseParamField();

  virtual void set_default();

  BaseParamField * next() const { return m_next; }

  const wchar_t * name() const { return m_name.c_str(); }

  const wchar_t * comment() const { return m_comment.c_str(); }

  FieldType type() const { return m_type; }

  int id() const { return m_id; }

  virtual bool load( std::basic_iostream<wchar_t> & strm );
  virtual bool save( std::basic_iostream<wchar_t> & strm ) const;
};


//=================================================================================================
/** \class BaseParamSection.
    \brief Section is the special field that contains parameters and other sub-sections. */
//=================================================================================================
class /*ALIB_API*/ BaseParamSection : public BaseParamField
{
protected:
  friend class BaseParamField;

  BaseParamSection * m_owner;     //!< owner of this section
  BaseParamField   * m_first;     //!< pointer to the first parameter of the section
  BaseParamField   * m_last;      //!< pointer to the last parameter (NOT the next after last!)

protected:
  virtual void copy( const BaseParamSection & source );

public:
  BaseParamSection();

  void init( const wchar_t * name, const wchar_t * comment, BaseParamSection * parent, int * id = 0 );

  void set_default();

  BaseParamField * first() const { return m_first; }

  virtual bool load( std::basic_iostream<wchar_t> & strm );
  virtual bool save( std::basic_iostream<wchar_t> & strm ) const;
};


//=================================================================================================
/** \class IntParam.
    \brief Integer parameter. */
//=================================================================================================
class /*ALIB_API*/ IntParam : public BaseParamField
{
private:
  int  m_value;         //!< current value of this parameter
  int  m_default;       //!< default value
  int  m_min;           //!< minimal value
  int  m_max;           //!< maximal value

private:
  virtual void copy( const IntParam & source );

  IntParam( const IntParam & );
  IntParam & operator=( const IntParam & );

public:
  IntParam();

  /** \brief Function gets value of this parameter. */
  int operator()() const
  {
    assert( m_type != BaseParamField::NOTHING );
    return m_value;
  }

  /** \brief Function sets new value of this parameter. */
  int operator=( int newValue )
  {
    assert( m_type != BaseParamField::NOTHING );
    return (m_value = std::max( m_min, std::min( m_max, newValue ) ));
  }

  void init( const wchar_t * name, const wchar_t * comment,
             int def, int min, int max,
             BaseParamSection * parent, int * id = 0 );

  void set_default();

  virtual bool load( std::basic_iostream<wchar_t> & strm );
  virtual bool save( std::basic_iostream<wchar_t> & strm ) const;
};


//=================================================================================================
/** \class FltParam.
    \brief Float parameter. */
//=================================================================================================
class /*ALIB_API*/ FltParam : public BaseParamField
{
private:
  float  m_value;       //!< current value of this parameter
  float  m_default;     //!< default value
  float  m_min;         //!< minimal value
  float  m_max;         //!< maximal value

private:
  virtual void copy( const FltParam & source );

  FltParam( const FltParam & );
  FltParam & operator=( const FltParam & );

public:
  FltParam();

  /** \brief Function gets value of this parameter. */
  float operator()() const
  {
    assert( m_type != BaseParamField::NOTHING );
    return m_value;
  }

  /** \brief Function sets new value of this parameter. */
  float operator=(float newValue)
  {
    assert( m_type != BaseParamField::NOTHING );
    return (m_value = std::max( m_min, std::min( m_max, newValue ) ));
  }

  void init( const wchar_t * name, const wchar_t * comment,
             float def, float min, float max, BaseParamSection * parent, int * id = 0 );

  void set_default();

  virtual bool load( std::basic_iostream<wchar_t> & strm );
  virtual bool save( std::basic_iostream<wchar_t> & strm ) const;
};


//=================================================================================================
/** \class BlnParam.
    \brief Boolean parameter. */
//=================================================================================================
class /*ALIB_API*/ BlnParam : public BaseParamField
{
private:
  bool  m_value;        //!< current value of this parameter
  bool  m_default;      //!< default value

private:
  virtual void copy( const BlnParam & source );

  BlnParam( const BlnParam & );
  BlnParam & operator=( const BlnParam & );

public:
  BlnParam();

  /** \brief Function gets value of this parameter. */
  bool operator()() const
  {
    assert( m_type != BaseParamField::NOTHING );
    return m_value;
  }

  /** \brief Function sets new value of this parameter. */
  bool operator=( bool newValue )
  {
    assert( m_type != BaseParamField::NOTHING );
    return (m_value = newValue);
  }

  void init( const wchar_t * name, const wchar_t * comment,
             bool def, BaseParamSection * parent, int * id = 0 );

  void set_default();

  virtual bool load( std::basic_iostream<wchar_t> & strm );
  virtual bool save( std::basic_iostream<wchar_t> & strm ) const;
};


//=================================================================================================
/** \class ChrParam.
    \brief Character parameter that is selected from the predefined set of admissible characters. */
//=================================================================================================
class /*ALIB_API*/ ChrParam : public BaseParamField
{
private:
  wchar_t                    m_value;       //!< current value of this parameter
  wchar_t                    m_default;     //!< default value
  std::basic_string<wchar_t> m_set;         //!< set of admissible characters

private:
  virtual void copy( const ChrParam & source );

  ChrParam(const ChrParam &);
  ChrParam & operator=(const ChrParam &);

public:
  ChrParam();

  /** \brief Function gets value of this parameter. */
  wchar_t operator()() const
  {
    assert( m_type != BaseParamField::NOTHING );
    return m_value;
  }

  wchar_t operator=(wchar_t newValue);

  void init( const wchar_t * name, const wchar_t * comment,
             wchar_t def, const wchar_t * setOfChars, BaseParamSection * parent, int * id = 0 );

  void set_default();

  const std::basic_string<wchar_t> & get_char_set() const
  {
    return m_set;
  }

  virtual bool load( std::basic_iostream<wchar_t> & strm );
  virtual bool save( std::basic_iostream<wchar_t> & strm ) const;
};


//=================================================================================================
/** \class IntSetParam.
    \brief Integer parameter that is selected from the predefined set of admissible integers. */
//=================================================================================================
class /*ALIB_API*/ IntSetParam : public BaseParamField
{
private:
  int              m_value;       //!< current value of this parameter
  int              m_default;     //!< default value
  std::vector<int> m_set;         //!< set of admissible integers

private:
  virtual void copy( const IntSetParam & src );

  IntSetParam( const IntSetParam & );
  IntSetParam & operator=( const IntSetParam & );

public:
  IntSetParam();

  /** \brief Function gets value of this parameter. */
  int operator()() const
  {
    assert( m_type == BaseParamField::INT_SET );
    return m_value;
  }

  int operator=( int newValue );

  void init( const wchar_t * name, const wchar_t * comment, int def,
             const int * setOfInts, int setSize, BaseParamSection * parent, int * id = 0 );

  void set_default();

  virtual bool load( std::basic_iostream<wchar_t> & strm );
  virtual bool save( std::basic_iostream<wchar_t> & strm ) const;
};

} // namespace paramlib

