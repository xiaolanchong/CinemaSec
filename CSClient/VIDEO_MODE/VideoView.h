#if !defined(AFX_VIDEOVIEW_H__D489D95F_F1A7_438F_B688_6A79F30D27E2__INCLUDED_)
#define AFX_VIDEOVIEW_H__D489D95F_F1A7_438F_B688_6A79F30D27E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoView.h : header file
//
//#include <afxtempl.h>

#if _MSC_VER <= 1200
#pragma warning( disable : 4097 )
#endif

#include "../CommonView/ArrangeView.h"
/////////////////////////////////////////////////////////////////////////////
// CVideoView view
class CVideoDocument;

class CVideoView : public CArrangeView
{
protected:
	CVideoView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CVideoView)
// Attributes
public:
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoView)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CVideoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CVideoDocument* GetDocument();

	// Generated message map functions
protected:
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOVIEW_H__D489D95F_F1A7_438F_B688_6A79F30D27E2__INCLUDED_)
