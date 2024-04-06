/****************************************************************************
  parameter_dialog.h
  ---------------------
  begin     : Aug 2004
  author(s) : A.Akhriev, E.Gorbachev, K.Lyadvinsky
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

extern "C"
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function shows dialog window for editing of algorithm parameters.

  \param  hParentWnd  the parent window handle.
  \param  pBinParams  in: pointer to the binary image of input parameters, out: modified parameters.
  \param  pOutput     pointer to the object that prints a warning or an error message.
  \return             Ok = S_OK, cancel = S_FALSE, otherwise error code E_xxx. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT ShowParameterDialog( IN HWND           hParentWnd,
                             IN OUT Int8Arr  * pBinParams,
                             IN IDebugOutput * pOutput );


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function fills out the binary image of algorithm parameters by default settings.

  \param  pBinParams  out: pointer to the binary image of modified parameters.
  \param  pOutput     pointer to the object that prints a warning or an error message.
  \return             Ok = S_OK, otherwise error code E_xxx. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT SetDefaultParameters( IN OUT Int8Arr  * pBinParams,
                              IN IDebugOutput * pOutput );


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
                               IN IDebugOutput * pOutput );


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function fills out the binary image of algorithm parameters by default settings.

  \param  pBinParams   out: pointer to the binary image of modified parameters.
  \param  bSaveInFile  if nonzero, then save parameters in a file, whose name is derived from a library full-path name.
  \param  pOutput      pointer to the object that prints a warning or an error message.
  \return              Ok = S_OK, otherwise error code E_xxx. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT SetDefaultParametersEx( OUT Int8Arr     * pBinParams,
                                IN bool           bSaveInFile,
                                IN IDebugOutput * pOutput );


///////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Function fills out the binary image of algorithm parameters by the current settings,
           which are stored in the library's local variables.

  \param  pBinParams  out: pointer to the binary image of modified parameters.
  \param  pOutput     in: pointer to the object that prints a warning or an error message.
  \return             Ok = S_OK, otherwise error code E_xxx. */
///////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT GetParameters( OUT Int8Arr     * pBinParams,
                       IN IDebugOutput * pOutput );

} // extern "C"

