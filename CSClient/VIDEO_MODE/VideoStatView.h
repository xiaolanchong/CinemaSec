#pragma once

#include "../UT/OXLayoutManager.h"
#include "../gui/ReadOnlyEdit/ReadOnlyEdit.h"

#include "../gui/AlphaToolbar/AlphaToolBar.h"
#include "../CommonView/ArrangeStatView.h"
#include "../gui/HoverBitmapButton/XPStyleButtonST.h"
// CVideoStatView view

class CVideoDocument;

class CToolBarNoDisable : public CAlphaToolBar
{
public:
	void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
	{
		//UpdateDialogControls(pTarget, bDisableIfNoHndler);
		pTarget = (CFrameWnd*)GetParent();
		CAlphaToolBar::OnUpdateCmdUI( pTarget, bDisableIfNoHndler );
	}
};

class CVideoStatView :	public CArrangeStatView
{
	DECLARE_DYNCREATE(CVideoStatView)

	// IStatisticObserver
	virtual void OnStatisticsUpdate(/*int nRoomID*/);

protected:
	CVideoStatView();           // protected constructor used by dynamic creation
	virtual ~CVideoStatView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	COXLayoutManager	m_LayoutManager;

	CReadOnlyEdit	m_edCurrentFilm;
#if 0
	CThemeHelperST		m_ThemeHelper;
	CXPStyleButtonST	m_btnPlay;
#endif
	CToolBarNoDisable	m_wndVideoControlBar;
	CWnd*			m_pMessageWnd;
	bool			m_bVideoMode;

	enum 
	{
		Col_Time = 0,
		Col_Number
	};
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnDblClickItem(HTREEITEM hti, CPoint pt);
	virtual void OnRClickItem(HTREEITEM hti);

	void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

	DECLARE_MESSAGE_MAP()
public:
	void	OnRoomChange(int nRoomID);
	void	SetMessageWnd( CWnd* pWnd  ) { m_pMessageWnd = pWnd;};
	void	UpdateFilm();
	void	UpdateTree(CTime timeBegin, CTime timeEnd);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnButtonPlay();
	afx_msg LRESULT OnUpdateUI(WPARAM wParam, LPARAM);
	afx_msg void OnUpdateButtonPlay(CCmdUI* pCmdUI);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	CVideoDocument* GetDocument();
};


