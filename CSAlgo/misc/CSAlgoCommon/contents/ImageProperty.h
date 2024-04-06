/****************************************************************************
  ImageProperty.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

namespace csalgocommon
{

//=================================================================================================
/** \struct ImageProperty.
    \brief  Structure keeps some image characteristics,
            where the image itself is considered as two interlaced halves. */
//=================================================================================================
struct ImageProperty
{
  float noiseDeviation[2];   //!< noise deviations of both half-frames
  float imgMean[2];          //!< image means of both half-frames
  float imgDeviation[2];     //!< image deviations of both half-frames
  float noise;               //!< the overall image noise deviation

  /** \brief Function clears this object. */
  void clear()
  {
    noiseDeviation[0] = (imgMean[0] = (imgDeviation[0] = 0.0f));
    noiseDeviation[1] = (imgMean[1] = (imgDeviation[1] = 0.0f));
    noise = 0.0f;
  }

  /** \brief Constructor. */
  ImageProperty()
  {
    clear();
  }
};

} // csalgocommon

