#if !defined(AFX_REPORT_H__D489D95F_F1A7_438F_B688_6A79F30D27E2__INCLUDED_)
#define AFX_REPORT_H__D489D95F_F1A7_438F_B688_6A79F30D27E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER <= 1200
#pragma warning( disable : 4097 )
#endif

#include "../GUI/DHTML/DHTMLUIIncludes.h"
//#include <afxhtml.h>

namespace XmlLite
{
	class XMLDocument;
}

/////////////////////////////////////////////////////////////////////////////
// CReportView view
class CReportDocument;

#define ParentClass	CHtmlViewEx

class CReportView : public ParentClass
{
	bool	m_bInit;
protected:
	CReportView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CReportView)
// Attributes
public:
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportView)
	public:
	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* pDC) { UNREFERENCED_PARAMETER(pDC) ;}
	//}}AFX_VIRTUAL

	void	CreateReport( XmlLite::XMLDocument& xmlDoc );
	void	PreviewReport();
	void	PrintReport();
	void	SaveReport();
// Implementation
protected:
	virtual ~CReportView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CReportDocument* GetDocument();

	// Generated message map functions
protected:
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORT_H__D489D95F_F1A7_438F_B688_6A79F30D27E2__INCLUDED_)
