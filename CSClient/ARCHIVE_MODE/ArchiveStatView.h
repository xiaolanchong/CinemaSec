#pragma once

#include "../UT/OXLayoutManager.h"
#include "../Gui/Subclass.h"
// use new button
#define USE_XP_HOVER_BUTTON
#ifdef USE_XP_HOVER_BUTTON
#include "../gui/HoverBitmapButton/XPStyleButtonST.h"
#else
#include "../UT/OXBitmapButton.h"
#endif
#include "../CommonView/ArrangeStatView.h"
// CVideoStatView view

//#define USE_HUMAN_COUNT

class CArchiveDoc;

class CArchiveStatView :	public CArrangeStatView
							
{
	DECLARE_DYNCREATE(CArchiveStatView)
protected:
	CArchiveStatView();           // protected constructor used by dynamic creation
	virtual ~CArchiveStatView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	COXLayoutManager	m_LayoutManager;
	CDateTimeCtrl		m_wndBeginTime;
	CDateTimeCtrl		m_wndEndTime;

	CThemeHelperST		m_ThemeHelper;
	CXPStyleButtonST	m_btnArchive;

	CToolTipCtrl		m_wndArchiveTooltip;

	CWnd*			m_pMessageWnd;

	enum 
	{
		Col_Name	= 0,
		Col_Number,
		Col_Date,
		Col_BeginTime,
		Col_EndTime
#ifdef USE_HUMAN_COUNT
		,Col_HumanNumber
#endif
	};

	virtual void OnDblClickItem(HTREEITEM hti, CPoint pt);
	virtual void OnRClickItem(HTREEITEM hti);

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct

//	virtual std::set< int >					GetRooms(int nCinemaID)			;
//	virtual std::set< int >					OnCinemaChange(int nCinemaID)	;
public:
	virtual void	OnRoomChange(int nRoomID)		;
	virtual void	Clear();
protected:
	void			UpdateTree(CTime timeBegin, CTime timeEnd);
	void			UpdateTreeWithCheck();
	void			UpdateTreeWithNoCheck();
	void			UpdateTreeOld();

	DECLARE_MESSAGE_MAP()
public:
	void	SetMessageWnd( CWnd* pWnd  ) { m_pMessageWnd = pWnd;};

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnDblClkNotify( NMHDR* pNMHDR, LRESULT* pResult  );
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnNotifyBegin( NMHDR* pHDR, LRESULT* pResult );
	afx_msg void OnButtonArchiveRefresh( );
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	CArchiveDoc* GetDocument();
};


