/****************************************************************************
  BaseChairEx.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : Albert.Akhriev@biones.com
****************************************************************************/

#pragma once

//=================================================================================================
/** \struct BaseChairEx.
    \brief  Extended chair structure that contains some characteristics computed off-line. */
//=================================================================================================
struct BaseChairEx : public BaseChair
{
  Vec2i    center;        //!< the central point of chair
  Vec2iArr points;        //!< internal points of a chair
  HRegion  region;        //!< horizontal scan region that covers this chair
  ARect    rect;          //!< rectangle that bounds this chair
  ARect    correctedRect; //!< slightly corrected bounding rectangle of this chair

  /** \brief Function clears this object. */
  void clear()
  {
    BaseChair::clear();
    center.set(0,0);
    points.clear();
    region.clear( true );
    rect.set_null();
    correctedRect.set_null();
  }

  /** \brief Constructor. */
  BaseChairEx()
  {
    clear();
  }

  /** \brief Destructor. */
  virtual ~BaseChairEx()
  {
  }

  BaseChairEx & operator=( const BaseChair & chair )
  {
    clear();
    *((BaseChair*)this) = chair;
    return (*this);
  }
};

typedef  std::vector<BaseChairEx>  BaseChairExArr;

