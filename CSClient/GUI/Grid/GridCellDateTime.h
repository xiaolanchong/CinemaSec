// GridCellDateTime.h: interface for the CGridCellDateTime class.
//
// Provides the implementation for a datetime picker cell type of the
// grid control.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATETIMECELL_H__A0B7DA0A_0AFE_4D28_A00E_846C96D7507A__INCLUDED_)
#define AFX_DATETIMECELL_H__A0B7DA0A_0AFE_4D28_A00E_846C96D7507A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GridCell.h"
#include "afxdtctl.h"	// for CDateTimeCtrl

#define DTS_EX_SHOW_MINUTES		0x0001
#define DTS_EX_NO_INIT_VALUE	0x0002

class CGridCellDateTime : public CGridCell  
{
  friend class CGridCtrl;
  DECLARE_DYNCREATE(CGridCellDateTime)

  CTime m_cTime;
  DWORD m_dwStyle;
  DWORD m_dwExStyle;

public:
	CGridCellDateTime();
	CGridCellDateTime(DWORD dwStyle);
	virtual ~CGridCellDateTime();

  // editing cells
public:
	void Init(DWORD dwStyle, DWORD dwExStyle = 0);
	void Init(const CTime& Time, DWORD dwStyle, DWORD dwExStyle);
  virtual BOOL  Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
  virtual CWnd* GetEditWnd() const;
  virtual void  EndEdit();


  CTime* GetTime() {return &m_cTime;};
};

class CInPlaceDateTime : public CDateTimeCtrl
{
// Construction
public:
	CInPlaceDateTime(CWnd* pParent,         // parent
                   CRect& rect,           // dimensions & location
                   DWORD dwStyle,         // window/combobox style
				   DWORD dwExStyle,
                   UINT nID,              // control ID
                   int nRow, int nColumn, // row and column
                   COLORREF crFore, COLORREF crBack,  // Foreground, background colour
                   CTime* pcTime,
          		   UINT nFirstChar);      // first character to pass to control

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceList)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceDateTime();
    void EndEdit();

// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceList)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	//afx_msg void OnSelendOK();

	DECLARE_MESSAGE_MAP()

private:
    CTime*   m_pcTime;
	int		 m_nRow;
	int		 m_nCol;
 	UINT     m_nLastChar; 
	BOOL	 m_bExitOnArrows; 
    COLORREF m_crForeClr, m_crBackClr;
};

#endif // !defined(AFX_DATETIMECELL_H__A0B7DA0A_0AFE_4D28_A00E_846C96D7507A__INCLUDED_)
