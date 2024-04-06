#if !defined(AFX_REPORTPARAM_H__D489D95F_F1A7_438F_B688_6A79F30D27E2__INCLUDED_)
#define AFX_REPORTPARAM_H__D489D95F_F1A7_438F_B688_6A79F30D27E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER <= 1200
#pragma warning( disable : 4097 )
#endif

#include "../res/resource.h"
#include "../UT/OXLayoutManager.h"
#include "../RoomChooserTab.h"
/////////////////////////////////////////////////////////////////////////////
// CReportParamView view
class CReportDocument;

class CReportParamView :	public CFormView,
							public RoomChooserTab,
							public IRoomChangeObserver
{
	bool m_bInit;
	bool	m_bReportCreated;

	void	EnableAdditionalButtons( bool bEnable );
protected:
	CReportParamView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CReportParamView)
// Attributes
public:
// Operations
public:
	enum
	{
		IDD = IDD_REPORT
	};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportParamView)
	public:
	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* pDC) { UNREFERENCED_PARAMETER(pDC) ;}
	//}}AFX_VIRTUAL
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CDateTimeCtrl	m_wndBeginDate,	m_wndBeginTime, 
					m_wndEndDate,	m_wndEndTime;
	CTreeCtrl		m_wndRoom;
	CFont			m_fnTree;
	COXLayoutManager	m_LayoutManager;

	void	SetRadioState(bool bMeasure);

	virtual void	OnRoomChange(int nRoomID);
// Implementation
protected:
	virtual ~CReportParamView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CReportDocument* GetDocument();

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonCreate();
	afx_msg void OnButtonExport();
	afx_msg void OnRadioMeasure();
	afx_msg void OnRadioFilm();
	afx_msg void OnTreeClick( NMHDR* pNmHdr, LRESULT* pResult  );
//	afx_msg void OnRadioFilm();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPrintpreview();
	afx_msg void OnBnClickedButtonPrint();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTPARAM_H__D489D95F_F1A7_438F_B688_6A79F30D27E2__INCLUDED_)
