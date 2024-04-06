#pragma once

#include "../UT/OXLayoutManager.h"
#include "DebugOuputList.h"
#include "TabSelection.h"
// CSimpleLog view

class CLauncherDoc; 

class CSimpleLog :	public CView,
					public DebugOuputList,
					public DebugOutputListCB,
					public TabSelection					
{
	enum
	{
		SHOW_NONE		= 0,
		SHOW_DEBUG		= 1 << 1,
		SHOW_TRACE		= 1	<< 2,
		SHOW_WARNING	= 1	<< 3,
		SHOW_ERROR		= 1	<< 4,
		SHOW_ALL		= (SHOW_DEBUG|SHOW_TRACE|SHOW_WARNING|SHOW_ERROR)
	};

	DWORD	m_dwShowFlags;

	bool	m_bInit;
	DECLARE_DYNCREATE(CSimpleLog)

	void	AddString( CMenu& menu, DWORD dwFlag,  LPCTSTR szItemName );
protected:
	COXLayoutManager	m_LayoutMgr;

	virtual bool	OnReceiveMessage( int nSeverity, LPCWSTR szMessage );
	virtual int		GetMaxLineNumber() ;

	bool	ShowMessageType( int nSeverity );
protected:
	CSimpleLog();           // protected constructor used by dynamic creation
	virtual ~CSimpleLog();

	CLauncherDoc* GetDocument() const;

	
public:
	virtual void OnInitialUpdate();
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnEraseBkgnd(CDC* pDC) { UNREFERENCED_PARAMETER(pDC); return TRUE; }
	afx_msg void OnNotifyRClick( NMHDR*, LRESULT * pResult );
};

#ifndef _DEBUG  // debug version in LauncherView.cpp
inline CLauncherDoc* CSimpleLog::GetDocument() const
{ return reinterpret_cast<CLauncherDoc*>(m_pDocument); }
#endif


