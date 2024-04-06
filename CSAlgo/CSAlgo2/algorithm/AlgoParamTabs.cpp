/****************************************************************************
  AlgoParamTabs.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "AlgoParameters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace csalgo2
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
AlgorithmParams::AlgorithmParams( int & controlIdentifier )
{
  p_bDetectDayNight.init(
    L"bDetectDayNight",
    L"enable day/night detection to switch off surveillance during break ",
    true, this, &controlIdentifier );

  p_bCorrelationNormL1.init(
    L"bCorrelationNormL1",
    L"correlation norm: robust L1 (checked), fast L2 (unchecked) ",
    true, this, &controlIdentifier );

  p_correctionRange.init(
    L"correctionRange",
    L"the half of the search range used to correct chair's position with respect to the static background ",
    1, 0, 10, this, &controlIdentifier );

  p_minStatIntensity.init(
    L"minStatIntensity",
    L"threshold on minimal mean intensity of a frame accepted for static correlation, [0..255] ",
    30.0f, 1.0f, 255.0f, this, &controlIdentifier );

  p_statThreshold.init(
    L"statThreshold",
    L"threshold on correlation with static background ",
    0.70f, MIN_CORR_DIFF, MAX_CORR_DIFF, this, &controlIdentifier );

  p_strongStatThreshold.init(
    L"strongStatThreshold",
    L"strong threshold on correlation with static background ",
    1.4f, MIN_CORR_DIFF, MAX_CORR_DIFF, this, &controlIdentifier );

  p_feebleDynaThreshold.init(
    L"feebleDynaThreshold",
    L"feeble threshold on accumulated dynamics of a human ",
    0.030f, 0.0f, 1.0f, this, &controlIdentifier );

  p_normalDynaThreshold.init(
    L"normalDynaThreshold",
    L"normal threshold on accumulated dynamics of a human ",
    0.042f, 0.0f, 1.0f, this, &controlIdentifier );

  p_strongDynaThreshold.init(
    L"strongDynaThreshold",
    L"strong threshold on accumulated dynamics of a human ",
    0.2f, 0.0f, 1.0f, this, &controlIdentifier );

  p_statDecayTau.init(
    L"staticDecayTau",
    L"time decay parameter of static interval integrator, in seconds",
    60.0f, 1.0f, 600.0f, this, &controlIdentifier );

  p_chairHumanityThreshold.init(
    L"chairHumanityThreshold",
    L"threshold on ratio of human detection period to chair observation time (minimal humanity)",
    0.33f, 0.0f, 1.0f, this, &controlIdentifier );

  p_weakBorderThickness.init(
    L"weakBorderThickness",
    L"relative (with respect to chair extent) thickness of weakly weighted layer of border points ",
    0.05f, 0.0f, 0.25f, this, &controlIdentifier );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
PeriodParams::PeriodParams( int & controlIdentifier )
{
  p_startDelayPeriod.init(
    L"startDelayPeriod",
    L"delay of statistics accumulation after start-up of surveillance process, in seconds ",
    30.0f, 0.0f, 3600.0f, this, &controlIdentifier );

  p_minStatInterval.init(
    L"minStaticInterval",
    L"minimal duration of 'statical' interval existance, in seconds ",
    180.0f, 1.0f, 1200.0f, this, &controlIdentifier );

  p_minTotalOccupationPeriod.init(
    L"minTotalOccupationPeriod",
    L"minimal total period of chair occupation by recognizable object, in seconds ",
    600.0f, 1.0f, 1800.0f, this, &controlIdentifier );

  p_minFeebleActivityPeriod.init(
    L"minFeebleActivityPeriod",
    L"minimal period of feeble activity inside a chair that can be classified as human-occupied, in seconds ",
    1200.0f, 1.0f, 1800.0f, this, &controlIdentifier );

  p_minNormalActivityPeriod.init(
    L"minNormalActivityPeriod",
    L"minimal period of normal activity inside a chair that can be classified as human-occupied, in seconds ",
    300.0f, 1.0f, 1800.0f, this, &controlIdentifier );

  p_minDayDuration.init(
    L"minDayDuration",
    L"if \"day\" continues this period (in seconds), then statistics must be resetted",
    60.0f, 1.0f, 1200.0f, this, &controlIdentifier );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
DemoParams::DemoParams( int & controlIdentifier )
{
  p_demoMode.init(
    L"demoMode",
    L"demo mode enabled if checked, otherwise disabled ",
    false, this, &controlIdentifier );

  p_bCamAnalyzerShowChairState.init(
    L"bCamAnalyzerShowChairState",
    L"show chair state while preparing demo image in camera analyzer ",
    false, this, &controlIdentifier );

  p_showValue.init(
    L"showValue",
    L"chair value: N - no, S - static score, D - dynamic score, I - identifier, H - humanity ",
    L'N', L"NSDIH", this, &controlIdentifier );
}

} // namespace csalgo2

