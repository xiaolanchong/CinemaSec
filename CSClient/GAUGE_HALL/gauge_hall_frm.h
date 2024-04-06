#pragma once

#include "ChildFrm.h"
#include "../UT/OXLayoutManager.h"
#include "../RoomChooserTab.h"

class CGaugeHallFrame : public CChildFrame,
						public RoomChooserTab,
						public IRoomChangeObserver
{
//	int m_nImageNumber;

public:
	DECLARE_DYNCREATE(CGaugeHallFrame)
	CGaugeHallFrame();
	virtual ~CGaugeHallFrame();

	bool	SetImageButtonText(const std::vector<int>& Images );
protected:
	CStatusBar m_wndStatusBar;
	CToolBar   m_wndToolBar;

	CReBar	 m_wndMainReBar;
	CDialogBar m_wndSelectBar;
	COXLayoutManager	m_LayoutManager;
	CTabCtrl			m_wndCamera;

	virtual void	OnRoomChange(int nRoomID);
	virtual void	OnCinemaChange(int nRoomID);
	void	OnRoomChange(int nRoomID, bool bNotify);
public:
	//{{AFX_VIRTUAL(CGaugeHallFrame)
	virtual BOOL PreCreateWindow( CREATESTRUCT & cs );
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
	virtual BOOL OnCmdMsg( UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo );
	//}}AFX_VIRTUAL

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif
	int			GetCurCamera();
	bool		ShowAllCameras();
	bool		IsReady() { return m_wndCamera.GetSafeHwnd() != 0 && GetActiveDocument();}
	void		InitRoomChange();

protected:
  //{{AFX_MSG(CGaugeHallFrame)
  afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
  afx_msg void OnCancelMode();

  afx_msg void OnCameraChange(NMHDR* pHdr, LRESULT* pResult);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

