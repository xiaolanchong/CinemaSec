/****************************************************************************
  app_param.h
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

//=================================================================================================
/** \struct AppParams.
    \brief  AppParams. */
//=================================================================================================
struct AppParams : public paramlib::BaseParamSection
{
  paramlib::IntParam  p_algoVersion;
  paramlib::BlnParam  p_bPerFrame;
  paramlib::BlnParam  p_bShowDemoImage;
  paramlib::BlnParam  p_bUseRawImage;
  paramlib::BlnParam  p_bLogCamAnalyzer;

/*
  paramlib::BlnParam  p_simulateTime;
  paramlib::FltParam  p_simulatedFPS;
*/

  explicit AppParams( int & controlIdentifier );
};


//=================================================================================================
/** \class AppParameters.
    \brief AppParameters. */
//=================================================================================================
struct AppParameters : public paramlib::BaseParamSection
{
  AppParams s_app;

  AppParameters();
};

