/****************************************************************************
  ParameterFunctions.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csutility
{

//-------------------------------------------------------------------------------------------------
/** \brief Function initializes parameter structure from the binary image of parameters.

  \param  binParams   the binary image of input parameters, empty container means default settings.
  \param  parameters  out: reference to the parameter structure to be initialized.
  \param  pOutput     pointer to the message handler.
  \return             Ok = true. */
//-------------------------------------------------------------------------------------------------
template< class PARAMETERS >
bool BinaryImageToParameters( const Int8Arr & binParams,
                              PARAMETERS    & parameters,
                              IDebugOutput  * pOutput )
{
  if (binParams.empty())
  {
    parameters.set_default();
    return true;
  }

  LPCWSTR text = reinterpret_cast<LPCWSTR>( &(*(binParams.begin())) );
  int     length = (int)(binParams.size()/sizeof(wchar_t));

  if ((length > 0) && (text[length-1] == L'\0'))
  {
    std::wstringstream strm;
    strm << text;
    if (parameters.load( strm ))
      return true;
  }

  parameters.set_default();
  if (pOutput != 0)
    pOutput->PrintW( IDebugOutput::mt_error,
      L"Failed to load parameters from binary image.\nDefault ones will be set on." );
  return false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function copies parameter structure to the binary image.

  \param  parameters  the source parameter structure.
  \param  binParams   the binary image of parameters to be filled out.
  \param  pOutput     pointer to the message handler.
  \return             Ok = true. */
//-------------------------------------------------------------------------------------------------
template< class PARAMETERS >
bool ParametersToBinaryImage( const PARAMETERS & parameters,
                              Int8Arr          & binParams,
                              IDebugOutput     * pOutput )
{
  std::wstringstream strm;
  binParams.clear();
  if (!(parameters.save( strm )))
  {
    if (pOutput != 0)
      pOutput->PrintW( IDebugOutput::mt_error, L"Failed to save parameters in binary image" );
    return false;
  }
  std::wstring text = strm.str();
  Int8Arr::const_pointer F = reinterpret_cast<Int8Arr::const_pointer>( text.c_str() );
  Int8Arr::const_pointer L = F + (int)((text.size()+1) * sizeof(wchar_t));
  binParams.assign( F, L );
  return true;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function show dialog of parameter editing.

  \param  hParentWnd    the parent window handle.
  \param  dialogWidth   the width of parameter dialog.
  \param  dialogHeight  the height of parameter dialog.
  \param  parameters    parameter structure that can be modified on output.
  \param  pBinParams    pointer to the binary image of modified parameters.
  \param  pOutput       pointer to the message handler.
  \return               Ok = S_OK, cancel = S_FALSE, otherwise error code. */
//-------------------------------------------------------------------------------------------------
template< class PARAMETERS >
HRESULT ShowParameterDialog( HWND           hParentWnd,
                             const int      dialogWidth,
                             const int      dialogHeight,
                             PARAMETERS   & parameters,
                             Int8Arr      * pBinParams,
                             IDebugOutput * pOutput )
{
  if ((pBinParams != 0) && !BinaryImageToParameters( *pBinParams, parameters, pOutput ))
    return E_INVALIDARG; 

  CWnd * pParent = ((hParentWnd == 0) ? ((CWnd*)0) : (CWnd::FromHandle( hParentWnd )));
  if ((CParameterDialog( parameters, pParent, dialogWidth, dialogHeight )).DoModal() == IDOK)
  {
    return (((pBinParams == 0) || ParametersToBinaryImage( parameters, *pBinParams, pOutput ))
            ? S_OK : E_FAIL);
  }
  return S_FALSE;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function fills out the binary image of parameter structure by default settings.

  \param  parameters  parameter structure that can be modified on output.
  \param  pBinParams  pointer to the binary image of modified parameters.
  \param  pOutput     pointer to the object that prints a warning or an error message.
  \return             Ok = S_OK. */
//-------------------------------------------------------------------------------------------------
template< class PARAMETERS >
HRESULT SetDefaultParameters( PARAMETERS   & parameters,
                              Int8Arr      * pBinParams,
                              IDebugOutput * pOutput )
{
  parameters.set_default();
  return (((pBinParams == 0) || ParametersToBinaryImage( parameters, *pBinParams, pOutput ))
          ? S_OK : E_FAIL);
}


//-------------------------------------------------------------------------------------------------
/** \brief Function loads parameters from a binary file.

  \param  filename    the name of parameter file.
  \param  parameters  reference to the parameter structure.
  \param  pOutput     pointer to the object that prints a warning or an error message.
  \return             Ok = true. */
//-------------------------------------------------------------------------------------------------
template< class PARAMETERS >
bool LoadParameters( LPCWSTR        filename,
                     PARAMETERS   & parameters,
                     IDebugOutput * pOutput )
{
  Int8Arr params;
  if (alib::LoadArray( filename, params ))
  {
    if (csutility::BinaryImageToParameters( params, parameters, pOutput ))
      return true;
  }
  else if (pOutput != 0)
  {
    pOutput->PrintW( IDebugOutput::mt_error, L"Failed to load parameters from a file" );
  }
  return false;
}


//-------------------------------------------------------------------------------------------------
/** \brief Function saves parameters to a binary file.

  \param  filename    the name of parameter file.
  \param  parameters  reference to the parameter structure.
  \param  pOutput     pointer to the object that prints a warning or an error message.
  \return             Ok = true. */
//-------------------------------------------------------------------------------------------------
template< class PARAMETERS >
bool SaveParameters( LPCWSTR            filename,
                     const PARAMETERS & parameters,
                     IDebugOutput     * pOutput )
{
  Int8Arr params;
  if (csutility::ParametersToBinaryImage( parameters, params, pOutput ))
  {
    if (alib::SaveArray( filename, params ))
    {
      return true;
    }
    else if (pOutput != 0)
    {
      pOutput->PrintW( IDebugOutput::mt_error, L"Failed to save parameters to a file" );
    }
  }
  return false;
}

} // namespace csutility

