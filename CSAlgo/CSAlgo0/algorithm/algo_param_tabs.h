/****************************************************************************
  algo_param_tabs.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

namespace csalgo
{

// The minimal (best) correlation difference obtained as normalized cross-correlation value.
const float MIN_CORR_DIFF = 0.0f;

// The maximal (worst) correlation difference obtained as normalized cross-correlation value.
const float MAX_CORR_DIFF = 2.0f;

//=================================================================================================
/** \struct AlgorithmParams.
    \brief  AlgorithmParams. */
//=================================================================================================
struct CS_ALGO_API AlgorithmParams : public paramlib::BaseParamSection
{
  paramlib::BlnParam  p_NCC;
  paramlib::BlnParam  p_normL2;
  paramlib::IntParam  p_frameChainSize;
  paramlib::IntParam  p_statCorrSearchRange;
  paramlib::FltParam  p_frameAccumulationDecayTau;
  paramlib::FltParam  p_dynaToAccumDecayRatio;
  paramlib::FltParam  p_statDiffThreshold;
  paramlib::FltParam  p_dynaDiffThreshold;
  paramlib::FltParam  p_statDecayTau;
  paramlib::FltParam  p_dynaDecayTau;
  paramlib::FltParam  p_minStatInterval;
  paramlib::FltParam  p_minDynaInterval;

  explicit AlgorithmParams( int & controlIdentifier );
};


//=================================================================================================
/** \struct ImageAcquireParams.
    \brief  ImageAcquireParams. */
//=================================================================================================
struct CS_ALGO_API ImageAcquireParams : public paramlib::BaseParamSection
{
  paramlib::FltParam  p_meanUpdateRate;
  paramlib::FltParam  p_meanOutlierThr;
  paramlib::FltParam  p_maxNoiseSignalRatio;
  paramlib::FltParam  p_maxMeanDiffThr;
  paramlib::IntParam  p_nSmoothRepeat;

  explicit ImageAcquireParams( int & controlIdentifier );
};


//=================================================================================================
/** \struct StatisticsParams.
    \brief  StatisticsParams. */
//=================================================================================================
struct CS_ALGO_API StatisticsParams : public paramlib::BaseParamSection
{
  paramlib::FltParam  p_startDelay;
  paramlib::FltParam  p_dynaToStatDurationRatio;
  paramlib::IntParam  p_minDynaIntervalNumber;
//  paramlib::IntParam  p_maxDynaIntervalNumber;
  paramlib::FltParam  p_statisticsAnalysisPeriod;

  explicit StatisticsParams( int & controlIdentifier );
};


//=================================================================================================
/** \struct DemoParams.
    \brief  DemoParams. */
//=================================================================================================
struct CS_ALGO_API DemoParams : public paramlib::BaseParamSection
{
  paramlib::BlnParam  p_demoMode;
  paramlib::BlnParam  p_bLogCamAnalyzer;
  paramlib::BlnParam  p_bLogHallAnalyzer;
  paramlib::BlnParam  p_bCamAnalyzerShowChairState;
  paramlib::ChrParam  p_showValue;

  explicit DemoParams( int & controlIdentifier );
};

} // namespace csalgo

