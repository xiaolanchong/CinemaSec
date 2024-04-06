/****************************************************************************
  chair.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//#include "algo_types.h"
//#include "base_chair.h"
#include "chair_state_data.h"

//=================================================================================================
/** \struct Chair.
    \brief  Extended chair structure that contains some computed off-line
            characteristics and temporal state variables. */
//=================================================================================================
struct Chair : public BaseChair, public ChairStateData
{
  Vec2i         center;        //!< the central point of chair
  Vec2i         correction;    //!< small additional offset between actual and observed chair's position
  ChairPointArr points;        //!< internal points of a chair
  HRegion       region;        //!< horizontal scan region that covers this chair
  ARect         rect;          //!< rectangle that bounds this chair
  bool          bPosCorrected; //!< non-zero, if chair's position has been corrected (see 'correction' field)
  bool          bTextureLess;  //!< non-zero, if chair's texture is less than a multiple of noise deviation

  /** \brief Function clears temporal, state data. */
  void clear_state_data()
  {
    ChairStateData::clear_state_data();
    correction.set(0,0);
    bPosCorrected = false;
    bTextureLess = false;
  }

  /** \brief Function serializes this object in a memory buffer. */
  __int8 * serialize( __int8 * p, csutility::SerializeAction action )
  {
    p = csutility::Serialize( p, cameraNo,      action );
    p = csutility::Serialize( p, index,         action );
    p = csutility::Serialize( p, bOccupied,     action );
    p = csutility::Serialize( p, bHardOccupied, action );
    p = csutility::Serialize( p, bMovement,     action );
    p = csutility::Serialize( p, bHardMovement, action );
    p = csutility::Serialize( p, bHuman,        action );
    p = csutility::Serialize( p, humanity,      action );
    p = csutility::Serialize( p, statNCC,       action );
    p = csutility::Serialize( p, dynaNCC,       action );
    p = csutility::SerializeComplexContainer( p, staticIntervals, action );
    return p;
  }

  /** \brief Constructor. */
  Chair()
  {
    center.set(0,0);
    correction.set(0,0);
    points.clear();
    region.clear( true );
    rect.set(0,0,0,0);
    bPosCorrected = false;
    bTextureLess = false;
  }
};

typedef  std::vector<Chair>  ChairArr;

