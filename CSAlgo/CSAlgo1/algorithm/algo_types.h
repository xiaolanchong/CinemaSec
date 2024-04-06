/****************************************************************************
  algo_types.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \struct Interval.
    \brief  Structure keeps information about the time period of some events inside a chair. */
//=================================================================================================
struct Interval
{
  float startTime; //!< start time of this interval in seconds
  float stopTime;  //!< stop time of this interval in seconds
  float charge;    //!< temporal variable used to overcome small time gaps (similar to charging RC-circuit)
  bool  bReady;    //!< non-zero if the interval has been completed

  /** \brief Constructor. */
  Interval()
  {
    startTime = 0;
    stopTime = 0;
    charge = 0;
    bReady = false;
  }

  /** \brief Function serializes this object in a memory buffer. */
  __int8 * serialize( __int8 * p, csutility::SerializeAction action )
  {
    return csutility::Serialize( p, (*this), action );
  }

  /** \brief Function gives duration of this interval at the moment. */
  float duration() const
  {
    return (stopTime - startTime);
  }
};

typedef  std::list<Interval>  IntervalLst;


//=================================================================================================
/** \struct StaticInterval.
    \brief  Structure keeps an interval of chair occupation
            along with the list of intervals of human activity. */
//=================================================================================================
struct StaticInterval : public Interval
{
  IntervalLst dynamicIntervals;   //!< the list of intervals of human activity

  /** \brief Constructor. */
  StaticInterval()
  {
    dynamicIntervals.clear();
  }

  /** \brief Function serializes this object in a memory buffer. */
  __int8 * serialize( __int8 * p, csutility::SerializeAction action )
  {
    p = Interval::serialize( p, action );
    p = csutility::SerializeComplexContainer( p, dynamicIntervals, action );
    return p;
  }
};

typedef  std::list<StaticInterval>  StaticIntervalLst;


//=================================================================================================
/** \struct ChairPoint.
    \brief  The point that belongs to chair's internal region and supplementary information. */
//=================================================================================================
struct ChairPoint : public Vec2i
{
  float statNCC;   //!< NCC between the current frame and the static background at this point
  float dynaNCC;   //!< NCC between the current frame and the dynamic background at this point

  /** \brief Constructor. */
  ChairPoint() : statNCC(0), dynaNCC(0) {}
};

typedef  std::vector<ChairPoint>  ChairPointArr;


//=================================================================================================
/** \struct ChairLearn.
    \brief  Structure is used to transfer data needed while learning from chair's states. */
//=================================================================================================
struct ChairLearn
{
  __int32 cameraNo;     //!< the index of camera that covers control area occupied by this chair
  __int32 index;        //!< unique identifier of this chair
  float   statNCC;      //!< the current NCC between the current frame and static background
  float   dynaNCC;      //!< the current NCC between the current frame and dynamic background

  ChairLearn() : cameraNo(-1), index(-1), statNCC(0), dynaNCC(0) {}
};

