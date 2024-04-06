#pragma once

#include "../UT/OXTreeCtrl.h"
#include "../DBFacet/StatisticDB.h"

#include "../DBFacet/DBSerializer.h"
#include "../DBFacet/DBRoomChooser.h"
#include "../RoomChooserTab.h"
#include "../MainFrm.h"

#include "ControlHelper.h"
// CVideoStatView view

class CArrangeDocument;



class CArrangeStatView :	public CFormView, 
							public RoomChooserTab,
							public IRoomChangeObserver,
							public IChangeTabCallback
							
{
	DECLARE_DYNAMIC(CArrangeStatView)

	bool	m_bInit;
protected:
	ControlHelperWithFrame	m_CtrlHelper;

	CArrangeStatView(UINT nFormID);           // protected constructor used by dynamic creation
	virtual ~CArrangeStatView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	COXTreeCtrl		m_wndStatistics;
	CWnd*			m_pMessageWnd;

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnDblClickItem(HTREEITEM, CPoint pt ) = 0;
	virtual void OnRClickItem(HTREEITEM )  = 0;

	virtual void OnActivateTab( HWND hWnd );
public:
	virtual void	OnRoomChange(int nRoomID);
protected:

	DECLARE_MESSAGE_MAP()
public:
	void	SetMessageWnd( CWnd* pWnd  ) { m_pMessageWnd = pWnd;};

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblClkNotify( NMHDR* pNMHDR, LRESULT* pResult  );
	afx_msg void OnRClkNotify( NMHDR* pNMHDR, LRESULT* pResult  );
	afx_msg void OnDestroy();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	CArrangeDocument* GetDocument();
};


