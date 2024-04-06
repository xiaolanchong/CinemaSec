#include "stdafx.h"
#include "csalgoapp.h"

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

// CCSAlgoDll
BEGIN_MESSAGE_MAP(CCSAlgoDll, CWinApp)
END_MESSAGE_MAP()


// CCSAlgoDll construction
CCSAlgoDll::CCSAlgoDll()
{
  // TODO: add construction code here,
  // Place all significant initialization in InitInstance
}


// The one and only CCSAlgoDll object
CCSAlgoDll theDll;


// CCSAlgoDll initialization
BOOL CCSAlgoDll::InitInstance()
{
  CWinApp::InitInstance();

  wchar_t name[_MAX_PATH+16];
  DWORD   n = GetModuleFileName( AfxGetInstanceHandle(), name, _MAX_PATH+1 );

  if (n > _MAX_PATH)
    return FALSE;

  m_dllName = name;
  return TRUE;
}

