/****************************************************************************
  background_accum_interface.h
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \class IBackgroundAccumulator.
    \brief Interface class of any background accumulator. 

  Working scenario:
  <tt><pre>
  CreateBackgroundAccumulator(...);
  Start(...);
  while (...);
  {
    ProcessImage(...);
  }
  Stop();
  Release();
  </pre></tt> */
//=================================================================================================
struct IBackgroundAccumulator
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Начало работы аккумулятора.

  \return  Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT Start() = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Передать очередное изображение для аккумулятора и обработать его.

  \param  pBI     указатель на заголовок изображения.
  \param  pBytes  указатель на начало изображения.
  \return         Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT ProcessImage( IN const BITMAPINFO * pBI,
                              IN const void       * pBytes ) = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Остановка работы аккумулятора.

  \param  ppBackground  the address of pointer that will point to the internal resultant background.
  \return               Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT Stop( OUT const Arr2f ** ppBackground ) = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function releases interface instance.

  \return  Ok = S_OK. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual HRESULT Release() = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function stores pointer to an object that prints debug information.

  \param  pDebug  pointer to the debug output object. */
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual void SetDebugOutput( IDebugOutput * pDebug ) = 0;

};

