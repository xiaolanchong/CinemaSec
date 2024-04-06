#pragma once

#if _MSC_VER <= 1200
#pragma warning( disable : 4097 )
#endif 

#include "../../CSEngine/DBFacet/TimeHelper.h"

class CBoxOfficeView;


class CBoxOfficeDoc : public CDocument
{
protected:
  DECLARE_DYNCREATE(CBoxOfficeDoc)
  CBoxOfficeDoc();
  virtual ~CBoxOfficeDoc();

public:

  CBoxOfficeView*		GetBoxOfficeView();
  CString				CreateReport( int nCinemaID, CTime timeFrom, CTime timeTo ) ;


public:
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif

public:
  //{{AFX_MSG(CBoxOfficeDoc)

  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


