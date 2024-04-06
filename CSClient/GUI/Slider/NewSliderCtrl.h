#if !defined(AFX_NEWSLIDERCTRL_H__232E9873_776C_4415_BCCC_D42FB500AA59__INCLUDED_)
#define AFX_NEWSLIDERCTRL_H__232E9873_776C_4415_BCCC_D42FB500AA59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewSliderCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewSliderCtrl window
/*
namespace Slider
{
	const int wm_slider = WM_APP + 0xf0;
//	sm_
}
*/

const int wm_slider	 = WM_USER + 0x4e;
const int sm_start_drag = 0;
const int sm_end_drag	= 1;
const int sm_change_pos	= 2;

class CNewSliderCtrl : public CWnd
{
public:
	typedef		__int64 ValueType;
private:

// Construction
//public:

	CWnd*	m_pReceiver;
	ValueType		m_nMaxValue;
	ValueType		m_nCurValue;

	COLORREF	m_crBackGround;
	COLORREF	m_crForeGround;

	bool		m_bDragMode;
	void		Normalize();
	void		Draw(CDC* pDC) const ;
public:
	CNewSliderCtrl();

	void	SetMax(__int64 nMax) { m_nMaxValue = nMax;};
	ValueType		GetMax()	const { return m_nMaxValue;}
	ValueType		GetCurrent() const { return m_nCurValue;}
	void	SetCurrent(__int64 nCurrent) 
	{ 
		m_nCurValue = nCurrent; 
	/*	if( m_nCurValue > 5 )
		{
			int a =1;
		}*/
		if( IsWindow(GetSafeHwnd()))Invalidate(FALSE);
	}; 
	void	Step();
	void	SetReceiver(CWnd* pWnd) { m_pReceiver = pWnd;} 
// Attributes
public:

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewSliderCtrl)
	//}}AFX_VIRTUAL
// Implementation
public:
	virtual ~CNewSliderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNewSliderCtrl)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSLIDERCTRL_H__232E9873_776C_4415_BCCC_D42FB500AA59__INCLUDED_)
