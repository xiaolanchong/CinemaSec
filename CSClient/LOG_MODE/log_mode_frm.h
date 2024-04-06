#pragma once

class CLogFrame : public CChildFrame
{
public:
  DECLARE_DYNCREATE(CLogFrame)
  //virtual ~CLogFrame();

public:
  //{{AFX_VIRTUAL(CAlgoDebugFrame)
  //}}AFX_VIRTUAL

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

protected:
  //{{AFX_MSG(CLogFrame)
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

