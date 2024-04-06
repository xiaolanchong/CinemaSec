/****************************************************************************
  csalgo_hresult_codes.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

namespace csalgo
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

// Returned if data of unknown type have been tried to upload/unload.
const HRESULT HRESULT_UNKNOWN_DATA = MAKE_HRESULT(1,CS_ALGO_FACILITY_CODE,1);

/*
const HRESULT // unsupported exception
CS_ALGO_ERROR_UNSUPPORTED = MAKE_HRESULT(1,CS_ALGO_FACILITY_CODE,1);
const HRESULT // invalid input parameter structure
CS_ALGO_ERROR_INVALID_PARAMS = MAKE_HRESULT(1,CS_ALGO_FACILITY_CODE,2);
const HRESULT
CS_ALGO_ERROR_SAVE_PARAMS = MAKE_HRESULT(1,CS_ALGO_FACILITY_CODE,3);
*/

} // namespace csalgo

