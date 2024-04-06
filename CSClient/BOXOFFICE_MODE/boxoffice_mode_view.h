#pragma once

#include "../res/resource.h"
#include "../RoomChooserTab.h"
#include "../UT/OXLayoutManager.h"
#include "../GUI/HtmlCtrl/HtmlCtrl.h"

class CBoxOfficeDoc;

class CBoxOfficeView :	public CFormView, 
						public RoomChooserTab,
						IRoomChangeObserver
{
protected:
  DECLARE_DYNCREATE(CBoxOfficeView)
  CBoxOfficeView();
  virtual ~CBoxOfficeView();

  //{{AFX_DATA(CAnchorView)
  enum
  {
	  IDD = IDD_BOXOFFICE
  };
  //}}AFX_DATA
  bool							m_bInitialized;
  COXLayoutManager				m_LayoutManager;

  CDateTimeCtrl					m_dtFrom;
  CDateTimeCtrl					m_dtTo;
  CHtmlCtrl						m_wndReport;

  CBoxOfficeDoc * GetDocument();
  void	OnRoomChange(int nRoomID);

  //{{AFX_VIRTUAL(CBoxOfficeView)
	virtual void OnInitialUpdate(  );
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

public:
  //{{AFX_MSG(CBoxOfficeView)
	afx_msg void OnGetData();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  
inline CBoxOfficeDoc * CBoxOfficeView::GetDocument() // non-debug version
{
  return reinterpret_cast<CBoxOfficeDoc*>( m_pDocument );
}
#endif