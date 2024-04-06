// CSPlayListDlg.h : header file
//
//UINT CDataPrepareDialog::CalcThreadFunc(void* parameter)
#pragma once
#include "afxcmn.h"


// CCSPlayListDlg dialog
class CCSPlayListDlg : public CDialog
{
// Construction
public:
	CCSPlayListDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CSPLAYLIST_DIALOG };

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


public:
	// main calculation thread
	afx_msg void OnTimer(UINT nIDEvent);


public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButton3();
};
