//! \author	Eugene Gorbachve
//!	\date	2004-11-03
//! modified 2005-10-06

#ifndef INTERFACE_H
#define INTERFACE_H

#ifdef _WIN32
#pragma once
#endif

// This function is automatically exported and allows you to access any interfaces exposed with the above macros.
// if pReturnCode is set, it will return one of the following values
// extend this for other error conditions/code
enum 
{
	IFACE_OK = 0,
	IFACE_FAILED
};

//! создание всех интейрфейсов dll

extern "C" DWORD WINAPI CreateInterface(const wchar_t *pName, void **ppInterface);

#endif



