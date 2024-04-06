// CSSeriesLearn.h : main header file for the CSSeriesLearn DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CCSSeriesLearnApp
// See CSSeriesLearn.cpp for the implementation of this class
//

class CCSSeriesLearnApp : public CWinApp
{
public:
	CCSSeriesLearnApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
