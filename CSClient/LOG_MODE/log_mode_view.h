#pragma once

#include "../UT/OXLayoutManager.h"
#include "../UT/OXTreeCtrl.h"
#include "../res/resource.h"

class CLogDoc;

class CLogView :	public CFormView
{
protected:
  DECLARE_DYNCREATE(CLogView)
  CLogView();
  virtual ~CLogView();

  //{{AFX_DATA(CLogView)
  enum
  {
	  IDD = IDD_LOG
  };
  //}}AFX_DATA
  bool							m_bInitialized;
  COXLayoutManager				m_LayoutManager;
  CTabCtrl						m_wndDebugSelector;
  size_t						m_nActiveTree ;

  std::vector< boost::shared_ptr<COXTreeCtrl> >			m_DebugWindowArr;
  std::vector< std::pair< std::wstring, size_t > >		m_DebugSystemArr;;		

  CLogDoc * GetDocument();

  COXTreeCtrl*	GetTree( LPCWSTR szSystem );
  void	InitTree( COXTreeCtrl* pTree, bool bFirst );
  void	InsertMessage( COXTreeCtrl* pTree, CTime timeMsg, int nType, const std::wstring& sMsg );

  //{{AFX_VIRTUAL(CLogView)
	virtual void OnInitialUpdate(  );
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

public:
  //{{AFX_MSG(CLogView)
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg void OnTabSelChange( NMHDR* pHdr, LRESULT* pRes );
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  
inline CLogDoc * CLogView::GetDocument() // non-debug version
{
  return reinterpret_cast<CLogDoc*>( m_pDocument );
}
#endif