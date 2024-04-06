/****************************************************************************
  learn_algo_param_tabs.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#include "stdafx.h"
#include "learn_algo_param_tabs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
LearnAlgoParams::LearnAlgoParams( int & controlIdentifier )
{
  p_lowFastDecay.init(
    _T("lowFastDecay"),
    _T("low value of fast decay rate"),
    0.1f, 0.05f, 100.0f, this, &controlIdentifier );

  p_highFastDecay.init(
    _T("highFastDecay"),
    _T("high value of fast decay rate"),
    2.0f, 0.05f, 100.0f, this, &controlIdentifier );

  p_fastDecayNumber.init(
    _T("fastDecayNumber"),
    _T("the number of fast decay rates to be tried within an interval bounded by low and high limits"),
    10, 2, 100, this, &controlIdentifier );

  p_lowDecayRatio.init(
    _T("lowDecayRatio"),
    _T("low value of ratio = (slow decay tau) / (fast decay tau)"),
    2.0f, 1.5f, 100.0f, this, &controlIdentifier );

  p_highDecayRatio.init(
    _T("highDecayRatio"),
    _T("high value of ratio = (slow decay tau) / (fast decay tau)"),
    10.0f, 1.5f, 100.0f, this, &controlIdentifier );

  p_decayRatioNumber.init(
    _T("decayRatioNumber"),
    _T("the number of ratios to be tried within an interval bounded by low and high limits"),
    10, 2, 100, this, &controlIdentifier );

  p_lowDynaThreshold.init(
    _T("lowDynaThreshold"),
    _T("the lowest dynamic threshold"),
    0.01f, 0.01f, 1.0f, this, &controlIdentifier );

  p_highDynaThreshold.init(
    _T("highDynaThreshold"),
    _T("the highest dynamic threshold"),
    0.5f, 0.01f, 1.0f, this, &controlIdentifier );

  p_dynaThresholdNumber.init(
    _T("dynaThresholdNumber"),
    _T("the number of thresholds of dynamic correlation to be tried"),
    40, 10, 1000, this, &controlIdentifier );

  p_minPeriodOfConstantType.init(
    _T("minPeriodOfConstantType"),
    _T("the minimal period while object's type remains unaltered in seconds"),
    2400.0f, 800.0f, 7200.0f, this, &controlIdentifier );

  p_minDynaInterval.init(
    _T("minDynaInterval"),
    _T("the minimal duration of dynamic interval in seconds"),
    1.0f, 0.1f, 10.0f, this, &controlIdentifier );

  p_minCalmInterval.init(
    _T("minCalmInterval"),
    _T("the minimal duration of calm interval in seconds"),
    10.0f, 1.0f, 1000.0f, this, &controlIdentifier );
}

