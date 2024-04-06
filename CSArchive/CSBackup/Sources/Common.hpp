/*
*	CSBackup 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Common.hpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-07-29
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#pragma once
#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#ifndef STRICT
#define STRICT
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER			
#define WINVER 0x0500
#endif

#ifndef _WIN32_WINNT		
#define _WIN32_WINNT 0x0500
#endif						

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlhost.h>
#include <atldbcli.h>
#include <atltime.h>
#include <atlcomtime.h>

// Windows Header Files:
#include <windows.h>
#include <shlwapi.h>



// Boost support
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>
#include <boost/bind.hpp>
#include <boost/regex.hpp>

//Just shutup the cl.exe
#pragma warning(disable : 4267 4244)
#include <boost/format.hpp>
#pragma warning(default : 4267 4244)

//STL support
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>


//Shared headers
#include "Shared\Debug_Interface.h"



#endif //__COMMON_HPP__
