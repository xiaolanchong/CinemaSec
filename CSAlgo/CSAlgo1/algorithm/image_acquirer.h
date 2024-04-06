/****************************************************************************
  image_acquirer.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "frame.h"
#include "image_property.h"

//=================================================================================================
/** \class ImageAcquirer.
    \brief Class obtaines raw bitmap as input, estimates some image characteristics (noise, etc.),
           optionally smoothes the image and converts it into 'Arr2f' object. */
//=================================================================================================
class ImageAcquirer
{
  enum { INVALID_CONTENT = 0, VALID_CONTENT }; // frame state flags

protected:
  Frame         m_frame;          //!< the storage of current frame
  int           m_frameFlag;      //!< state flag of frame storage
  ImageProperty m_property;       //!< some characteristics of frame
  float         m_meanIntensity;  //!< mean intensity of frame that is being updated by time averaging
  int           m_nDroppedFrames; //!< the number of dropped frames
  int           m_nPassedFrames;  //!< the number of passed frames
  bool          m_bDropFrame;     //!< if non-zero, the the current frame must be discarded
  UByteArr      m_tempBuffer;     //!< temporal buffer

public:
  ImageAcquirer();
  virtual ~ImageAcquirer();

  virtual void Clear();
  virtual bool Init( int frameWidth, int frameHeight );
  virtual bool Acquire( LPCDIB pDib, LPCUBYTE pImg, const csalgo::ImageAcquireParams & param );

  static float ProcessNoiseHistogram( int * histogram, const int HISTO_SIZE, const double SCALE );
  static bool  GaugeAndConvertImage( LPCDIB pDib, LPCUBYTE pImg, Frame & frame, ImageProperty & property );
  static void  RepeatedlySmoothImage( Arr2f & image, int nRepeat, UByteArr & tempBuffer );

  const Frame & GetFrame() const;
  void          SwapFrame( Frame & exFrame );
  float         GetNoiseDeviation() const { return m_property.noise; } 
  int           DroppedFrameNumber() const { return m_nDroppedFrames; }
};

