/****************************************************************************
  LearningFinalizer.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

//=================================================================================================
/** \class LearningFinalizer.
    \brief LearningFinalizer. */
//=================================================================================================
class LearningFinalizer
{
private:
  void ProcessStaticData( const FloatArr & staticEmpty,
                          const FloatArr & staticClothes,
                          const FloatArr & staticHuman,
                          const wchar_t  * resultPath );

  void ProcessDynamicData( const FloatArr & dynamicThing,
                           const FloatArr & dynamicHuman,
                           const wchar_t  * resultPath );

public:
  LearningFinalizer();
  virtual ~LearningFinalizer();
  bool Process( const FloatArr            & staticEmpty,
                const FloatArr            & staticClothes,
                const FloatArr            & staticHuman,
                const FloatArr            & dynamicThing,
                const FloatArr            & dynamicHuman,
                const wchar_t             * resultPath,
                csinterface::IDebugOutput * pOutput = 0 );
};

