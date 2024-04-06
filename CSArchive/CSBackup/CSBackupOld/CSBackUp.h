/****************************************************************************
CSBackUp.h
---------------------
begin     : Nov 2004
author(s) : S.Murga
email     : murgas@newmail.ru
****************************************************************************/
#pragma once

#ifdef CSBACKUP_EXPORTS
#define CSBACKUP_API __declspec(dllexport)
#else
#define CSBACKUP_API __declspec(dllimport)
#endif

#include "backup_interface.h"
#include "../../CSUtility/utility/debug_interface.h"

#ifdef _cplusplus
extern "C" {
#endif

HRESULT __cdecl CreateCSBackUp(ICSBackUp** ppBackUp, IDebugOutput* pDebugOut);

#ifdef _cplusplus
}
#endif