/****************************************************************************
  algo_param.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "algo_param_tabs.h"

namespace csalgo
{

extern const int FIRST_CONTROL_IDENTIFIER;

//=================================================================================================
/** \class Parameters.
    \brief Parameters. */
//=================================================================================================
struct CS_ALGO_API Parameters : public paramlib::BaseParamSection
{
  AlgorithmParams    s_algorithm;      //!< algorithm section
  ImageAcquireParams s_acquirer;       //!< image acquirer section
  StatisticsParams   s_statistics;     //!< statistics section
  DemoParams         s_demo;           //!< demo section

  Parameters( int control_identifier );
};

} // namespace csalgo

