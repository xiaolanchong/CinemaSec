// CSGuard.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error �b�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'
#endif

#include "resource.h"		// �D�n�Ÿ�
#include "CSGuardDlg.h"

// CCSGuardApp:
// �аѾ\��@�����O�� CSGuard.cpp
//

class CCSGuardApp : public CWinApp
{
public:
	CCSGuardApp();

// �мg
	public:
	virtual BOOL InitInstance();
	std::auto_ptr<CCSGuardDlg> m_pdlg;

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CCSGuardApp theApp;
