/****************************************************************************
  app_param.cpp
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "app_param.h"

#define APSTUDIO_INVOKED
#undef  APSTUDIO_READONLY_SYMBOLS
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static int control_identifier = _APS_NEXT_CONTROL_VALUE;

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
AppParameters::AppParameters() : s_app( control_identifier )
{
  s_app.init( L"Application", L"application parameters", this );
}


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
AppParams::AppParams( int & controlIdentifier )
{
  p_algoVersion.init(
    L"algoVersion",
    L"version of algorithm",
    1, 1, 2, this, &controlIdentifier );

  p_bPerFrame.init(
    L"bPerFrame",
    L"get frame by frame from videofile (checked), otherwise get the latest available frame (unchecked)",
    false, this, &controlIdentifier );

  p_bShowDemoImage.init(
    L"bShowDemoImage",
    L"show demo image while running surveillance process",
    true, this, &controlIdentifier );

  p_bUseRawImage.init(
    L"bUseRawImage",
    L"demonstrate raw, noisy image (checked, fast), otherwise preprocessed one (unchecked, slow)",
    true, this, &controlIdentifier );

  p_bLogCamAnalyzer.init(
    L"bLogCamAnalyzer",
    L"open a log-file per camera analyzer",
    false, this, &controlIdentifier );

/*
  p_simulateTime.init(
    L"simulateTime",
    L"simulate time pace while running video (checked), otherwise use default value (unchecked)",
    false, this, &controlIdentifier );

  p_simulatedFPS.init(
    L"simulatedFPS",
    L"simulated frames-per-second rate (reasonable, if time simulation is selected)",
    15.0f, 1.0f, 50.0f, this, &controlIdentifier );
*/
}

