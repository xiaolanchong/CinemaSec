// CSGuardDlg.h : 
//
#include "../../CSClient/UT/OXTaskbarIcon.h"
#include <afxmt.h>

#pragma once



class CCSGuardDlg : public CDialog
{

public:
	CCSGuardDlg(CWnd* pParent = NULL);	


	enum { IDD = IDD_CSGUARD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	



protected:
	HICON			m_hIcon;
	std::auto_ptr<COXTaskbarIcon>	m_pwndTaskbar;
//	CEvent			m_SyncEvent;
	HANDLE			m_hThread;

	void	HandleMenu();
	void	InitTaskbar();
	void	OnExit();
	
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg LRESULT	OnTaskbarMsg( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnTaskBarCreated(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnExitGuard(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
};
