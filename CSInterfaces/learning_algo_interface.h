/****************************************************************************
  learning_algo_interface.h
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "common_interface_settings.h"
#include "info_query_interface.h"
#include "debug_interface.h"

namespace csalgo
{

//=================================================================================================
/** \struct ILearningAlgorithm.
    \brief  Interface class of any learning algorithm.

  Working scenario:
  <tt><pre>
  CreateLearningAlgorithmInstance(...);
  Start(...);
  while (...);
  {
    ProcessImage(...);
  }
  Stop(...);
  Release();
  </pre></tt> */
//=================================================================================================
struct ILearningAlgorithm : public csinterface::IInformationQuery
{

//-------------------------------------------------------------------------------------------------
/** \brief Function launches computational process.

  \param  startTime  the start time of surveillance process in seconds.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT Start( IN float startTime ) = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function receives a successive image and processes it.

  \param  pBI        pointer to the image's header.
  \param  pImage     pointer to the image's beginning.
  \param  timeStamp  the current time in seconds.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT ProcessImage( IN const BITMAPINFO * pBI,
                              IN const __int8     * pImage,
                              IN float              timeStamp ) = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function finalizes computational process.

  \param  stopTime   the stop time of surveillance process in seconds.
  \param  saveResult true if algorithm's result(s) should be saved.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT Stop( IN float stopTime, IN bool saveResult ) = 0;

//-------------------------------------------------------------------------------------------------
/** \brief Function releases the implementation of this interface.

  \return  Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
virtual HRESULT Release() = 0;

};

} // namespace csinterface


///////////////////////////////////////////////////////////////////////////////////////////////////
// These functions must be exported by a DLL that implements any learning algorithm.
///////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{

//-------------------------------------------------------------------------------------------------
/** \brief Function creates the instance of any learning algorithm.

  \param  pInfo      pointer to the external object that could supply additional information.
  \param  pDebugOut  pointer to the object that prints debug information.
  \param  ppAlgo     address of pointer that will point to the instance of created object.
  \return            Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
// TODO: could be collisions if two or more instances of learning algorithms were loaded:
HRESULT CreateLearningAlgorithmInstance( IN  csinterface::IInformationQuery   * pInfo,
                                         IN  IDebugOutput                     * pDebug,
                                         OUT csinterface::ILearningAlgorithm ** ppAlgo );


//-------------------------------------------------------------------------------------------------
/** \brief Function returns the name of learning algorithm implemented in a DLL.

  \return  the name of learning algorithm. */
//-------------------------------------------------------------------------------------------------
LPCWSTR GetLearningAlgorithmName();

} // extern "C"

