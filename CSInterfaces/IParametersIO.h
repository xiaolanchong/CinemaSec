/****************************************************************************
  IParametersIO.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "common_interface_settings.h"

namespace csinterface
{

//=================================================================================================
/** \struct IParametesIO.
    \brief  IParametesIO. */
//=================================================================================================
struct IParametesIO
{

bool ShowDialog( IN HWND hParent ) = 0;

bool SetDefault( IN IDebugOutput * pDebugOut ) = 0;

bool Load( IN const std::wstring * fileName ) = 0;

bool Load( IN const std::vector<__int8> * pBinImage ) = 0;

bool Save( IN const std::wstring * fileName ) = 0;

bool Save( IN std::vector<__int8> * pBinImage ) = 0;

};

} // namespace csinterface

