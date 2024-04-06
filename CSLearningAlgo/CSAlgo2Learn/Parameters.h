/****************************************************************************
  Parameters.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once
#include "Param_Tabs.h"
extern const int FIRST_CONTROL_IDENTIFIER;

//=================================================================================================
/** \class Parameters.
    \brief Parameters. */
//=================================================================================================
struct Parameters : public paramlib::BaseParamSection
{
  csalgo2::ImageAcquireParams s_acquirer;       //!< image acquirer section
  SequenceParams s_sequence;

  Parameters( int control_identifier );
};

