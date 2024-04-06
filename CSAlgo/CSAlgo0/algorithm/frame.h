/****************************************************************************
  frame.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \struct Frame.
    \brief  Structure extends two-dimensional array in a certain way. */
//=================================================================================================
struct Frame : public Arr2f
{
  float noise;          //!< noise deviation

  /** \brief Function clears this object. */
  void clear()
  {
    Arr2D<float,double>::clear();
    noise = 0.0f;
  }

  /** \brief Function clears this object without memory deallocation. */
  void reset()
  {
    std::fill( begin(), end(), 0.0f );
    noise = 0.0f;
  }

  /** \brief Constructor. */
  Frame()
  {
    clear();
  }

  /** \brief Function swaps contents of two frames. */
  void swap( Frame & frame )
  {
    Arr2D<float,double>::swap( frame );
    std::swap( noise, frame.noise );
  }
};

typedef  std::vector<Frame>  FrameArr;

