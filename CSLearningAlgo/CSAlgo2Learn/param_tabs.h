/****************************************************************************
  algo_param_tabs.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \struct SequenceParams.
    \brief  SequenceParams. */
//=================================================================================================
struct SequenceParams : public paramlib::BaseParamSection
{
//  paramlib::BlnParam  p_NCC;
  paramlib::IntParam  p_framesToSkip;
//  paramlib::FltParam  p_frameAccumulationDecayTau;
  explicit SequenceParams( int & controlIdentifier );
};


