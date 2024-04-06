// CSSeqRefiner.h : main header file for the CSSeqRefiner application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CCSSeqRefinerApp:
// See CSSeqRefiner.cpp for the implementation of this class
//

class CCSSeqRefinerApp : public CWinApp
{
public:
	CCSSeqRefinerApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnHelp( );

	DECLARE_MESSAGE_MAP()
};

extern CCSSeqRefinerApp theApp;