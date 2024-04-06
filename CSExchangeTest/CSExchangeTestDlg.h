// CSExchangeTestDlg.h : header file
//

#pragma once

#include "ResultDesc.h"

#define ParentClass	CPropertyPage

// CCSExchangeTestDlg dialog
class CCSExchangeTestDlg : public ParentClass
{
// Construction
	ResultDesc	m_Desc;
public:
	CCSExchangeTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CSEXCHANGETEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSend();

	afx_msg void OnBnSave();
	afx_msg void OnBnLoad();
	afx_msg void OnBnInputPath();
	afx_msg void OnBnOutputPath();
};
