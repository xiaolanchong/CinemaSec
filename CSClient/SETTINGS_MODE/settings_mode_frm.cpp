#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "../ChildFrm.h"
#include "settings_mode_frm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSettingsFrame, CChildFrame)

BEGIN_MESSAGE_MAP(CSettingsFrame, CChildFrame)
  //{{AFX_MSG_MAP(CSettingsFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CSettingsFrame::CSettingsFrame()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CSettingsFrame::~CSettingsFrame()
{
}


//-------------------------------------------------------------------------------------------------
// CBoxOfficeFrame diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CSettingsFrame::AssertValid() const
{
  CChildFrame::AssertValid();
}
void CSettingsFrame::Dump( CDumpContext & dc ) const
{
  CChildFrame::Dump( dc );
}
#endif //_DEBUG