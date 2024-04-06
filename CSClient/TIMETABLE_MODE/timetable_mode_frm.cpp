#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "../ChildFrm.h"
#include "timetable_mode_doc.h"
#include "timetable_mode_frm.h"
#include "timetable_mode_view.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTimeTableFrame, CChildFrame)

BEGIN_MESSAGE_MAP(CTimeTableFrame, CChildFrame)
  //{{AFX_MSG_MAP(CTimeTableFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CTimeTableFrame::CTimeTableFrame()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CTimeTableFrame::~CTimeTableFrame()
{
}


//-------------------------------------------------------------------------------------------------
// CBoxOfficeFrame diagnostics
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void CTimeTableFrame::AssertValid() const
{
  CChildFrame::AssertValid();
}
void CTimeTableFrame::Dump( CDumpContext & dc ) const
{
  CChildFrame::Dump( dc );
}
#endif //_DEBUG