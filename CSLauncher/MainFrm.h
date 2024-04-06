// MainFrm.h : interface of the CMainFrame class
//
#include "UT/OXLayoutManager.h"
#include "TabViews/TabSelection.h"

#pragma once
class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	BOOL	AddTab( boost::shared_ptr< SelectionTag >  TagForTab );

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void ChangeView(CView*);

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CReBar		m_wndMainReBar;
	CDialogBar	m_wndSelectBar;

	COXLayoutManager m_LayoutManager;

	std::vector<boost::shared_ptr< SelectionTag > > m_TagArr;

	CView*		GetTagView(boost::shared_ptr< SelectionTag > Tag);

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelectTab( NMHDR * pHdr, LRESULT * pResult  );
	afx_msg	LRESULT	OnPingMsg( WPARAM wparam, LPARAM lparam );

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};


