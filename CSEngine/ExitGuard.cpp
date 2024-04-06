//======================================================================================//
//                                      CinemaSec                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Функция для автоматического завершения серверов
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 25.07.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"

extern "C" void	__stdcall	ExitGuard()
{
	const UINT WM_EXIT_GUARD		= ::RegisterWindowMessage(_T("ExitGuard"));
	DWORD_PTR dwRes;
	LRESULT res = SendMessageTimeout( HWND_BROADCAST, WM_EXIT_GUARD, 0, 0, SMTO_NORMAL, 30 * 1000, &dwRes );
}