/****************************************************************************
  CSOnlineWriter.h
  ---------------------
  begin     : 30 Aug 2005
  modified  : 30 Aug 2005
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

extern "C"
{

typedef HRESULT (*PCreateLearningAlgorithmInstance)( csinterface::IInformationQuery   * pInfo,
                                                     IDebugOutput                     * pDebug,
                                                     csinterface::ILearningAlgorithm ** ppAlgo );

typedef LPCWSTR (*PGetLearningAlgorithmName)();


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

