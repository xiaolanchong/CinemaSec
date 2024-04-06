#pragma once

class CMainFrame : public CMDIFrameWnd
{
  DECLARE_DYNAMIC(CMainFrame)

public:
  CStatusBar m_wndStatusBar;
  CToolBar   m_wndToolBar;

public:
  CMainFrame();
  virtual ~CMainFrame();
  virtual BOOL PreCreateWindow( CREATESTRUCT & cs );

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

protected:
  virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
  virtual BOOL OnCmdMsg( UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo );

public:
  DECLARE_MESSAGE_MAP()
  afx_msg int  OnCreate( LPCREATESTRUCT lpCreateStruct );
  afx_msg void OnSysCommand( UINT nID, LPARAM lParam );
  afx_msg void OnFileOpenPlaylist();
};

