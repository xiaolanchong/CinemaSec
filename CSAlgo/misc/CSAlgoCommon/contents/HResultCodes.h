/****************************************************************************
  HResultCodes.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csalgocommon
{

const HRESULT CS_ALGO_FACILITY_CODE = 130;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Warnings.
///////////////////////////////////////////////////////////////////////////////////////////////////

// Returned if the current frame should be dropped as unreliable.
const HRESULT HRESULT_DROP_FRAME = MAKE_HRESULT(0,CS_ALGO_FACILITY_CODE,1);

// Returned if the current frame should be skipped for any reason unrelated to the frame itself.
const HRESULT HRESULT_SKIP_FRAME = MAKE_HRESULT(0,CS_ALGO_FACILITY_CODE,2);

///////////////////////////////////////////////////////////////////////////////////////////////////
// Errors.
///////////////////////////////////////////////////////////////////////////////////////////////////

// Returned if data of unknown type have been encountered.
const HRESULT HRESULT_UNKNOWN_DATA = MAKE_HRESULT(1,CS_ALGO_FACILITY_CODE,1);

// Returned if pointer to a DLL function is NULL (function not found).
const HRESULT HRESULT_NULL_PROC_ADDR = MAKE_HRESULT(1,CS_ALGO_FACILITY_CODE,2);

// Returned if NULL pointer has been passed as a function parameter.
const HRESULT HRESULT_NULL_PARAM_PTR = MAKE_HRESULT(1,CS_ALGO_FACILITY_CODE,3);

// Returned if some serious error has occurred while loading/freeing a DLL.
const HRESULT HRESULT_DLL_CRITICAL_ERR = MAKE_HRESULT(1,CS_ALGO_FACILITY_CODE,4);

extern "C"
{
  HRESULT PrintCode( HRESULT code, IDebugOutput * pOutput, const char * file, int line );
}

#define CS_ALGO_PRINT_CODE( code, pOut ) csalgocommon::PrintCode( code, pOut, __FILE__, __LINE__ )


/*
const HRESULT // unsupported exception
CS_ALGO_ERROR_UNSUPPORTED = MAKE_HRESULT(1,CS_ALGO_FACILITY_CODE,1);
const HRESULT // invalid input parameter structure
CS_ALGO_ERROR_INVALID_PARAMS = MAKE_HRESULT(1,CS_ALGO_FACILITY_CODE,2);
const HRESULT
CS_ALGO_ERROR_SAVE_PARAMS = MAKE_HRESULT(1,CS_ALGO_FACILITY_CODE,3);
*/

} // namespace csalgocommon

