// LauncherView.h : interface of the CLauncherView class
//
#pragma once

#define USE_HTML_LOG

#include "SimpleLog.h"
#include "afxwin.h"

class CLauncherDoc;

class LauncherTag : public SelectionTag
{
public:
	virtual bool		operator == ( const SelectionTag& rhs )
	{
		// assume it's single
		const LauncherTag* pTag = dynamic_cast< const LauncherTag* >( &rhs );
		return pTag != 0;
	}
	virtual CString		GetTabName() 	
	{
		return CString( _T("Main server"));
	}
};

class CLauncherView :	public CSimpleLog
{
	bool				m_bInit;
protected: // create from serialization only
	CLauncherView();
	DECLARE_DYNCREATE(CLauncherView)

// Attributes
	BOOL	Create( CWnd* pParent, UINT nID, CCreateContext& cc );
// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CLauncherView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool	OnReceiveMessage( int nSeverity, LPCWSTR szMessage );
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	// When this flag checked, user can not control the server work manually
};

