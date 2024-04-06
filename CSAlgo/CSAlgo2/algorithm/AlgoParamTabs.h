/****************************************************************************
  AlgoParamTabs.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csalgo2
{

// The minimal (best) correlation difference obtained from normalized cross-correlation value.
const float MIN_CORR_DIFF = 0.0f;

// The maximal (worst) correlation difference obtained from normalized cross-correlation value.
const float MAX_CORR_DIFF = 2.0f;

//=================================================================================================
/** \struct AlgorithmParams.
    \brief  AlgorithmParams. */
//=================================================================================================
struct AlgorithmParams : public paramlib::BaseParamSection
{
  paramlib::BlnParam  p_bDetectDayNight;
  paramlib::BlnParam  p_bCorrelationNormL1;
  paramlib::IntParam  p_correctionRange;
  paramlib::FltParam  p_minStatIntensity;
  paramlib::FltParam  p_statThreshold;
  paramlib::FltParam  p_strongStatThreshold;
  paramlib::FltParam  p_feebleDynaThreshold;
  paramlib::FltParam  p_normalDynaThreshold;
  paramlib::FltParam  p_strongDynaThreshold;
  paramlib::FltParam  p_statDecayTau;
  paramlib::FltParam  p_chairHumanityThreshold;
  paramlib::FltParam  p_weakBorderThickness;

  explicit AlgorithmParams( int & controlIdentifier );
};


//=================================================================================================
/** \struct PeriodParams.
    \brief  PeriodParams. */
//=================================================================================================
struct PeriodParams : public paramlib::BaseParamSection
{
  paramlib::FltParam  p_startDelayPeriod;
  paramlib::FltParam  p_minStatInterval;
  paramlib::FltParam  p_minTotalOccupationPeriod;
  paramlib::FltParam  p_minFeebleActivityPeriod;
  paramlib::FltParam  p_minNormalActivityPeriod;
  paramlib::FltParam  p_minDayDuration;

  explicit PeriodParams( int & controlIdentifier );
};


//=================================================================================================
/** \struct DemoParams.
    \brief  DemoParams. */
//=================================================================================================
struct DemoParams : public paramlib::BaseParamSection
{
  paramlib::BlnParam  p_demoMode;
  paramlib::BlnParam  p_bCamAnalyzerShowChairState;
  paramlib::ChrParam  p_showValue;

  explicit DemoParams( int & controlIdentifier );
};

} // namespace csalgo2

