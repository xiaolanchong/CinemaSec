/****************************************************************************
  csutility.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

#ifdef CS_UTILITY_USER_DLL
  #define CS_UTIL_API __declspec(dllexport)
#else
  #define CS_UTIL_API __declspec(dllimport)
#endif

#include "../CSInterfaces/data_transfer_interface.h"

#include "alib/alib.h"
#include "alib/matrixlib.h"
#include "alib/vl_curve2d_lib.h" 
#include "alib/vl_gray_edge_detector.h"
#include "alib/vl_horz_region.h"
#include "alib/vl_vert_region.h"
#include "alib/parameter_ex.h"
#include "alib/paramlib_dlg.h"
#include "alib/noiselib.h"

#include "utility/text_constants.h"
#include "utility/constants.h"
#include "utility/types.h"
#include "utility/inline_functions.h"
#include "utility/digits.h"
#include "utility/layout.h"
#include "utility/long_timer.h"
#include "utility/live_snake.h"
//>>>>>
#include "utility/base_frame_grabber.h"
//>>>>>
#include "utility/debug_interface.h"
#include "utility/frame_grabber_interface.h"
#include "utility/serialization.h"
#include "utility/multi_histogram.h"
#include "utility/MultiHistogramEx.h"
#include "utility/LinearHistogramAdapter.h"
#include "utility/LogBimodalHistogramAdapter.h"
#include "utility/ParameterFunctions.h"
#include "utility/auto_interface_ptr.h"
#include "utility/BilinearCoefs.h"
#include "utility/SampleDistance.h"
#include "alib/console_paint_interface.h"
#include "utility/triangle_storage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Global Function.
///////////////////////////////////////////////////////////////////////////////////////////////////

//>>>>> OBSOLETE
// helper.cpp:
CS_UTIL_API bool   IsGrayOrYUY2( const BITMAPINFOHEADER & h );
CS_UTIL_API void   MyGetFilePath( const StdStr & source, StdStr & result );
CS_UTIL_API StdStr MyMakeFileNameWithNewExtension( LPCWSTR source, LPCWSTR newExt );
CS_UTIL_API StdStr MyConstructDerivedFileName( LPCWSTR source, LPCWSTR label, LPCWSTR ext );
CS_UTIL_API StdStr MyConstructOfflineFileName( LPCWSTR source, LPCWSTR label, LPCWSTR ext );
CS_UTIL_API void   MyAddTrailingBackslashToPathName( StdStr & path );
CS_UTIL_API int    MyGetDibScanSizeInBytes( const BITMAPINFOHEADER * p );
CS_UTIL_API int    MyGetDibContentsSizeInBytes( const BITMAPINFOHEADER * p );
CS_UTIL_API void   MyAdjustPositiveFloatImageToByteRange( Arr2f & image );
CS_UTIL_API bool   MyGrayImageToQImage( LPCBITMAPINFO pHeader, LPCUBYTE pImage, int imgSize, QImage * pResult );
CS_UTIL_API void   MyFloatImageToColorImage( const Arr2f & source, QImage & result );
CS_UTIL_API void   MyFloatImageToByteImage( const Arr2f & source, Arr2ub & result, int scale = 1, bool bColor16 = true );
CS_UTIL_API void   MyDrawGrayImage( const Arr2ub & image, HDC hdc, CRect rect, bool bInvertY, bool bColor16 = false );
CS_UTIL_API void   MyDrawColorImage( const QImage & image, HDC hdc, CRect rect, bool bInvertY );
CS_UTIL_API void   MyInitColorGrayPalette( RGBQUAD * palette, int size, bool bColor16 = true );
CS_UTIL_API void   MyAdjustLowestValuesToAllowSystemColors( Arr2ub & image );
CS_UTIL_API int    MyDrawDigitInQImage( double value, int x, int y, int precision, QImage & image, COLORREF textColor, COLORREF backColor = 0xFFFFFFFF );
//>>>>> OBSOLETE

extern "C"
{

// byte_bitmap.cpp:
CS_UTIL_API bool LoadByteImage( LPCWSTR fname, Arr2ub & image, bool bInvertY );
CS_UTIL_API bool SaveByteImage( LPCWSTR fname, const Arr2ub & image, bool bInvertY, bool bColor16 = true );

// float_bitmap.cpp:
CS_UTIL_API bool LoadFloatImage( LPCWSTR fname, Arr2f & image, bool bInvertY );
CS_UTIL_API bool SaveFloatImage( LPCWSTR fname, const Arr2f & image, bool bInvertY );

// io_background.cpp
CS_UTIL_API bool LoadBackgroundImage( LPCWSTR fileName, Arr2f * background, int nSubFrames );
CS_UTIL_API bool SaveBackgroundImage( LPCWSTR fileName, const FltImgArr & backgrounds );

} // extern "C"


namespace alib
{
/*
  struct CS_UTIL_API IMessage : public IDebugOutput
  {
    //virtual void Print( __int32 messageType, LPCWSTR szMessage );
    virtual void PrintW( __int32 messageType, const wchar_t * szMessage );
    virtual void PrintA( __int32 messageType, const char * szMessage );
  };
*/
  typedef  IDebugOutput  IMessage;

  const csinterface::DataTypeId DATAID_STATE_FLAG = csinterface::DATAID_STATE_FLAG;

  typedef  csinterface::TDataWraper<__int32,DATAID_STATE_FLAG>  TStateFlag;
  typedef  csinterface::IDataType                               IDataType;
  typedef  std::runtime_error                                   GeneralError;

} // namespace alib

#include "alib/video_interface.h"

namespace avideolib
{
  extern "C"
  {
    // alib/avideo_reader.cpp:
    avideolib::IVideoReader * CreateAVideoReader( const WCHAR    * name,
                                                  alib::IMessage * pOutput,
                                                  bool             bInvertY );

    // alib/avideo_writer.cpp:
    avideolib::IVideoWriter * CreateAVideoWriter( const WCHAR    * name,
                                                  alib::IMessage * pOutput,
                                                  bool             bTrueColor,
                                                  bool             bInvertY );
  } // extern "C"
} // namespace avideolib


namespace csutility
{
  extern "C"
  {
    //>>>>> Obsolete.
    bool GrayImageToQImage( LPCBITMAPINFO pHeader, LPCUBYTE pBytes, Arr2ub * pImage );
    //>>>>>

    bool IsGrayOrYUY2( const BITMAPINFOHEADER * p );

    void InitColorGrayPalette( RGBQUAD * palette, int size, bool bColor16 );

    int GetDibScanSizeInBytes( const BITMAPINFOHEADER * p );

    int GetDibContentsSizeInBytes( const BITMAPINFOHEADER * p );

    void CopyDIBToColorImage( LPCBITMAPINFO pHeader, LPCUBYTE pBytes, QImage * pImage );

    void CopyDIBToGrayImage( LPCBITMAPINFO pHeader, LPCUBYTE pBytes, Arr2ub * pImage );

    void CopyDIBToFloatImage( LPCBITMAPINFO pHeader, LPCUBYTE pBytes, Arr2f * pImage );

    void CopyGrayImageToQImage( const Arr2ub * pSource, QImage * pImage );

    void CopyFloatImageToColorImage( const Arr2f * pSource, QImage * pImage );

    void DrawColorImage( const QImage * pImage, HDC hdc, CRect rect );

    void DrawGrayImage( const Arr2ub * pImage, HDC hdc, CRect rect, bool bInvertY, bool bColor16 );

    void DrawDigits5x10( const char * text, int x, int y, bool bCenter,
                         QImage * pImage, COLORREF textColor, COLORREF backColor );

    void AddTrailingBackslashToPathName( std::wstring * pPath );

    void RepeatedlySmoothImage( Arr2f * pImage, int nRepeat, UByteArr * pTempBuffer );

    bool SaveQImage( LPCWSTR fname, const QImage * pImage, bool bInvertY );

    // CorrectBrightness.cpp:
    bool CorrectBrightness( const Arr2f * pSource, int winDim, QImage * pResult, UByteArr * pTempBuffer );
    bool CorrectByteBrightness( const Arr2ub * pSource, int winDim, Arr2ub * pResult, UByteArr * pTempBuffer );

    // ScheduleTestVideo.cpp:
    int ShouldTestVideoBeWritten( LPCWSTR dllName, int cameraNo );

    // WrappedAVideoReader.cpp:
    BaseFrameGrabberEx * CreateWrappedAVideoReader( LPCWSTR name, alib::IMessage * pOutput );

    // float_bitmap_ex.cpp:
    bool LoadFloatImageEx( LPCWSTR fname, Arr2f * pImage, bool doDownwardAxisY );
    bool SaveFloatImageEx( LPCWSTR fname, const Arr2f * pImage, bool doDownwardAxisY );

    // byte_bitmap_ex.cpp:
    bool LoadByteImageEx( LPCWSTR fname, Arr2ub * pImage, bool bInvertY );
    bool SaveByteImageEx( LPCWSTR fname, const Arr2ub * pImage, bool bInvertY, bool bColor16 );

  } // extern "C"

} // namespace csutility

