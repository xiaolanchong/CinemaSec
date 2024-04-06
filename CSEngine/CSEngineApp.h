// CSEngine.h : main header file for the CSEngine DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "res/en/resource.h"		// main symbols


// CCSEngineApp
// See CSEngine.cpp for the implementation of this class
//

class CCSEngineApp : public CWinApp
{
public:
	CCSEngineApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
