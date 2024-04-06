#pragma once

class CBoxOfficeFrame : public CChildFrame
{
public:
  DECLARE_DYNCREATE(CBoxOfficeFrame)
  CBoxOfficeFrame();
  virtual ~CBoxOfficeFrame();

  bool	m_bSplitterCreated;
public:
  //{{AFX_VIRTUAL(CAlgoDebugFrame)
  //}}AFX_VIRTUAL

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

protected:
  //{{AFX_MSG(CAlgoDebugFrame)
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize( UINT nType, int cx, int cy  );
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

