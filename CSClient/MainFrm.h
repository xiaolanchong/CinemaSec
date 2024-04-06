// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__92D2B66A_58EE_4FF4_9B58_0D7C650B44E8__INCLUDED_)
#define AFX_MAINFRM_H__92D2B66A_58EE_4FF4_9B58_0D7C650B44E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

#ifdef USE_GUILIB
#include "../GuiLib/Header/GuiMDIFrame.h"
#include "../GuiLib/header/Subclass.h"
#else
#include "gui/AlphaToolbar/AlphaToolBar.h"
#include "gui/Subclass.h"
#endif 

#define		WM_ADDCHILD WM_USER + 0xf3

#ifndef USE_GUILIB
#define CGuiMDIFrame	CMDIFrameWnd
#endif

class CMDIClientHook : public CSubclassWnd
{
public:
	BOOL Install(HWND hWndToHook);
protected:
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
};

struct IChangeTabCallback
{
	virtual void OnActivateTab( HWND hWnd ) = 0;
};

class CMainFrame : public CGuiMDIFrame
{
	//******************************************************************
	/////////////////////////////////////////////////////////////////////////////
	// CMDIClientHook 

	friend struct DeveloperChecker;

CMDIClientHook m_MDIClientHook;
	void	ChangeActiveView( UINT nViewID );
	void	OpenNewMode( LPCTSTR modeTemplateName );

	CArray<HWND,HWND>	m_ChildFrames;

	std::vector<IChangeTabCallback*>	m_CallbackArr;

	void	FireChangeTab( HWND hWnd );
protected: // create from serialization only

	DECLARE_DYNCREATE(CMainFrame)

public:

#ifndef USE_GUILIB
	CAlphaToolBar	m_wndToolBar;
	CStatusBar		m_wndStatusBar;

	BOOL InitToolBar(UINT nResID);
	BOOL InitStatusBar( UINT * pIndicators, int nSize );

	BOOL LoadFrame( UINT nIDResource,
		DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
		CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL );
	BOOL OnCreateClient(LPCREATESTRUCT lpcs,
		CCreateContext* /*pContext*/);

#endif

	void	AddChildFrame( CWnd* pFrame ) ;

	void	RegisterChangeTab( IChangeTabCallback* pInt );
	void	UnregisterChangeTab( IChangeTabCallback* pInt );
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void	OnUpdateFrameTitle(BOOL bAddToTitle);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
		CMainFrame();
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnInitMenuPopup( CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu );
	afx_msg void OnViewVideo();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateViewVideo(CCmdUI* pCmdUI);
	afx_msg void OnViewRoom();
	afx_msg void OnUpdateViewRoom(CCmdUI* pCmdUI);
	afx_msg LRESULT OnAddChild( WPARAM wParam, LPARAM lParam );
	afx_msg void OnViewAlgorithm();
	afx_msg void OnUpdateViewAlgorithm(CCmdUI* pCmdUI);

	afx_msg void OnViewMode( UINT nID );
	afx_msg void OnViewModeUpdate( CCmdUI* pCmdUI );

	afx_msg void OnCancelMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg LRESULT OnMDISetMenu(WPARAM, LPARAM){ return TRUE;}
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__92D2B66A_58EE_4FF4_9B58_0D7C650B44E8__INCLUDED_)
