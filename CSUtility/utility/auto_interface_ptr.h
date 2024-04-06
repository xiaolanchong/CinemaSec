/****************************************************************************
  AutoInterfacePtr.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csutility
{

//=================================================================================================
/** \class AutoInterfacePtr.
    \brief Fully analogous to std::auto_ptr<T>,
           the class wraps an interface pointer to ensure destruction. */
//=================================================================================================
template< class TYPE >
class AutoInterfacePtr
{
private:
  TYPE * m_ptr;  //!< the wrapped object pointer

public:
  typedef TYPE                    element_type;
  typedef AutoInterfacePtr<TYPE>  this_type;

  explicit AutoInterfacePtr( TYPE * p = 0 ) throw() : m_ptr(p)
  {
  }

  // construct by assuming pointer from x AutoInterfacePtr
  AutoInterfacePtr( this_type & x ) throw() : m_ptr( x.release() )
  {
  }

  // convert to compatible AutoInterfacePtr
  template< class OTHER >
  operator AutoInterfacePtr<OTHER>() throw()
  {
    return (AutoInterfacePtr<OTHER>( *this ));
  }

  // assign compatible x (assume pointer)
  template< class OTHER >
  this_type & operator=( AutoInterfacePtr<OTHER> & x ) throw()
  {
    reset( x.release() );
    return (*this);
  }

  // construct by assuming pointer from x
  template< class OTHER >
  AutoInterfacePtr( AutoInterfacePtr<OTHER> & x ) throw() : m_ptr( x.release() )
  {
  }

  // assign compatible x (assume pointer)
  this_type & operator=( this_type & x ) throw()
  {
    reset( x.release() );
    return (*this);
  }

  // destroy the object
  ~AutoInterfacePtr()
  {
    if (m_ptr != 0)
      m_ptr->Release();
  }

  // return designated value
  TYPE & operator*() const throw()
  {
    ASSERT( m_ptr != 0 );
    return (*m_ptr);
  }

  // return pointer to class object
  TYPE * operator->() const throw()
  {
    ASSERT( m_ptr != 0 );
    return (m_ptr); // (&**this);
  }

  // return wrapped pointer
  TYPE * get() const throw()
  {
    return (m_ptr);
  }

  // return wrapped pointer and give up ownership
  TYPE * release() throw()
  {
    TYPE * t = m_ptr;
    m_ptr = 0;
    return (t);
  }

  // destroy designated object and store new pointer
  void reset( TYPE * p = 0 )
  {
    if ((p != m_ptr) && (m_ptr != 0))
      m_ptr->Release();
    m_ptr = p;
  }

  bool null() const
  {
    return (m_ptr == 0);
  }
};

} // namespace csutility


