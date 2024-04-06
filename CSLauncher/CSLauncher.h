// CSLauncher.h : main header file for the CSLauncher application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "res\resource.h"       // main symbols
#include "CSLauncher_i.h"


// CLauncherApp:
// See CSLauncher.cpp for the implementation of this class
//

class CLauncherApp : public CWinApp
{
public:
	CLauncherApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	BOOL ExitInstance(void);
};

extern CLauncherApp theApp;