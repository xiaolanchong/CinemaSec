#pragma once

class CSettingsFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CSettingsFrame);
public:
  CSettingsFrame();
  virtual ~CSettingsFrame();

public:
  //{{AFX_VIRTUAL(CSettingsFrame)
  //}}AFX_VIRTUAL

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

protected:
  //{{AFX_MSG(CSettingsFrame)
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

