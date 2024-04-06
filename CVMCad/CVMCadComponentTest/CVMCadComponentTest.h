// CVMCadComponentTest.h : main header file for the CVMCadComponentTest application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CCVMCadComponentTestApp:
// See CVMCadComponentTest.cpp for the implementation of this class
//

class CCVMCadComponentTestApp : public CWinApp
{
public:
	CCVMCadComponentTestApp();
	~CCVMCadComponentTestApp();



// Overrides
public:
	virtual BOOL InitInstance();
	ULONG_PTR gdiplusToken;


// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CCVMCadComponentTestApp theApp;