#pragma	  warning( disable : 4995 )		//deprecated functions

#include "stdafx.h"
#include "base_idle_view.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(BaseIdleView, CScrollView)

BEGIN_MESSAGE_MAP(BaseIdleView, CScrollView)
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
BaseIdleView::BaseIdleView()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
BaseIdleView::~BaseIdleView()
{
}


void BaseIdleView::OnDraw( CDC * )
{
}


//-------------------------------------------------------------------------------------------------
// BaseIdleView diagnostics.
//-------------------------------------------------------------------------------------------------
#ifdef _DEBUG
void BaseIdleView::AssertValid() const
{
  CScrollView::AssertValid();
}
void BaseIdleView::Dump( CDumpContext & dc ) const
{
  CScrollView::Dump(dc);
}
#endif //_DEBUG


//-------------------------------------------------------------------------------------------------
/** \brief Function is called during the idle cycle to process the current frame.

  \return  true, if the idle cycle should proceed, otherwise false. */
//-------------------------------------------------------------------------------------------------
bool BaseIdleView::OnIdle()
{
  return false;
}


