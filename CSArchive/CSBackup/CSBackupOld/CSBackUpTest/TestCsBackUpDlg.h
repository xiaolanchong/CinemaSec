// TestCsBackUpDlg.h : header file
//

#pragma once

#include "../../../CSUtility/utility/debug_interface.h"

// CTestCsBackUpDlg dialog
class CTestCsBackUpDlg : public CDialog
{
// Construction
public:
	CTestCsBackUpDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTCSBACKUP_DIALOG };

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
  afx_msg void OnBnClickedOk();
};

class CMyDebug : public IDebugOutput
{
  long l;
public:
  virtual void PrintW( __int32 messageType, LPCWSTR szMessage);
  virtual void PrintA( __int32 messageType, LPCSTR szMessage);
};