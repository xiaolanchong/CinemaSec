// CSGuard.cpp : �w�q���ε{�������O�欰�C
//

#include "stdafx.h"
#include "CSGuard.h"
#include "CSGuardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCSGuardApp

BEGIN_MESSAGE_MAP(CCSGuardApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCSGuardApp �غc

CCSGuardApp::CCSGuardApp()
{
	// TODO: �b���[�J�غc�{���X�A
	// �N�Ҧ����n����l�]�w�[�J InitInstance ��
}


// �Ȧ����@�� CCSGuardApp ����

CCSGuardApp theApp;


// CCSGuardApp ��l�]�w

BOOL CCSGuardApp::InitInstance()
{
	// ���p���ε{����T�M����w�ϥ� ComCtl32.dll 6.0 (�t) �H�᪩��
	// �H�ҥε�ı�Ƽ˦��A�h Windows XP �ݭn InitCommonControls()�C�_�h���ܡA
	// ����������إ߱N���ѡC
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

#if 0
	CCSGuardDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �b����m��ϥ� [�T�w] �Ӱ���ϥι�ܤ����
		// �B�z���{���X
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �b����m��ϥ� [����] �Ӱ���ϥι�ܤ����
		// �B�z���{���X
	}
	return FALSE;
#else
	m_pdlg = std::auto_ptr<CCSGuardDlg>( new CCSGuardDlg() );
	m_pdlg->Create( CCSGuardDlg::IDD );
	m_pMainWnd = m_pdlg.get();
	//m_pMainWnd->ShowWindow( SW_SHOW );
	return TRUE;
/*	CCSGuardDlg* pdlg = new CCSGuardDlg() ;
	pdlg->Create( CCSGuardDlg::IDD );
	m_pMainWnd = pdlg;*/
	return TRUE;
#endif
	// �]���w�g������ܤ���A�Ǧ^ FALSE�A�ҥH�ڭ̷|�������ε{���A
	// �ӫD���ܶ}�l���ε{�����T���C
	
}
