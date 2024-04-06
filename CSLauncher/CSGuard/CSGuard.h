// CSGuard.h : PROJECT_NAME 應用程式的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在對 PCH 包含此檔案前先包含 'stdafx.h'
#endif

#include "resource.h"		// 主要符號
#include "CSGuardDlg.h"

// CCSGuardApp:
// 請參閱實作此類別的 CSGuard.cpp
//

class CCSGuardApp : public CWinApp
{
public:
	CCSGuardApp();

// 覆寫
	public:
	virtual BOOL InitInstance();
	std::auto_ptr<CCSGuardDlg> m_pdlg;

// 程式碼實作

	DECLARE_MESSAGE_MAP()
};

extern CCSGuardApp theApp;
