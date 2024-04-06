#if !defined(AFX_OFFSETCHAIRDIALOG_H__601F403C_6A82_4DD6_BA57_73A506085362__INCLUDED_)
#define AFX_OFFSETCHAIRDIALOG_H__601F403C_6A82_4DD6_BA57_73A506085362__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OffsetChairDialog.h : header file
//
#include "../UT/OXEdit.h"
/////////////////////////////////////////////////////////////////////////////
// COffsetChairDialog dialog

struct IOffsetCallBack
{
	virtual void Move(int x, int y) = 0;

	virtual ~IOffsetCallBack(){}
};

class COffsetChairDialog : public CDialog
{
// Construction
	IOffsetCallBack* m_pCallback;
public:
	COffsetChairDialog(CWnd* pParent = NULL);   // standard constructor

	void SetCallback(IOffsetCallBack* pCallback) { m_pCallback = pCallback;}
// Dialog Data
	//{{AFX_DATA(COffsetChairDialog)
	enum { IDD = IDD_OFFSET_CHAIR };
	COXNumericEdit	m_edDown;
	COXNumericEdit	m_edLeft;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COffsetChairDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COffsetChairDialog)
	afx_msg void OnMove();
	afx_msg BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OFFSETCHAIRDIALOG_H__601F403C_6A82_4DD6_BA57_73A506085362__INCLUDED_)
