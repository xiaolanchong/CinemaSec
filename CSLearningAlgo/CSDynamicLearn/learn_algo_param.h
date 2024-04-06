/****************************************************************************
  learn_algo_param.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "../../CSAlgo/algorithm/algo_param_tabs.h"
#include "learn_algo_param_tabs.h"

extern const int FIRST_CONTROL_IDENTIFIER;

//=================================================================================================
/** \class Parameters.
    \brief Parameters. */
//=================================================================================================
struct Parameters : public paramlib::BaseParamSection
{
  csalgo::AlgorithmParams    s_algorithm;      //!< algorithm section
  csalgo::ImageAcquireParams s_acquirer;       //!< image acquirer section
  csalgo::StatisticsParams   s_statistics;     //!< statistics section
  csalgo::DemoParams         s_demo;           //!< demo section
  LearnAlgoParams            s_learn;          //!< learning section

  explicit Parameters( int control_identifier );
};

