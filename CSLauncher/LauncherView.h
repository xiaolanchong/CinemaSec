// LauncherView.h : interface of the CLauncherView class
//
#pragma once

#define USE_HTML_LOG

#include "UT/OXTreeCtrl.h"
#include "UT/OXLayoutManager.h"
#include "ut/OXDropEdit.h"
#include "afxwin.h"

#include <afxmt.h>
#include <list>
#include <mmsystem.h>
#include "../CSUtility/csutility.h"

class CBrowseEdit : public COXDropEdit<CEdit>
{
	CString		m_strFilter;
	CWnd*		m_pWnd;
public:
	virtual void OnDropButton();
	void		 SetFilter(CString str, CWnd* pWnd) { m_strFilter = str; m_pWnd = pWnd; }
};

class CLauncherDoc;

class CLauncherView : public CFormView, public IDebugOutput
{

	CCriticalSection cs;

	COXTreeCtrl			m_wndDebugMessage;
	CBrowseEdit			m_wndBrowseEdit;
	COXLayoutManager	m_LayoutMgr;

//	IMainServer*		m_pMainServer;
	int					m_nRoomID;
	bool				m_bInit;
	typedef std::list< std::pair< unsigned int, std::wstring> > MsgBuffer_t ;
	MsgBuffer_t m_MsgBuffer;

	
	void	PrintA( __int32 mt, LPCSTR szMessage);

protected: // create from serialization only
	CLauncherView();
	DECLARE_DYNCREATE(CLauncherView)

public:
	enum{ IDD = IDD_CSLAUNCHER_FORM };

// Attributes
public:
	CLauncherDoc* GetDocument() const;

	BOOL	Create( CWnd* pParent, UINT nID, CCreateContext& cc );

	void	SetRoomID( int nRoomID )	{ m_nRoomID = nRoomID;	}
	int		GetRoomID() const			{ return m_nRoomID;		}
	void	PrintW( __int32 mt, LPCWSTR szMessage);
// Operations
public:

// Overrides
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CLauncherView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg LRESULT OnDbgMessage( WPARAM, LPARAM );
	afx_msg LRESULT OnInitConfig( WPARAM wp, LPARAM lp);

	CEdit			m_edLineNumber;
	CSpinButtonCtrl m_spinLineNumber;
	// When this flag checked, user can not control the server work manually
};

#ifndef _DEBUG  // debug version in LauncherView.cpp
inline CLauncherDoc* CLauncherView::GetDocument() const
   { return reinterpret_cast<CLauncherDoc*>(m_pDocument); }
#endif

