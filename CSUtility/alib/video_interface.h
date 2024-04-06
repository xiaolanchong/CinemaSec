/******************************************************************************
  video_interface.h
  ---------------------
  begin     : Apr 2005
  modified  : 24 Jul 2005
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
******************************************************************************/

#pragma once

namespace avideolib
{

//=================================================================================================
/** \struct IBaseVideo.
    \brief  Base class of any frame grabber. */
//=================================================================================================
struct IBaseVideo
{

enum DataOrState
{
  LOOPBACK_ON  = (1<<0),
  LOOPBACK_OFF = (1<<1),
  VERSION      = (1<<2),
  WIDTH        = (1<<3),
  HEIGHT       = (1<<4),
  FRAME_NUMBER = (1<<5),
  COLORSCHEME  = (1<<6),
  FPS_1000     = (1<<7),
};

enum ColorScheme
{
  GRAYSCALED = 0,
  GRAY_PLUS_COLOR16,
  TRUECOLOR
};


//-------------------------------------------------------------------------------------------------
/** \brief Virtual destructor ensures proper object destruction. */
//-------------------------------------------------------------------------------------------------
virtual ~IBaseVideo() {}


//-------------------------------------------------------------------------------------------------
/** \brief Function uploads specified data to this object.

  \param  pData  pointer to the input data storage.
  \return        Ok = true. */
//-------------------------------------------------------------------------------------------------
virtual bool SetData( const alib::IDataType * pData ) = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function unloads specified data from this object.

  \param  pData  pointer to the output data storage.
  \return        Ok = true. */
//-------------------------------------------------------------------------------------------------
virtual bool GetData( alib::IDataType * pData ) const = 0;

};


//=================================================================================================
/** \struct IVideoReader.
    \brief  Base class of any reading frame grabber.

  Working scenario:
  <tt><pre>
  IVideoReader * pReader = CreateDesiredReader(...);
  for (bool bContinue = true; bContinue;)
  {
    ...
    switch (pReader->ReadFrame(...))
    {
      case NORMAL        : DoUsefulJob(...);  break;
      case NO_FRESH_FRAME: break;
      case END_IS_REACHED: bContinue = false;
      case GENERAL_ERROR : CallErrorHandler(...);  bContinue = false;  break;
    }
    ...
  }
  delete pReader;
  pReader = 0;
  </pre></tt> */
//=================================================================================================
struct IVideoReader : public avideolib::IBaseVideo
{

enum ReturnCode
{
  NORMAL = 0,
  NO_FRESH_FRAME = 1,
  END_IS_REACHED = 2,
  GENERAL_ERROR = -1
};

//-------------------------------------------------------------------------------------------------
/** \brief Function reads the next frame of a sequence or specified frame,
           if parameter 'desiredNo' >= 0.

  \param  pImage      pointer to the destination image.
  \param  desiredNo   nonnegative value means the index of desired frame, should be -1 by default.
  \param  pFrameNo    pointer to the storage that receives frame index or 0.
  \param  pMilliTime  pointer to the storage that receives frame time in milliseconds or 0.
  \return             any value of 'ReturnCode' type. */
//-------------------------------------------------------------------------------------------------
virtual ReturnCode ReadFrame( Arr2ub  * pImage,
                              __int32   desiredNo,
                              __int32 * pFrameNo,
                              __int32 * pMilliTime ) = 0;

//-------------------------------------------------------------------------------------------------
/** \brief Function reads the next frame of a sequence or specified frame,
           if parameter 'desiredNo' >= 0.

  \param  pImage      pointer to the destination image.
  \param  desiredNo   nonnegative value means the index of desired frame, should be -1 by default.
  \param  pFrameNo    pointer to the storage that receives frame index or 0.
  \param  pMilliTime  pointer to the storage that receives frame time in milliseconds or 0.
  \return             any value of 'ReturnCode' type. */
//-------------------------------------------------------------------------------------------------
virtual ReturnCode ReadFrame( AImage  * pImage,
                              __int32   desiredNo,
                              __int32 * pFrameNo,
                              __int32 * pMilliTime ) = 0;

//-------------------------------------------------------------------------------------------------
/** \brief Function sets file pointer to the beginning of specified frame.

  \param  frameNo  index of desired frame.
  \return          Ok = true. */
//-------------------------------------------------------------------------------------------------
virtual bool Seek( __int32 frameNo ) = 0;

};


//=================================================================================================
/** \struct IVideoWriter.
    \brief  Base class of any writing frame grabber.

  Working scenario:
  <tt><pre>
  IVideoWriter * pWriter = CreateDesiredWriter(...);
  for (;;)
  {
    ...
    if (!(pWriter->WriteFrame(...)))
    {
      CallErrorHandler(...);
      break;
    }
    ...
  }
  delete pWriter;
  pWriter = 0;
  </pre></tt> */
//=================================================================================================
struct IVideoWriter : public avideolib::IBaseVideo
{

//-------------------------------------------------------------------------------------------------
/** \brief Function writes the next frame of a sequence.

  \param  pImage     pointer to the source image.
  \param  milliTime  frame time in milliseconds.
  \return            Ok = true. */
//-------------------------------------------------------------------------------------------------
virtual bool WriteFrame( const Arr2ub * pImage, unsigned __int32 milliTime ) = 0;


//-------------------------------------------------------------------------------------------------
/** \brief Function writes the next frame of a sequence.

  \param  pImage     pointer to the source image.
  \param  milliTime  frame time in milliseconds.
  \return            Ok = true. */
//-------------------------------------------------------------------------------------------------
virtual bool WriteFrame( const AImage * pImage, unsigned __int32 milliTime ) = 0;

};

} // namespace avideolib

