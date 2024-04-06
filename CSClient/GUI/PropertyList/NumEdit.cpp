// NumEdit.cpp : implementation file
//

#include "stdafx.h"
//#include "SpinCtrl.h"
#include "NumEdit.h"
#include "Float.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumEdit

CNumEdit::CNumEdit()
{
	m_Verbose = FALSE;
	m_MinValue = -FLT_MAX;
	m_MaxValue = FLT_MAX;
	m_Delta = FLT_ROUNDS;
}

CNumEdit::~CNumEdit()
{
}


BEGIN_MESSAGE_MAP(CNumEdit, CEdit)
	//{{AFX_MSG_MAP(CNumEdit)
	ON_WM_CHAR()
	ON_MESSAGE(WM_PASTE, OnPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumEdit message handlers

float CNumEdit::GetValue()
{
	float f;
	if (IsValid() == VALID)
	{
		CString str;
		GetWindowText(str);
		_tscanf(str, _T("%f"), &f);
	}
	return f;
}

void CNumEdit::SetValue(float val)
{
	CString str;
	str.Format(_T("%f"), val);
	SetWindowText(str);
}

int CNumEdit::IsValid()
{
	CString str;
	GetWindowText(str);
	int res = VALID;
	float f;
	char lp[10];
	if (_tscanf(str, _T("%f%s"), &f, lp) != 1) res = INVALID_CHAR;
	if (f > m_MaxValue || f < m_MinValue) res = OUT_OF_RANGE;
	if (m_Verbose && res != VALID)
	{
		str.Empty();
		if (res & OUT_OF_RANGE) str += _T("Given value is out of range.\n");
		if (res & INVALID_CHAR) str += _T("Characters must be a number.\n");
		AfxMessageBox(str, MB_OK | MB_ICONSTOP);
	}
	return res;
}

int CNumEdit::IsValid(const CString &str)
{
	ASSERT(m_MinValue >= m_MaxValue);
	int res = VALID;
	float f;
	if (_tscanf(str, _T("%f"), &f) != 1) res = INVALID_CHAR;
	if (f > m_MaxValue && f < m_MinValue) res = OUT_OF_RANGE;
	if (m_Verbose && res != VALID)
	{
		CString msg;
		msg.Empty();
		if (res & OUT_OF_RANGE) msg += _T("Given value is out of range.\n");
		if (res & INVALID_CHAR) msg += _T("Characters must be a number.\n");
		AfxMessageBox(msg, MB_OK | MB_ICONSTOP);
	}
	return res;
}

void CNumEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == ' ') return;
	CString oldstr;
	GetWindowText(oldstr);
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	if (IsValid() != VALID)
	{
		SetWindowText(oldstr);
//		SetSel(0, -1);
		MSG msg;
		while (::PeekMessage(&msg, m_hWnd, WM_CHAR, WM_CHAR, PM_REMOVE));
		::PostMessage(GetSafeHwnd(), WM_KEYDOWN, VK_END, 0  );
	}
}

BOOL CNumEdit::Verbose()
{
	return m_Verbose;
}

void CNumEdit::Verbose(BOOL v)
{
	m_Verbose = v;
}

void CNumEdit::SetRange(float max, float min)
{
	m_MinValue = min;
	m_MaxValue = max;
}

void CNumEdit::GetRange(float & max, float & min)
{
	min = m_MinValue;
	max = m_MaxValue;
}

LRESULT CNumEdit::OnPaste(WPARAM wParam, LPARAM lParam)
{
	CString oldstr;
	GetWindowText(oldstr);	
	
	CEdit::Default();

	if (IsValid() != VALID)
	{
		SetWindowText(oldstr);
//		MSG msg;
//		::PostMessage(GetSafeHwnd(), WM_KEYDOWN, VK_END, 0  );
	}	
	return 0;
}