/********************************************************************************
  mainfrm.h
  ---------------------
  begin     : Aug 2005
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  e-mail    : aaah@mail.ru, aaahaaah@hotmail.com
********************************************************************************/

#pragma once

//=================================================================================================
/** \class CMainFrame.
    \brief CMainFrame. */
//=================================================================================================
class CMainFrame : public CFrameWnd
{
  friend class CTrackApp;
  friend class PlayToolBar;

public:
  virtual ~CMainFrame();
  CMainFrame();
  DECLARE_DYNAMIC(CMainFrame)

public:
  virtual BOOL PreCreateWindow( CREATESTRUCT & cs );
  virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
  virtual BOOL OnCmdMsg( UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo );

  void SetFrameIndexIndicator();
  void SetFrameRateIndicator();
  void SetSpeedIndicator();

  void OpenNewVideoFile( LPCTSTR name );
  BOOL OnPlayCommand( unsigned int menuItemId );
  BOOL OnUpdatePlayCommand( CCmdUI * pCmdUI );
  virtual void FrameHasChanged( BOOL bInvalidateView );

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

protected:
  CStatusBar  m_wndStatusBar;
  PlayToolBar m_wndToolBar;
  CClientView m_view;

public:
  DECLARE_MESSAGE_MAP()
  afx_msg int  OnCreate( LPCREATESTRUCT lpCreateStruct );
  afx_msg void OnSetFocus( CWnd * pOldWnd );
  afx_msg void OnFileOpen();
  afx_msg void OnUpdateFileOpen( CCmdUI * pCmdUI );
  afx_msg void OnViewVideoInfo();
  afx_msg void OnUpdateViewVideoInfo( CCmdUI * pCmdUI );
  afx_msg void OnDropFiles( HDROP hDropInfo );
};

