#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "log_mode_doc.h"
#include "log_mode_view.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CLogDoc, CDocument)

BEGIN_MESSAGE_MAP(CLogDoc, CDocument)
  //{{AFX_MSG_MAP(CLogDoc)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CLogDoc::CLogDoc()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CLogDoc::~CLogDoc()
{
}


//-------------------------------------------------------------------------------------------------
// CLogDoc diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CLogDoc::AssertValid() const
{
  CDocument::AssertValid();
}
void CLogDoc::Dump( CDumpContext & dc ) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief Function returns pointer to CLogView class. */
//-------------------------------------------------------------------------------------------------
CLogView * CLogDoc::GetLogView()
{
  for (POSITION pos = GetFirstViewPosition(); pos != 0;)
  {
    CView * p = GetNextView( pos );
    if ((p != 0) && p->IsKindOf( RUNTIME_CLASS( CLogView ) ))
    {
      return reinterpret_cast<CLogView*>( p );
    }
  }
  return 0;
}






