// CSExchangeTest.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "CSExchangeTest_i.h"


// CCSExchangeTestApp:
// See CSExchangeTest.cpp for the implementation of this class
//

class CCSExchangeTestApp : public CWinApp
{
public:
	CCSExchangeTestApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	BOOL ExitInstance(void);
};

extern CCSExchangeTestApp theApp;