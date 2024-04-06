/****************************************************************************
  algo_param_tabs.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#include "stdafx.h"
#include "parameters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
SequenceParams::SequenceParams( int & controlIdentifier )
{
 /* p_NCC.init(
    _T("NCC"),
    _T("normalized cross-correlation measure (checked), otherwise mean-normalized one (unchecked)"),
    true, this, &controlIdentifier );
*/

  p_framesToSkip.init(
    _T("framesToSkip"),
    _T("frames to skip"),
    5, 0, 1000, this, &controlIdentifier );
/*
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
*/
}

