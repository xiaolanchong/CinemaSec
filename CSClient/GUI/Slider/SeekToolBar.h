#if !defined(AFX_SEEKTOOLBAR_H__8272425A_0B59_4BA8_992D_6B82F95CBC30__INCLUDED_)
#define AFX_SEEKTOOLBAR_H__8272425A_0B59_4BA8_992D_6B82F95CBC30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SeekToolBar.h : header file
//

//#include "MySliderControl.h"
#include "NewSliderCtrl.h"
#include "gui/AlphaToolbar/AlphaToolBar.h"

/////////////////////////////////////////////////////////////////////////////
// CSeekToolBar window

//const int nSeekMaxPos = 260;

class CSeekToolBar : public CAlphaToolBar
{
	CWnd*				m_pWnd;
	CNewSliderCtrl		m_Slider;
	UINT				m_nSeekID;
#ifdef USE_POS
	struct CPosStatic : CStatic	
	{	
		CFont				m_Font;
	} m_wndPosition;
#endif
// Construction
public:
	CSeekToolBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeekToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSeekToolBar();

	void				SetReceiver(CWnd* pWnd) { m_Slider.SetReceiver( m_pWnd = pWnd);};
	CNewSliderCtrl*		GetSlider() { return &m_Slider;}
//	CStatic*			GetPosStatic() { return &m_wndPosition;}

	void				EnableSeeker( bool bEnable);
	void				CreateSeeker(int size, int nGapRight, UINT nSeekID);
	void				SizeSeeker(int size, int nGapRight);

	void drawDropDownArrow(int btnIndex);

	// Generated message map functions
protected:
	//{{AFX_MSG(CSeekToolBar)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEEKTOOLBAR_H__8272425A_0B59_4BA8_992D_6B82F95CBC30__INCLUDED_)
