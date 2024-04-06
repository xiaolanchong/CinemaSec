/******************************************************************************
  avideo_header.h
  ---------------------
  begin     : Apr 2005
  modified  : 29 Jul 2005
  author(s) : Albert Akhriev
  copyright : Albert Akhriev
  email     : aaah@mail.ru, Albert.Akhriev@biones.com
******************************************************************************/

#pragma once

namespace avideolib
{

const __int32 AVIDEO_VERSION = alib::MakeFourCC( 'A', 'V', 'D', '0' );

const WCHAR WRONG_HEADER[] = _T("Wrong header size");
const WCHAR WRONG_VERSION[] = _T("Wrong file version");
const WCHAR CORRUPTED_FILE[] = _T("Apparently corrupted file");
const WCHAR DIFFERENT_SIZES[] = _T("All frames must have the same dimensions");

//=================================================================================================
/** \struct VideoHeader.
    \brief  VideoHeader. */
//=================================================================================================
struct VideoHeader
{
  __int32 thisSize;         //!< size of this structure
  __int32 version;          //!< file version
  __int32 width;            //!< frame width
  __int32 height;           //!< frame height
  __int32 frameNum;         //!< number of frames
  __int32 colorScheme;      //!< color scheme being used
  __int32 fps1000;          //!< frames-per-second * 1000
  __int32 reserved[64-7];   //!< reserved
  __int8  text[128];        //!< some text information

  VideoHeader()
  {
    memset( this, 0, sizeof(VideoHeader) );
    thisSize = sizeof(VideoHeader);
    ASSERT( thisSize == (64*4+128) );
  }

  int BytesPerPixel() const
  {
    switch (colorScheme)
    {
      case avideolib::IBaseVideo::GRAYSCALED       : return (int)(sizeof(ubyte));
      case avideolib::IBaseVideo::GRAY_PLUS_COLOR16: return (int)(sizeof(ubyte));
      case avideolib::IBaseVideo::TRUECOLOR        : return (int)(3*sizeof(ubyte));
    }
    ASSERT(0);
    return 0;
  }
};


//=================================================================================================
/** \struct FrameHeader.
    \brief  Per frame header. */
//=================================================================================================
struct FrameHeader
{
  __int32 thisSize;          //!< size of this structure
  __int32 time;              //!< time in milliseconds counted off from the first frame
  __int32 index;             //!< frame index
  __int32 extraDataSize;     //!< size of extra data attached to a frame
  __int32 reserved[3];       //!< reserved

  FrameHeader()
  {
    memset( this, 0, sizeof(FrameHeader) );
    thisSize = sizeof(FrameHeader);
    ASSERT( thisSize == (7*4) );
  }
};

} // namespace avideolib

