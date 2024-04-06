#pragma once

class CLogView;


class CLogDoc : public CDocument
{
protected:
  DECLARE_DYNCREATE(CLogDoc)
  CLogDoc();
  virtual ~CLogDoc();

public:

  CLogView*		GetLogView();


public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

public:
  //{{AFX_MSG(CLogDoc)

  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


