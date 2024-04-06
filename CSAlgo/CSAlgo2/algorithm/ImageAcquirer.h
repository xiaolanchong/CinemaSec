/****************************************************************************
  ImageAcquirer.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

#include "FrameUpdator.h"

namespace csalgo2
{

//=================================================================================================
/** \struct ImageAcquireParams.
    \brief  ImageAcquireParams. */
//=================================================================================================
struct ImageAcquireParams : public paramlib::BaseParamSection
{
  paramlib::ChrParam  p_updateMethod;
  paramlib::FltParam  p_signal2noise;
  paramlib::FltParam  p_meanDiffThr;
  paramlib::FltParam  p_minMeanIntensity;
  paramlib::IntParam  p_nSmoothRepeat;
  //paramlib::IntParam  p_suppressBrightnessRange;

  explicit ImageAcquireParams( int & controlIdentifier );
};


//=================================================================================================
/** \class ImageAcquirer.
    \brief Class obtaines raw bitmap as input, estimates some image characteristics (noise, etc.),
           optionally smoothes the image and converts it into 'Arr2f' object. */
//=================================================================================================
class ImageAcquirer
{
protected:
  Arr2f                       m_frame;            //!< the latest frame
  csalgocommon::ImageProperty m_property;         //!< some characteristics of frame
  float                       m_meanDiffThr;      //!< threshold on relative difference of mean intensities of both half-frames
  float                       m_minMeanIntensity; //!< threshold on minimal mean intensity of a frame
  int                         m_droppedFrameNum;  //!< the number of dropped frames
  int                         m_frameNum;         //!< the total number of passed frames
  FrameUpdator                m_updator;          //!< frame updating object
  UByteArr                    m_tempBuffer;       //!< temporal buffer
  int                         m_brightnessThr;    //!< brightness threshold used to detect day/night
  int                         m_percentageThr;    //!< percentage threshold used to detect day/night
  int                         m_dayNightState;    //!< day/night state: -1 - undefined, 0 - night, 1 - day

protected:
  bool GaugeAndConvertImage( const BITMAPINFO * pDib, const ubyte * pImg );

public:
  ImageAcquirer();
  virtual ~ImageAcquirer();

  void Clear();
  void Initialize( int width, int height, const csalgo2::ImageAcquireParams & params,
                   int brightnessThreshold = 0, int percentageThreshold = 0 );
  bool Acquire( const BITMAPINFO * pDib, const ubyte * pImg, std::ostream * pLog = 0 );

  const Arr2f & GetRawFrame() const { return m_frame; }
  const Arr2f & GetSmoothImage() const { return (m_updator.Image()); }
  void          SwapFrame( Arr2f & f ) { f.swap( m_frame ); }
  float         GetNoiseDeviation() const { return m_property.noise; } 
  int           DroppedFrameNumber() const { return m_droppedFrameNum; }
  int           GetPassedFrameNumber() const { return m_frameNum; }
  int           GetDayNightState() const { return m_dayNightState; }

  const csalgocommon::ImageProperty & GetImageProperties() const { return m_property; }
};

} // csalgo2

