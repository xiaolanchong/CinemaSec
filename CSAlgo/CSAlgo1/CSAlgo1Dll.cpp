/****************************************************************************
  CSAlgo1Dll.cpp
  ---------------------
  begin     : Aug 2004
  modified  : 24 Aug 2005
  author(s) : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "CSAlgo1Dll.h"

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

CCSAlgo1Dll theDll;

const int DIALOG_TAB_WIDTH  = 320;     // the width of a property page in dialog units
const int DIALOG_TAB_HEIGHT = 200;     // the height of a property page in dialog units

BEGIN_MESSAGE_MAP(CCSAlgo1Dll, CWinApp)
END_MESSAGE_MAP()


//-------------------------------------------------------------------------------------------------
/** \brief Constructor. */
//-------------------------------------------------------------------------------------------------
CCSAlgo1Dll::CCSAlgo1Dll() : m_parameters( csalgo1::FIRST_CONTROL_IDENTIFIER )
{
  m_parameters.set_default();
  m_paramFileName.clear();
}


//-------------------------------------------------------------------------------------------------
/** \brief Function initializes each new instance of your application running under Windows. */
//-------------------------------------------------------------------------------------------------
BOOL CCSAlgo1Dll::InitInstance()
{
  if (!(CWinApp::InitInstance()))
    return FALSE;

  wchar_t fname[_MAX_PATH+16];
  DWORD   n = GetModuleFileName( AfxGetInstanceHandle(), fname, _MAX_PATH+1 );

  if (n > _MAX_PATH)
    return FALSE;
  m_dllName = fname;

  // Try to load parameter file or retain default parameters.
  m_parameters.set_default();
  m_paramFileName.clear();
  if (PathRenameExtension( fname, WILDCARD_PARAM+1 ))
  {
    m_paramFileName = fname;
    if ((CFileFind().FindFile( fname )) && !(csutility::LoadParameters( fname, m_parameters, 0 )))
      m_parameters.set_default();
  }
  return TRUE;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function shows dialog window for editing of algorithm parameters.

  \param  hParentWnd   the parent window handle.
  \param  pBinParams   in: pointer to the binary image of input parameters, out: modified parameters.
  \param  bSaveInFile  if nonzero, then save parameters in a file, whose name is derived from a library full-path name.
  \param  pOutput      pointer to the message handler.
  \return              Ok = S_OK, cancel = S_FALSE, otherwise error code E_xxx. */
//-------------------------------------------------------------------------------------------------
HRESULT ShowParameterDialogEx( IN HWND           hParentWnd,
                               IN OUT Int8Arr  * pBinParams,
                               IN bool           bSaveInFile,
                               IN IDebugOutput * pOutput )
{
  CSingleLock lock( &(theDll.m_dataLocker), TRUE );
  HRESULT retValue = csutility::ShowParameterDialog( hParentWnd, DIALOG_TAB_WIDTH, DIALOG_TAB_HEIGHT,
                                                     theDll.m_parameters, pBinParams, pOutput );

  if (bSaveInFile && (retValue == S_OK) && !(theDll.m_paramFileName.empty()))
  {
    if (!(csutility::SaveParameters( theDll.m_paramFileName.c_str(), theDll.m_parameters, pOutput )))
    {
      if (pOutput != 0)
        pOutput->PrintW( IDebugOutput::mt_error, ERROR_SAVE_PARAMETERS_IN_FILE );
      retValue = E_FAIL;
    }
  }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function shows dialog window for editing of algorithm parameters.

  \param  hParentWnd  the parent window handle.
  \param  pBinParams  in: pointer to the binary image of input parameters, out: modified parameters.
  \param  pOutput     pointer to the object that prints a warning or an error message.
  \return             Ok = S_OK, cancel = S_FALSE, otherwise error code E_xxx. */
//-------------------------------------------------------------------------------------------------
HRESULT ShowParameterDialog( IN HWND           hParentWnd,
                             IN OUT Int8Arr  * pBinParams,
                             IN IDebugOutput * pOutput )
{
  return ShowParameterDialogEx( hParentWnd, pBinParams, false, pOutput );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function fills out the binary image of algorithm parameters by default settings.

  \param  pBinParams   out: pointer to the binary image of modified parameters.
  \param  bSaveInFile  if nonzero, then save parameters in a file, whose name is derived from a library full-path name.
  \param  pOutput      pointer to the object that prints a warning or an error message.
  \return              Ok = S_OK, otherwise error code E_xxx. */
//-------------------------------------------------------------------------------------------------
HRESULT SetDefaultParametersEx( OUT Int8Arr     * pBinParams,
                                IN bool           bSaveInFile,
                                IN IDebugOutput * pOutput )
{
  CSingleLock lock( &(theDll.m_dataLocker), TRUE );
  HRESULT retValue = csutility::SetDefaultParameters( theDll.m_parameters, pBinParams, pOutput );

  if (bSaveInFile && SUCCEEDED(retValue) && !(theDll.m_paramFileName.empty()))
  {
    if (!(csutility::SaveParameters( theDll.m_paramFileName.c_str(), theDll.m_parameters, pOutput )))
    {
      if (pOutput != 0)
        pOutput->PrintW( IDebugOutput::mt_error, ERROR_SAVE_PARAMETERS_IN_FILE );
      retValue = E_FAIL;
    }
  }
  return retValue;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function fills out the binary image of algorithm parameters by default settings.

  \param  pBinParams  out: pointer to the binary image of modified parameters.
  \param  pOutput     pointer to the object that prints a warning or an error message.
  \return             Ok = S_OK, otherwise error code E_xxx. */
//-------------------------------------------------------------------------------------------------
HRESULT SetDefaultParameters( IN OUT Int8Arr  * pBinParams,
                              IN IDebugOutput * pOutput )
{
  return SetDefaultParametersEx( pBinParams, false, pOutput );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function gets binary parameter image.

  \param  pBinParams  pointer to the storage that receives binary image of parameters.
  \param  pOutput     pointer to the object that prints a warning or an error message.
  \return             Ok = S_OK, otherwise error code E_xxx. */
//-------------------------------------------------------------------------------------------------
HRESULT GetParameters( OUT Int8Arr     * pBinParams,
                       IN IDebugOutput * pOutput )
{
  CSingleLock lock( &(theDll.m_dataLocker), TRUE );
  return ((pBinParams != 0) &&
          csutility::ParametersToBinaryImage( theDll.m_parameters, *pBinParams, pOutput ));
}


//-------------------------------------------------------------------------------------------------
/** \brief Function loads parameters from a binary file.

  \param  filename  the name of parameter file.
  \param  pOutput   pointer to the object that prints a warning or an error message.
  \return           Ok = true. */
//-------------------------------------------------------------------------------------------------
bool LoadParameters( LPCWSTR        filename,
                     IDebugOutput * pOutput )
{
  CSingleLock lock( &(theDll.m_dataLocker), TRUE );
  return csutility::LoadParameters( filename, theDll.m_parameters, pOutput );
}


//-------------------------------------------------------------------------------------------------
/** \brief Function saves parameters to a binary file.

  \param  filename  the name of parameter file.
  \param  pOutput   pointer to the object that prints a warning or an error message.
  \return           Ok = true. */
//-------------------------------------------------------------------------------------------------
bool SaveParameters( LPCWSTR        filename,
                     IDebugOutput * pOutput )
{
  CSingleLock lock( &(theDll.m_dataLocker), TRUE );
  return csutility::SaveParameters( filename, theDll.m_parameters, pOutput );
}

