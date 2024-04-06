/****************************************************************************
  CSAlgo2.h
  ---------------------
  begin     : Aug 2004
  modified  : 12 Sep 2005
  author(s) : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

extern "C"
{

HRESULT CreateBackgroundAccumulator( IBackgroundAccumulator ** ppAccumulator );

HRESULT CreateCameraAnalyzer( ICameraAnalyzer   ** ppAnalyzer,
                              const Int8Arr      * pParameters,
                              const BaseChairArr * pChairs,
                              const Arr2f        * pBackground,
                              __int32              cameraNo,
                              IDebugOutput       * pOutput );

HRESULT CreateHallAnalyzer( IHallAnalyzer     ** ppAnalyzer,
                            const Int8Arr      * pParameters,
                            const BaseChairArr * pChairs,
                            __int64              startTime,
                            IDebugOutput       * pOutput );

HRESULT ShowParameterDialog( HWND           hParentWnd,
                             Int8Arr      * pBinParams,
                             IDebugOutput * pOutput );

HRESULT SetDefaultParameters( Int8Arr      * pBinParams,
                              IDebugOutput * pOutput );

HRESULT ShowParameterDialogEx( HWND           hParentWnd,
                               Int8Arr      * pBinParams,
                               bool           bSaveInFile,
                               IDebugOutput * pOutput );

HRESULT SetDefaultParametersEx( Int8Arr      * pBinParams,
                                bool           bSaveInFile,
                                IDebugOutput * pOutput );

HRESULT GetParameters( Int8Arr      * pBinParams,
                       IDebugOutput * pOutput );

bool LoadParameters( LPCWSTR        filename,
                     IDebugOutput * pOutput );

bool SaveParameters( LPCWSTR        filename,
                     IDebugOutput * pOutput );

} // extern "C"

