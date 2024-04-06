/****************************************************************************
  learn_algo_param_tabs.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \struct LearnAlgoParams.
    \brief  LearnAlgoParams. */
//=================================================================================================
struct LearnAlgoParams : public paramlib::BaseParamSection
{
  paramlib::FltParam  p_lowFastDecay;
  paramlib::FltParam  p_highFastDecay;
  paramlib::IntParam  p_fastDecayNumber;
  paramlib::FltParam  p_lowDecayRatio;
  paramlib::FltParam  p_highDecayRatio;
  paramlib::IntParam  p_decayRatioNumber;
  paramlib::FltParam  p_lowDynaThreshold;
  paramlib::FltParam  p_highDynaThreshold;
  paramlib::IntParam  p_dynaThresholdNumber;
  paramlib::FltParam  p_minPeriodOfConstantType;
  paramlib::FltParam  p_minDynaInterval;
  paramlib::FltParam  p_minCalmInterval;

  explicit LearnAlgoParams( int & control_identifier );
};

