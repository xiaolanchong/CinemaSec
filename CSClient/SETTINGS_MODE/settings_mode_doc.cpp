#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "settings_mode_doc.h"
#include "settings_mode_view.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSettingsDoc, CDocument)

BEGIN_MESSAGE_MAP(CSettingsDoc, CDocument)
  //{{AFX_MSG_MAP(CSettingsDoc)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CSettingsDoc::CSettingsDoc()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CSettingsDoc::~CSettingsDoc()
{
}


//-------------------------------------------------------------------------------------------------
// CDiagnosticView diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CSettingsDoc::AssertValid() const
{
  CDocument::AssertValid();
}
void CSettingsDoc::Dump( CDumpContext & dc ) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief Function returns pointer to CBoxOfficeView class. */
//-------------------------------------------------------------------------------------------------
CSettingsView * CSettingsDoc::GetSettingsView()
{
  for (POSITION pos = GetFirstViewPosition(); pos != 0;)
  {
    CView * p = GetNextView( pos );
    if ((p != 0) && p->IsKindOf( RUNTIME_CLASS( CSettingsView ) ))
    {
      return reinterpret_cast<CSettingsView*>( p );
    }
  }
  return 0;
}
