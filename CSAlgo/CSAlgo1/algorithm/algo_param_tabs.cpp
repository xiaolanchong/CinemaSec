/****************************************************************************
  algo_param_tabs.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#include "stdafx.h"
#include "algo_param.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csalgo
{

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
AlgorithmParams::AlgorithmParams( int & controlIdentifier )
{
  p_NCC.init(
    _T("NCC"),
    _T("normalized cross-correlation measure (checked), otherwise mean-normalized one (unchecked)"),
    true, this, &controlIdentifier );

  p_normL2.init(
    _T("normL2"),
    _T("use L2 norm (checked) in correlation measure, otherwise L1 norm (unchecked)"),
    true, this, &controlIdentifier );

  p_frameChainSize.init(
    _T("frameChainSize"),
    _T("the number of summarized lastest frames that constitutes the current one"),
    5, 1, 10, this, &controlIdentifier );

  p_statCorrSearchRange.init(
    _T("statCorrSearchRange"),
    _T("the half of the search range used to correct chair's position with respect to the static background"),
    2, 0, 10, this, &controlIdentifier );

  p_frameAccumulationDecayTau.init(
    _T("frameAccumulationDecayTau"),
    _T("frame accumulation decay tau, in seconds"),
    0.2f, 0.01f, 1.0f, this, &controlIdentifier );

  p_dynaToAccumDecayRatio.init(
    _T("dynaToAccumDecayRatio"),
    _T("ratio: (dynamic background decay tau) / (frame accumulation decay tau)"),
    3.0f, 1.5f, 100.0f, this, &controlIdentifier );

  p_statDiffThreshold.init(
    _T("statDiffThreshold"),
    _T("threshold on correlation with static background"),
    0.25f, MIN_CORR_DIFF, MAX_CORR_DIFF, this, &controlIdentifier );

  p_dynaDiffThreshold.init(
    _T("dynaDiffThreshold"),
    _T("threshold on correlation with dynamic background"),
    0.05f, MIN_CORR_DIFF, MAX_CORR_DIFF, this, &controlIdentifier );

  p_statDecayTau.init(
    _T("staticDecayTau"),
    _T("time decay parameter of static interval integrator, in seconds"),
    1.0f, 0.1f, 10.0f, this, &controlIdentifier );

  p_dynaDecayTau.init(
    _T("dynamicDecayTau"),
    _T("time decay parameter of dynamic interval integrator, in seconds"),
    1.0f, 0.1f, 10.0f, this, &controlIdentifier );

  p_minStatInterval.init(
    _T("minStaticInterval"),
    _T("minimal time of existance of statically detected object, in seconds"),
    10.0f, 0.1f, 100.0f, this, &controlIdentifier );

  p_minDynaInterval.init(
    _T("minDynamicInterval"),
    _T("minimal time of existance of dynamically detected object, in seconds"),
    3.0f, 0.1f, 100.0f, this, &controlIdentifier );
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
ImageAcquireParams::ImageAcquireParams( int & controlIdentifier )
{
  p_meanUpdateRate.init(
    _T("meanUpdateRate"),
    _T("update rate of mean intensity"),
    0.02f, 0.001f, 1.0f, this, &controlIdentifier );

  p_meanOutlierThr.init(
    _T("meanOutlierThr"),
    _T("threshold on outlier of mean intensity measured as a multiple of noise deviation"),
    2.0f, 0.1f, 100.0f, this, &controlIdentifier );

  p_maxNoiseSignalRatio.init(
    _T("maxNoiseSignalRation"),
    _T("max. noise to signal ratio"),
    0.2f, 0.001f, 0.5f, this, &controlIdentifier );

  p_maxMeanDiffThr.init(
    _T("maxMeanDiffThr"),
    _T("threshold on difference of mean intensities of both half-frames as a multiple of noise deviation"),
    2.0f, 0.1f, 100.0f, this, &controlIdentifier );

  p_nSmoothRepeat.init(
    _T("nSmoothRepeat"),
    _T("the number of times acquired image must be repeatedly smoothed, zero means no smoothing"),
    1, 0, 10, this, &controlIdentifier );
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
StatisticsParams::StatisticsParams( int & controlIdentifier )
{
  p_startDelay.init(
    _T("startDelay"),
    _T("delay of statistics accumulation after start-up of surveillance process in seconds"),
    3.0f, 0.0f, 3600.0f, this, &controlIdentifier );

  p_dynaToStatDurationRatio.init(
    _T("dynaToStatDurationRatio"),
    _T("max. ratio of activity period to occupation period"),
    0.7f, 0.0f, 1.0f, this, &controlIdentifier );

  p_minDynaIntervalNumber.init(
    _T("minDynaIntervalNumber"),
    _T("min. number of dynamic interval within a static one to qualify a chair as human occupied"),
    4, 1, 100, this, &controlIdentifier );

/*
  p_maxDynaIntervalNumber.init(
    _T("maxDynaIntervalNumber"),
    _T("if this number of dynamic intervals was exceeded, then the object is definitely a human"),
    3, 1, 100, this, &controlIdentifier );
*/

  p_statisticsAnalysisPeriod.init(
    _T("statisticsAnalysisPeriod"),
    _T("statistics analysis periodicity in seconds"),
    600.0f, 1.0f, 3600.0f, this, &controlIdentifier );
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
DemoParams::DemoParams( int & controlIdentifier )
{
  p_demoMode.init(
    _T("demoMode"),
    _T("demo mode enabled if checked, otherwise disabled"),
    false, this, &controlIdentifier );

  p_bLogCamAnalyzer.init(
    _T("bLogCamAnalyzer"),
    _T("print intermediate results of camera analyzer(s) in the log file(s)"),
    false, this, &controlIdentifier );

  p_bLogHallAnalyzer.init(
    _T("bLogHallAnalyzer"),
    _T("print intermediate results of hall analyzer in the log file"),
    false, this, &controlIdentifier );

  p_bCamAnalyzerShowChairState.init(
    _T("bCamAnalyzerShowChairState"),
    _T("show chair state while preparing demo image in camera analyzer"),
    false, this, &controlIdentifier );

  p_showValue.init(
    _T("showValue"),
    _T("chair value: N - no, S - static score, D - dynamic score, I - identifier, H - humanity"),
    _T('N'), _T("NSDIH"), this, &controlIdentifier );
}

} // namespace csalgo

