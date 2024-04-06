/******************************************************************************
  alib_exception.h
  ---------------------
  begin     : Apr 2005
  modified  : 6 Sep 2005
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  email     : aaah@mail.ru
******************************************************************************/

#pragma once

namespace alib
{

//=================================================================================================
/** \class exception.
    \brief Exception class that supports wide characters. */
//=================================================================================================
template< class CHAR_TYPE >
class exception
{
public:
  typedef  CHAR_TYPE  char_type;

private:
  StdStr m_message;

public:
  /** \brief Constructor. */
  explicit exception( const CHAR_TYPE * msg )
  {
    if (msg != 0) m_message = msg;
  }

  /** \brief Constructor. */
  explicit exception( const std::basic_string<CHAR_TYPE> & msg )
  {
    m_message = msg;
  }

  /** \brief Destructor. */
  virtual ~exception()
  {
  }

  /** \brief Function returns pointer to message string. */
  virtual const CHAR_TYPE * what() const throw()
  {
    return (m_message.c_str());
  }
};

} // namespace alib

