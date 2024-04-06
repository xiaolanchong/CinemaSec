#pragma once

class CCamAnalyzerDoc;

class CCamAnalyzerFrame : public CMDIChildWnd
{
  friend class CCamAnalyzerDoc;

public:
  DECLARE_DYNCREATE(CCamAnalyzerFrame)

protected:
  CStatusBar m_wndStatusBar;

public:
  CCamAnalyzerFrame();
  virtual ~CCamAnalyzerFrame();
  virtual BOOL PreCreateWindow( CREATESTRUCT & cs );
  // void SetCursorIndicator( int x, int y );

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

protected:
  DECLARE_MESSAGE_MAP()
  afx_msg int     OnCreate( LPCREATESTRUCT lpCreateStruct );
  afx_msg LRESULT OnMyUpdateStatusBarText( WPARAM, LPARAM );
  afx_msg LRESULT OnMyUpdateStateIndicators( WPARAM, LPARAM );
  afx_msg LRESULT OnMyUpdateWindowTitle( WPARAM, LPARAM );
};

