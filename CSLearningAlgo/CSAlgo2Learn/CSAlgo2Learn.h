// CSAlgo2Learn.h : main header file for the CSAlgo2Learn DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CCSAlgo2LearnApp
// See CSAlgo2Learn.cpp for the implementation of this class
//

class CCSAlgo2LearnApp : public CWinApp
{
public:
	CCSAlgo2LearnApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
