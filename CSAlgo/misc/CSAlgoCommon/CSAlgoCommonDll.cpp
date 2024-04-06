/****************************************************************************
  CSAlgoCommonDll.cpp
  ---------------------
  begin     : Aug 2004
  modified  : 24 Aug 2005
  author(s) : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "CSAlgoCommonDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//  Note!
//
//  If this DLL is dynamically linked against the MFC
//  DLLs, any functions exported from this DLL which
//  call into MFC must have the AFX_MANAGE_STATE macro
//  added at the very beginning of the function.
//
//  For example:
//
//  extern "C" BOOL PASCAL EXPORT ExportedFunction()
//  {
//    AFX_MANAGE_STATE(AfxGetStaticModuleState());
//    // normal function body here
//  }
//
//  It is very important that this macro appear in each
//  function, prior to any calls into MFC.  This means that
//  it must appear as the first statement within the 
//  function, even before any object variable declarations
//  as their constructors may generate calls into the MFC
//  DLL.
//
//  Please see MFC Technical Notes 33 and 58 for additional
//  details.
//

CCSAlgoCommonDll theDll;

BEGIN_MESSAGE_MAP(CCSAlgoCommonDll, CWinApp)
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CCSAlgoCommonDll::CCSAlgoCommonDll()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief Destructor. */
//-------------------------------------------------------------------------------------------------
CCSAlgoCommonDll::~CCSAlgoCommonDll()
{
}


//-------------------------------------------------------------------------------------------------
/** \brief InitInstance(). */
//-------------------------------------------------------------------------------------------------
BOOL CCSAlgoCommonDll::InitInstance()
{
  if (!(CWinApp::InitInstance()))
    return FALSE;
  return TRUE;
}

