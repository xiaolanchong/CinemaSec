// CSEngine.h : main header file for the CSEngine DLL
//

#ifndef _CS_ENGINE_H_
#define _CS_ENGINE_H_

namespace csengine
{
//! код возврата CreateEngineInterface
enum 
{
	IFACE_OK = 0,	//! успех
	IFACE_FAILED	//! ошибка, интерфейс не создан
};
}

//! This function is automatically exported and allows you to access any interfaces exposed with the above macros.
//! if pReturnCode is set, it will return one of the following values
//! extend this for other error conditions/code
//! 
//! \param pName UID интерейса или его уникальное имя
//! \param ppInterface для передачи интерфейса
//! \return IFACE_OK или IFACE_FAILED
extern "C" DWORD WINAPI CreateEngineInterface(const wchar_t *pName, void **ppInterface);

#endif