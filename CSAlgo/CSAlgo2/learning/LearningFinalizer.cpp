/****************************************************************************
  LearningFinalizer.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"

const int     STATIC_HISTO_SIZE = (1<<8);
const int     DYNAMIC_HISTO_SIZE = (1<<8);
const wchar_t STATIC_HISTO_NAME[] = L"csalgo2_static.histo";
const wchar_t DYNAMIC_HISTO_NAME[] = L"csalgo2_dynamic.histo";
const wchar_t STATIC_THRESHOLD_NAME[] = L"csalgo2_static_threshold.txt";
const wchar_t DYNAMIC_THRESHOLD_NAME[] = L"csalgo2_dynamic_threshold.txt";
const wchar_t STATIC_GNUPLOT_HISTO_NAME[] = L"csalgo2_static_gnuplot.histo";
const wchar_t DYNAMIC_GNUPLOT_HISTO_NAME[] = L"csalgo2_dynamic_gnuplot.histo";

// CalcRawDataThreshold.cpp:
float CalcRawDataThreshold( const FloatArr & arr1, const FloatArr & arr2 );

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
LearningFinalizer::LearningFinalizer()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
LearningFinalizer::~LearningFinalizer()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
void LearningFinalizer::ProcessStaticData( const FloatArr & staticEmpty,
                                           const FloatArr & staticClothes,
                                           const FloatArr & staticHuman,
                                           const wchar_t  * resultPath )
{
  ALIB_ASSERT( !(staticEmpty.empty()) || !(staticClothes.empty()) || !(staticHuman.empty()) );

  csutility::MultiHistogramEx<double,3,csutility::LinearHistogramAdapter> histo;
  csutility::LinearHistogramAdapter                                       adapter;

  double minValue = +FLT_MAX;
  double maxValue = -FLT_MAX;

  // Estimate histogram's margins.
  {
    for (int k = 0; k < 3; k++)
    {
      const FloatArr * p = 0;

      switch (k)
      {
        case 0: p = &staticEmpty;    break;
        case 1: p = &staticClothes;  break;
        case 2: p = &staticHuman;    break;
      }

      if (!(p->empty()))
      {
        minValue = std::min<double>( minValue, *(std::min_element( p->begin(), p->end() )) );
        maxValue = std::max<double>( maxValue, *(std::max_element( p->begin(), p->end() )) );
      }
    }
  }

  // Initialize multi-histogram.
  ALIB_ASSERT( adapter.Initialize( minValue, maxValue, STATIC_HISTO_SIZE, true ) );
  ALIB_ASSERT( histo.Initialize( adapter ) );

  // Fill up histograms.
  {
    for (int k = 0; k < 3; k++)
    {
      const FloatArr * p = 0;

      switch (k)
      {
        case 0: p = &staticEmpty;    break;
        case 1: p = &staticClothes;  break;
        case 2: p = &staticHuman;    break;
      }

      for (FloatArr::const_iterator it = p->begin(); it != p->end(); ++it)
        ALIB_ASSERT( histo.AddValue( *it, k ) );
    }
    histo.Normilize();
  }

  // Save resultant histogram.
  {
    USES_CONVERSION;
    std::wstring fname;
    if (resultPath != 0) csutility::AddTrailingBackslashToPathName( &(fname = resultPath) );
    fname += STATIC_HISTO_NAME;

    std::fstream file( W2CA(fname.c_str()), std::ios::trunc | std::ios::out );
    ALIB_ASSERT( histo.Save( file ) );
  }

  // Save resultant histogram in GNUPLOT format.
  {
    const char * labels[3] = {"Empty", "Clothes", "Human"};

    USES_CONVERSION;
    std::wstring fname;
    if (resultPath != 0) csutility::AddTrailingBackslashToPathName( &(fname = resultPath) );
    fname += STATIC_GNUPLOT_HISTO_NAME;

    ALIB_ASSERT( histo.SaveInGNUPlotFormat( W2CA(fname.c_str()), labels, 0 ) );
  }

  // Calculate and save best threshold.
  {
    float bestThreshold = CalcRawDataThreshold( staticEmpty, staticHuman );

    USES_CONVERSION;
    std::wstring fname;
    if (resultPath != 0) csutility::AddTrailingBackslashToPathName( &(fname = resultPath) );
    fname += STATIC_THRESHOLD_NAME;

    std::fstream file( W2CA(fname.c_str()), std::ios::trunc | std::ios::out );
    file << "empty/human raw threshold = " << bestThreshold << std::endl;
    file << "histogram threshold = " << (adapter.EntryIndex( bestThreshold )) << std::endl;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
void LearningFinalizer::ProcessDynamicData( const FloatArr & dynamicThing,
                                            const FloatArr & dynamicHuman,
                                            const wchar_t  * resultPath )
{
  ALIB_ASSERT( !(dynamicThing.empty()) && !(dynamicHuman.empty()) );

  csutility::MultiHistogramEx<double,2,csutility::LogBimodalHistogramAdapter> histo;
  csutility::LogBimodalHistogramAdapter                                       adapter;

  double mean[2] = {0,0};
  double dev[2] = {0,0};

  // Estimate means and deviations.
  {
    for (int k = 0; k < 2; k++)
    {
      const FloatArr *     p = 0;
      MeanVarAccum<double> accum;

      switch (k)
      {
        case 0: p = &dynamicThing;  break;
        case 1: p = &dynamicHuman;  break;
      }

      for (FloatArr::const_iterator it = p->begin(); it != p->end(); ++it)
        accum += (*it);

      accum.statistics( &(mean[k]), 0, &(dev[k]) );
      dev[k] = std::max<double>( dev[k], (FLT_MIN/FLT_EPSILON) );
    }
  }

  // Initialize multi-histogram.
  ALIB_ASSERT( adapter.Initialize( mean[0], dev[0], mean[1], dev[1], DYNAMIC_HISTO_SIZE, true ) );
  ALIB_ASSERT( histo.Initialize( adapter ) );

  // Fill up histograms.
  {
    for (int k = 0; k < 2; k++)
    {
      const FloatArr * p = 0;

      switch (k)
      {
        case 0: p = &dynamicThing;  break;
        case 1: p = &dynamicHuman;  break;
      }

      for (FloatArr::const_iterator it = p->begin(); it != p->end(); ++it)
        ALIB_ASSERT( histo.AddValue( *it, k ) );
    }
    histo.Normilize();
  }

  // Save resultant histogram.
  {
    USES_CONVERSION;
    std::wstring fname;
    if (resultPath != 0) csutility::AddTrailingBackslashToPathName( &(fname = resultPath) );
    fname += DYNAMIC_HISTO_NAME;

    std::fstream file( W2CA(fname.c_str()), std::ios::trunc | std::ios::out );
    ALIB_ASSERT( histo.Save( file ) );
  }

  // Save resultant histogram in GNUPLOT format along with exponential approximation.
  {
    const char *      labels[2] = {"Thing", "Human"};
    std::stringstream func;

    for (int k = 0; k < 2; k++)
    {
      if (k > 0) func << ", ";
      func << "(exp(x/" << adapter.GetA()
        << ")*exp(-(exp(x/" << adapter.GetA() << ")-" << (adapter.GetB()+mean[k])
        << ")**2/" << (2.0*dev[k]*dev[k]) << "))/" << (adapter.GetA()*dev[k]*sqrt( 2.0*ALIB_PI ))
        << " title \"" << labels[k] << "\"";
    }

    USES_CONVERSION;
    std::wstring fname;
    if (resultPath != 0) csutility::AddTrailingBackslashToPathName( &(fname = resultPath) );
    fname += DYNAMIC_GNUPLOT_HISTO_NAME;

    ALIB_ASSERT( histo.SaveInGNUPlotFormat( W2CA(fname.c_str()), labels, func.str().c_str() ) );
  }

  // Calculate and save best threshold.
  {
    float bestThreshold = CalcRawDataThreshold( dynamicThing, dynamicHuman );

    USES_CONVERSION;
    std::wstring fname;
    if (resultPath != 0) csutility::AddTrailingBackslashToPathName( &(fname = resultPath) );
    fname += DYNAMIC_THRESHOLD_NAME;

    std::fstream file( W2CA(fname.c_str()), std::ios::trunc | std::ios::out );
    file << "thing/human raw threshold = " << bestThreshold << std::endl;
    file << "histogram threshold = " << (adapter.EntryIndex( bestThreshold )) << std::endl;
  }
}


//-------------------------------------------------------------------------------------------------
/** \brief Function . */
//-------------------------------------------------------------------------------------------------
bool LearningFinalizer::Process( const FloatArr            & staticEmpty,
                                 const FloatArr            & staticClothes,
                                 const FloatArr            & staticHuman,
                                 const FloatArr            & dynamicThing,
                                 const FloatArr            & dynamicHuman,
                                 const wchar_t             * resultPath,
                                 csinterface::IDebugOutput * pOutput )
{
  try
  {
    ProcessStaticData( staticEmpty, staticClothes, staticHuman, resultPath );
    ProcessDynamicData( dynamicThing, dynamicHuman, resultPath );
  }
  catch (std::runtime_error & e)
  {
    if (pOutput != 0)
    {
      std::stringstream msg;
      msg << "LearningFinalizer failed" << std::endl << e.what();
      pOutput->PrintA( IDebugOutput::mt_error, msg.str().c_str() );
    }
  }
  return true;
}

