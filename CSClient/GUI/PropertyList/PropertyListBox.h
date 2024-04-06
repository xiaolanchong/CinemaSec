#if !defined(AFX_PROPERTYLISTBOX_H__BC61E067_0A24_42E7_BAB5_16F21BC0C261__INCLUDED_)
#define AFX_PROPERTYLISTBOX_H__BC61E067_0A24_42E7_BAB5_16F21BC0C261__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyListBox.h : header file
//



#undef max
#undef min

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <vector>
#include <gdiplus.h>

#include "Callback.h"


/////////////////////////////////////////////////////////////////////////////
// CPropertyListBox window

class CPropertyListBox : public CListBox
{
	class GdiPlusInit
	{
		ULONG_PTR					gdiplusToken;
	public:
		GdiPlusInit()
		{
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			// Initialize GDI+.
			Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		}
		~GdiPlusInit()
		{
			Gdiplus::GdiplusShutdown(gdiplusToken);
		}
	} m_Init;	
// Construction
public:
	CPropertyListBox();

// Attributes
public:
/*
	enum FieldType
	{
		ft_edit,
		ft_combobox
	};
*/
	CString  operator[] ( CString Name) const;
	void	SetEditItem		(	CString Name, CString Value = CString(), 
								PropertyObserver* obs = NULL);
	void	SetNumEditItem		(	CString Name, CString Value = CString(), 
								PropertyObserver* obs = NULL);

	void	SetComboBoxItem	(	CString Name, const std::vector<CString>& Values, int nDefValue,
								bool bDropDown,
								PropertyObserver* obs = NULL);

	void	SetColorPicker( CString Name, COLORREF clDefault, PropertyObserver* obs = NULL );

//	void	SetReadOnly(  CString Name, CString Value  );

	void	Reset();

private:
	friend struct Find1st;
	struct Item
	{
		CString					m_Name;
		PropertyObserver*		m_Observer;
		boost::shared_ptr<CWnd> m_Control;


		Item(){}
		Item(const Item& it): m_Name(it.m_Name), m_Observer(it.m_Observer), m_Control(it.m_Control){}
		Item(CString Name, PropertyObserver* Obs, CWnd* pWnd): m_Name(Name), m_Observer(Obs), m_Control(pWnd){}
	};

	struct Cursor
	{
		HCURSOR Arrow, Size;
		Cursor() : 
		Arrow( AfxGetApp()->LoadStandardCursor( IDC_ARROW )), 
		Size( AfxGetApp()->LoadCursor(AFX_IDC_HSPLITBAR) ){}
	} m_Cursor;

	std::vector< Item >	m_Windows;
	CFont				m_Font;
	
	bool				m_bDragMode;;
	int					m_nDelemiter;

	bool	IsOnDelemiter(CPoint point) const;
	void	DrawEdge(Graphics& gr, const CRect& rcItem);

	CRect	GetItemWndRect(int nIndex) const;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyListBox)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyListBox)
	afx_msg void OnSelchange();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
	afx_msg void OnCancelMode();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	afx_msg void OnEditKillFocus(UINT nID);
	afx_msg void OnEditChange(UINT nID);
//	afx_msg void OnComboBoxKillFocus(int nID);
	afx_msg void OnComboBoxChange(UINT nID);
	afx_msg void OnComboBoxEditChange(UINT nID);
	DECLARE_MESSAGE_MAP()

	afx_msg void OnEnterMenuLoop( BOOL bIsTrackPopupMenu );

	virtual void PreSubclassWindow();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYLISTBOX_H__BC61E067_0A24_42E7_BAB5_16F21BC0C261__INCLUDED_)
