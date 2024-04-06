// HtmlDialog.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __HTMLDIALOG_H__
#define __HTMLDIALOG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CHtmlDialog class

class CHtmlDialog
{
// Construction
public:
	CHtmlDialog();
	CHtmlDialog(LPCTSTR lpszURL, BOOL bRes = FALSE, CWnd* pParent = NULL);
	CHtmlDialog(UINT nResID, CWnd* pParent = NULL);

// Attributes
public:
	void SetSize(int x, int y);

protected:
	HWND				m_hWndParent;
	CString			m_strOptions;
	COleVariant	m_varArgs, m_varReturn;
	CString			m_strURL;
	HINSTANCE		m_hInstMSHTML;

// Operations
public:
	void 				SetParam(LPCTSTR lpszArgs);
	void 				SetParam(VARIANT* pvarArgs);
	void 				SetDlgOptions(LPCTSTR lpszOptions);
	LPCVARIANT 	GetReturnVariant();
	CString 		GetReturnString();

	virtual int DoModal();

// Implementation
public:
	virtual ~CHtmlDialog();

private:
	inline void ResourceToURL(LPCTSTR lpszURL);
	void CommonConstruct();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};

/////////////////////////////////////////////////////////////////////////////

#endif // __HTMLDIALOG_H__

