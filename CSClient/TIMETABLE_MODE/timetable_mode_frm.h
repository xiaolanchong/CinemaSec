#pragma once

class CTimeTableFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CTimeTableFrame);
public:
  CTimeTableFrame();
  virtual ~CTimeTableFrame();

public:
  //{{AFX_VIRTUAL(CTimeTableFrame)
  //}}AFX_VIRTUAL

public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

protected:
  //{{AFX_MSG(CTimeTableFrame)
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

