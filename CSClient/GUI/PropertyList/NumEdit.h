#if !defined(AFX_NUMEDIT_H__7482F7FF_A479_11D2_A6A7_00600844997A__INCLUDED_)
#define AFX_NUMEDIT_H__7482F7FF_A479_11D2_A6A7_00600844997A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NumEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNumEdit window

class CNumEdit : public CEdit
{
// Construction
public:
	CNumEdit();

// Attributes
public:
	enum {VALID = 0x00, OUT_OF_RANGE = 0x01, INVALID_CHAR = 0x02};
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNumEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void GetRange(float &max, float &min);
	virtual void SetRange(float max, float min);
	virtual void Verbose(BOOL v);
	virtual BOOL Verbose();
	virtual int IsValid();
	virtual int IsValid(const CString &str);

	virtual void SetValue(float val);
	virtual float GetValue();
	virtual ~CNumEdit();

	// Generated message map functions
protected:
	BOOL m_Verbose;
	float m_Delta, m_MinValue, m_MaxValue;
	//{{AFX_MSG(CNumEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	afx_msg LRESULT OnPaste(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NUMEDIT_H__7482F7FF_A479_11D2_A6A7_00600844997A__INCLUDED_)
