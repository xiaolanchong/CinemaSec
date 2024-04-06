#ifndef  A_WORK_SPACE_ALLOCATOR_H
#define  A_WORK_SPACE_ALLOCATOR_H

namespace matrixlib
{

//-------------------------------------------------------------------------------------------------
// Object is designed to allocate temporal working space. Allocated memory will be
// automatically released on object destruction.
//-------------------------------------------------------------------------------------------------
class WorkSpace
{
 private:
  void *  m_data;       //!< allocated storage
  size_t  m_size;       //!< the size of allocated storage in BYTES

  WorkSpace();          // no default contructor

 public:
  //-----------------------------------------------------------------------------------------------
  /** \brief Contructor allocates memory if external amount is not sufficient.

             If amount of external memory [(*F),(*L)), allocated anywhere, is not sufficient,
             then contructor allocates required amount and overwrites pointers (*F), (*L).

      \param  need  size of needed working space (in BYTES).
      \param  F     address of begin pointer that points to external memory.
      \param  L     address of end pointer that points to external memory, maybe 0. */
  //-----------------------------------------------------------------------------------------------
  WorkSpace(size_t need, void ** F, void ** L) : m_data(0), m_size(0)
  { 
    ASSERT( sizeof(char) == 1 );
    ASSERT( F != 0 );

    if (need > 0)
    {
      if (L == 0 || (*F) == 0 || (*L) == 0 || (((char*)(*F))+need) > ((char*)(*L)))
      {
        (*F) = ( m_data = malloc( m_size = need ) );
        if (L != 0)
          (*L) = (void*)( ((char*)m_data) + need );
      }
    }
  }

  //-----------------------------------------------------------------------------------------------
  /*! \brief Destructor releases allocated memory on closing. */
  //-----------------------------------------------------------------------------------------------
  ~WorkSpace()
  {
    if (m_data != 0)
      free( m_data );
  }
};

} // namespace matrixlib

#endif // A_WORK_SPACE_ALLOCATOR_H

