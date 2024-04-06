//======================= Copyright © 2004, Eugene V. Gorbachev =================
//
// Purpose: 
//
//=============================================================================

#ifndef MEMLEAK_H
#define MEMLEAK_H
#ifdef _WIN32
#pragma once
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif /* _DEBUG */

#endif // MEMLEAK_H
