/****************************************************************************
  serialization.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csutility
{

enum SerializeAction
{
  SERIALIZE_SIZE,    //!< do not actual serialization, just determine the size of storage
  SERIALIZE_LOAD,    //!< load data
  SERIALIZE_SAVE     //!< save data
};

//-------------------------------------------------------------------------------------------------
/** \brief Function serializes a datum in a memory buffer.

  \param  p       pointer to the memory buffer.
  \param  x       the datum to serialize.
  \param  action  the type of operation to do (read, write or estimate size).
  \return         modified position inside the buffer. */
//-------------------------------------------------------------------------------------------------
template< class T >
inline __int8 * Serialize( __int8 * p, T & x, SerializeAction action )
{
  switch (action)
  {
    case SERIALIZE_SIZE: break;
    case SERIALIZE_LOAD: ASSERT( p != 0 );  memcpy( &x, p, sizeof(T) );  break;
    case SERIALIZE_SAVE: ASSERT( p != 0 );  memcpy( p, &x, sizeof(T) );  break;
    default            : return p;
  }
  p += sizeof(T);
  return p;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function serializes a container in a memory buffer,
           assuming that containers elements have no special serialization method.

  \param  p       pointer to the memory buffer.
  \param  x       the container to serialize.
  \param  action  the type of operation to do (read, write or estimate size).
  \return         modified position inside the buffer. */
//-------------------------------------------------------------------------------------------------
template< class CONTAINER >
inline __int8 * SerializeSimpleContainer( __int8 * p, CONTAINER & x, SerializeAction action )
{
  typedef  typename CONTAINER::value_type  value_type;

  size_t size = x.size();
  p = Serialize( p, size, action );
  ASSERT( ALIB_IS_RANGE( size, 0, (1<<24) ) );
  size_t byteSize = size * sizeof(value_type);

  switch (action)
  {
    case SERIALIZE_SIZE: break;

    case SERIALIZE_LOAD: ASSERT( p != 0 );
                         x.resize( size );
                         if (size > 0) memcpy( &(x[0]), p, byteSize );
                         break;

    case SERIALIZE_SAVE: ASSERT( p != 0 );
                         if (size > 0) memcpy( p, &(x[0]), byteSize );
                         break;

    default: return p;
  }
  p += byteSize;
  return p;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function serializes a container in a memory buffer,
           assuming that containers elements have their own serialization method.

  \param  p       pointer to the memory buffer.
  \param  x       the container to serialize.
  \param  action  the type of operation to do (read, write or estimate size).
  \return         modified position inside the buffer. */
//-------------------------------------------------------------------------------------------------
template< class CONTAINER >
inline __int8 * SerializeComplexContainer( __int8 * p, CONTAINER & x, SerializeAction action )
{
  size_t size = x.size();
  p = Serialize( p, size, action );
  ASSERT( ALIB_IS_RANGE( size, 0, (1<<24) ) );

  switch (action)
  {
    case SERIALIZE_SIZE: break;
    case SERIALIZE_LOAD: x.resize( size );  break;
    case SERIALIZE_SAVE: break;
    default            : return p;
  }

  for (typename CONTAINER::iterator it = x.begin(); it != x.end(); ++it)
  {
    p = (*it).serialize( p, action );
  }
  return p;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function serializes an image in a memory buffer.

  \param  p       pointer to the memory buffer.
  \param  x       the image to serialize.
  \param  action  the type of operation to do (read, write or estimate size).
  \return         modified position inside the buffer. */
//-------------------------------------------------------------------------------------------------
template< class CONTAINER >
inline __int8 * SerializeImage( __int8 * p, CONTAINER & x, SerializeAction action )
{
  typedef  typename CONTAINER::value_type  value_type;

  int width = x.width();
  int height = x.height();
  p = Serialize( p, width, action );
  p = Serialize( p, height, action );
  ASSERT( ALIB_IS_RANGE( width, 0, (1<<15) ) && ALIB_IS_RANGE( height, 0, (1<<15) ) );
  int byteSize = width * height * sizeof(value_type);

  switch (action)
  {
    case SERIALIZE_SIZE: break;

    case SERIALIZE_LOAD: x.resize( width, height, value_type(), false );
                         ASSERT( p != 0 );
                         if (byteSize > 0) memcpy( x.begin(), p, byteSize );
                         break;

    case SERIALIZE_SAVE: ASSERT( p != 0 );
                         if (byteSize > 0) memcpy( p, x.begin(), byteSize );
                         break;
    default : return p;
  }
  p += byteSize;
  return p;
}

} // namespace csutility

