/****************************************************************************
  CSAlgo2Dll.cpp
  ---------------------
  begin     : Aug 2004
  modified  : 24 Aug 2005
  author(s) : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
****************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "CSAlgo2Dll.h"

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

CCSAlgo2Dll theDll;

const int DIALOG_TAB_WIDTH  = 340;     // the width of a property page in dialog units
const int DIALOG_TAB_HEIGHT = 180;     // the height of a property page in dialog units

BEGIN_MESSAGE_MAP(CCSAlgo2Dll, CWinApp)
END_MESSAGE_MAP()


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Constructor. */
///////////////////////////////////////////////////////////////////////////////////////////////////
CCSAlgo2Dll::CCSAlgo2Dll() : m_parameters( csalgo2::FIRST_CONTROL_IDENTIFIER )
{
  m_parameters.set_default();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function initializes each new instance of your application running under Windows. */
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CCSAlgo2Dll::InitInstance()
{
  if (!(CWinApp::InitInstance()))
    return FALSE;

  wchar_t fname[_MAX_PATH+16];
  if (GetModuleFileName( AfxGetInstanceHandle(), fname, _MAX_PATH ) > _MAX_PATH)
    return FALSE;
  m_dllName = fname;

  // Try to load parameter file or retain default parameters.
  m_parameters.set_default();
  m_paramFileName.clear();
  if (PathRenameExtension( fname, WILDCARD_PARAM+1 ))
  {
    m_paramFileName = fname;
    if (!((CFileFind()).FindFile( fname )) ||
        !(csutility::LoadParameters( fname, m_parameters, 0 )))
    {
      m_parameters.set_default();
      csutility::SaveParameters( fname, m_parameters, 0 );
    }
  }

  // Generate trees of learned samples per object type.
/*
  PathRemoveFileSpec( fname ); PathAppend( fname, ALGO2_EMPTY_SEQUENCE );
  if (!CreateTreeOfSampleImages( fname, m_emptyImages, m_emptyTree ))
    return FALSE;
  PathRemoveFileSpec( fname ); PathAppend( fname, ALGO2_CLOTH_SEQUENCE );
  //if (!CreateTreeOfSampleImages( fname, m_clothImages, m_clothTree ))
  //  return FALSE;
  PathRemoveFileSpec( fname ); PathAppend( fname, ALGO2_HUMAN_SEQUENCE );
  //if (!CreateTreeOfSampleImages( fname, m_humanImages, m_humanTree ))
  //  return FALSE;
*/
  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function creates tree of sample images from a resource file (video sequence).

  \param  fileName  the name of resource file.
  \param  samples   array of sample images loaded from the resource file.
  \param  tree      resultant tree of samples images.
  \return           Ok = true. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool CCSAlgo2Dll::CreateTreeOfSampleImages( LPCWSTR fileName, SampleArr & samples, ImageTree & tree )
{
  typedef  std::auto_ptr<avideolib::IVideoReader>  ReaderPtr;

  samples.clear();
  tree.Clear();

  // Create video sequence reader.
  ReaderPtr reader;
  reader.reset( avideolib::CreateAVideoReader( fileName, 0, INVERT_AXIS_Y ) );
  if (reader.get() == 0)
    return 0;

  // Get sequence info.
  alib::TStateFlag flag;
  flag.data = avideolib::IBaseVideo::WIDTH;
  if (!(reader->GetData( &flag ))) return false;
  int width = flag.data;
  flag.data = avideolib::IBaseVideo::HEIGHT;
  if (!(reader->GetData( &flag ))) return false;
  int height = flag.data;
  flag.data = avideolib::IBaseVideo::FRAME_NUMBER;
  if (!(reader->GetData( &flag ))) return false;
  int frameNum = flag.data;
  if ((width != 64) || (height != 64) || (frameNum == 0))
    return false;

  // Raed samples from the video sequence.
  samples.resize( frameNum );
  for (int i = 0; i < frameNum; i++)
  {
    int f = -1;
    avideolib::IVideoReader::ReturnCode res = reader->ReadFrame( &(samples[i]), -1, &f, 0 );
    if ((res != avideolib::IVideoReader::NORMAL) && (res != avideolib::IVideoReader::END_IS_REACHED))
      return false;
    if (f != i)
      return false;
  }

  // Generate tree.
  return tree.Create( &(*(samples.begin())), &(*(samples.end())) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function shows dialog window for editing of algorithm parameters.

  \param  hParentWnd   the parent window handle.
  \param  pBinParams   in: pointer to the binary image of input parameters, out: modified parameters.
  \param  bSaveInFile  if nonzero, then save parameters in a file, whose name is derived from a library full-path name.
  \param  pOutput      pointer to the message handler.
  \return              Ok = S_OK, cancel = S_FALSE, otherwise error code E_xxx. */
///////////////////////////////////////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function shows dialog window for editing of algorithm parameters.

  \param  hParentWnd  the parent window handle.
  \param  pBinParams  in: pointer to the binary image of input parameters, out: modified parameters.
  \param  pOutput     pointer to the object that prints a warning or an error message.
  \return             Ok = S_OK, cancel = S_FALSE, otherwise error code E_xxx. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT ShowParameterDialog( IN HWND           hParentWnd,
                             IN OUT Int8Arr  * pBinParams,
                             IN IDebugOutput * pOutput )
{
  return ShowParameterDialogEx( hParentWnd, pBinParams, false, pOutput );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function fills out the binary image of algorithm parameters by default settings.

  \param  pBinParams   out: pointer to the binary image of modified parameters.
  \param  bSaveInFile  if nonzero, then save parameters in a file, whose name is derived from a library full-path name.
  \param  pOutput      pointer to the object that prints a warning or an error message.
  \return              Ok = S_OK, otherwise error code E_xxx. */
///////////////////////////////////////////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function fills out the binary image of algorithm parameters by default settings.

  \param  pBinParams  out: pointer to the binary image of modified parameters.
  \param  pOutput     pointer to the object that prints a warning or an error message.
  \return             Ok = S_OK, otherwise error code E_xxx. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT SetDefaultParameters( IN OUT Int8Arr  * pBinParams,
                              IN IDebugOutput * pOutput )
{
  return SetDefaultParametersEx( pBinParams, false, pOutput );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function gets binary parameter image.

  \param  pBinParams  pointer to the storage that receives binary image of parameters.
  \param  pOutput     pointer to the object that prints a warning or an error message.
  \return             Ok = S_OK, otherwise error code E_xxx. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT GetParameters( OUT Int8Arr     * pBinParams,
                       IN IDebugOutput * pOutput )
{
  CSingleLock lock( &(theDll.m_dataLocker), TRUE );
  return ((pBinParams != 0) &&
          csutility::ParametersToBinaryImage( theDll.m_parameters, *pBinParams, pOutput ));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function loads parameters from a binary file.

  \param  filename  the name of parameter file.
  \param  pOutput   pointer to the object that prints a warning or an error message.
  \return           Ok = true. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool LoadParameters( LPCWSTR        filename,
                     IDebugOutput * pOutput )
{
  CSingleLock lock( &(theDll.m_dataLocker), TRUE );
  return csutility::LoadParameters( filename, theDll.m_parameters, pOutput );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function saves parameters to a binary file.

  \param  filename  the name of parameter file.
  \param  pOutput   pointer to the object that prints a warning or an error message.
  \return           Ok = true. */
///////////////////////////////////////////////////////////////////////////////////////////////////
bool SaveParameters( LPCWSTR        filename,
                     IDebugOutput * pOutput )
{
  CSingleLock lock( &(theDll.m_dataLocker), TRUE );
  return csutility::SaveParameters( filename, theDll.m_parameters, pOutput );
}

