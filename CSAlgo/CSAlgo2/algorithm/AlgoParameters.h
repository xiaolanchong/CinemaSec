/****************************************************************************
  AlgoParameters.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

#include "AlgoParamTabs.h"
#include "ImageAcquirer.h"

namespace csalgo2
{

extern const int FIRST_CONTROL_IDENTIFIER;

//=================================================================================================
/** \class Parameters.
    \brief Parameters. */
//=================================================================================================
struct Parameters : public paramlib::BaseParamSection
{
  csalgo2::AlgorithmParams    s_algorithm;      //!< algorithm section
  csalgo2::PeriodParams       s_period;         //!< period setion
  csalgo2::ImageAcquireParams s_acquirer;       //!< image acquirer section
  csalgo2::DemoParams         s_demo;           //!< demo section

  Parameters( int control_identifier );
};

} // namespace csalgo2

