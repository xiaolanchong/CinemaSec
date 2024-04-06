// CSChair.h : main header file for the CSChair DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

//#include "resource.h"		// main symbols


// CCSChairApp
// See CSChair.cpp for the implementation of this class
//

class CCSChairApp : public CWinApp
{
public:
	CCSChairApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
