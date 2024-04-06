#pragma once

class CDiagnosticFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CDiagnosticFrame);
public:
  CDiagnosticFrame();
  virtual ~CDiagnosticFrame();

public:
  //{{AFX_VIRTUAL(CAlgoDebugFrame)
  //}}AFX_VIRTUAL

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

protected:
  //{{AFX_MSG(CDiagnosticFrame)
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize( UINT nType, int cx, int cy  );
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

