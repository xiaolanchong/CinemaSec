#pragma once

#include "../ut/OXTreeCtrl.h"
#include "../ut/OXLayoutManager.h"

#include "../RoomChooserTab.h"
#include "afxcmn.h"

// CDiagnosticView form view
class CDiagnosticDoc;

class CDiagnosticView : public CFormView,
						public RoomChooserTab,
						public IRoomChangeObserver
{
	DECLARE_DYNCREATE(CDiagnosticView)

	bool	m_bInit;
protected:
	CDiagnosticView();           // protected constructor used by dynamic creation
	virtual ~CDiagnosticView();

	COXTreeCtrl			m_wndDiagnostic;
	COXLayoutManager	m_LayoutManager;
	CImageList			m_ilDiagnostic;
public:
	enum { IDD = IDD_DIAGNOSTIC };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	BOOL	Create(  CWnd* pParent, const CRect & rc, CCreateContext& cc, int i );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();
	virtual void OnRoomChange(int nRoomID);

	afx_msg void OnTimer( UINT_PTR nIDEvent );

	DECLARE_MESSAGE_MAP()
	void OnRClkNotify( NMHDR* pNMHDR, LRESULT* pResult  );

	CDiagnosticDoc*	GetDocument();
};
