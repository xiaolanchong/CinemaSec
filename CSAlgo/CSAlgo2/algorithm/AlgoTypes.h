///////////////////////////////////////////////////////////////////////////////////////////////////
// AlgoTypes.h
// ---------------------
// begin     : Aug 2004
// modified  : 28 Oct 2005
// author(s) : Albert Akhriev
// email     : Albert.Akhriev@biones.com
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DynamicAccumulator.h"

namespace csalgo2
{

typedef  std::vector<Arr2f>  Image2fSet;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Structure keeps information about static interval of a chair.
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Interval
{
  ////////////////////////////////////////////////////////////////////////////
  /// \brief Structure keeps the measure of activity within a static interval.
  ////////////////////////////////////////////////////////////////////////////
  struct Activity : public MeanAccum<double>
  {
    float   duration;        //!< duration of activity interval in seconds
    __int32 bAboveThr;       //!< nonzero if activity measure is above threshold, otherwise zero
  };

  Activity feebleActivity;   //!< data of feeble activity
  Activity normalActivity;   //!< data of normal activity
  Activity strongActivity;   //!< data of strong activity
  float    startTime;        //!< start time of this interval in seconds
  float    stopTime;         //!< stop time of this interval in seconds
  float    charge;           //!< result of time integration (similar to charging RC-circuit)
  __int8   bFinished;        //!< nonzero if the interval has been completed
  __int8   bHumanDetected;   //!< nonzero if human has been detected at least once
  __int8   reserved[2];      //!< reserved for alignment
  __int32  frameNo[2];       //!< indices of start and stop frames

  ///////////////////////
  /// \brief Constructor.
  ///////////////////////
  Interval()
  {
    feebleActivity.reset();   feebleActivity.duration = 0.0f;   feebleActivity.bAboveThr = 0;
    normalActivity.reset();   normalActivity.duration = 0.0f;   normalActivity.bAboveThr = 0;
    strongActivity.reset();   strongActivity.duration = 0.0f;   strongActivity.bAboveThr = 0;

    startTime = 0;
    stopTime = 0;
    charge = 0;
    bFinished = 0;
    bHumanDetected = 0;
    reserved[0] = (reserved[1] = 0);
    frameNo[0] = (frameNo[1] = 0);
  }

  //////////////////////////////////////////////////////////////
  /// \brief Function serializes this object in a memory buffer.
  //////////////////////////////////////////////////////////////
  __int8 * serialize( __int8 * p, csutility::SerializeAction action )
  {
    return csutility::Serialize( p, (*this), action );
  }

  //////////////////////////////////////////////////////////////////
  /// \brief Function gives duration of this interval at the moment.
  //////////////////////////////////////////////////////////////////
  float duration() const
  {
    return (stopTime - startTime);
  }
};

typedef  std::list<Interval>  IntervalLst;


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Structure reflects the current state of a chair.
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ChairStateData
{
  bool        bOccupied;      //!< nonzero if a chair is currently occupied by an object
  bool        bHardOccupied;  //!< nonzero if a chair is currently persistently occupied by an object
  bool        bHuman;         //!< nonzero if a human has been identified within a chair
  float       humanity;       //!< the degree of how reliable is to qualify a chair as occupied by a human
  float       statEntity;     //!< some measure of correlation with static background
  float       dynaEntity;     //!< some measure that reflects dynamic properties of an object in a chair
  float       occupationTime; //!< the total period of chair occupation in seconds
  IntervalLst intervals;      //!< intervals of chair's occupation and activity

  //////////////////////////////////////////////////
  /// \brief Function swaps contents of two objects.
  //////////////////////////////////////////////////
  void swap_state_data( ChairStateData & x )
  {
    std::swap( bOccupied,      x.bOccupied      );
    std::swap( bHardOccupied,  x.bHardOccupied  );
    std::swap( bHuman,         x.bHuman         );
    std::swap( humanity,       x.humanity       );
    std::swap( statEntity,     x.statEntity     );
    std::swap( dynaEntity,     x.dynaEntity     );
    std::swap( occupationTime, x.occupationTime );
    intervals.swap( x.intervals );
  }

  ////////////////////////////////////////////////
  /// \brief Function clears temporal, state data.
  //////////////////////////////////////////////// 
  void clear_state_data()
  {
    bOccupied = false;
    bHardOccupied = false;
    bHuman = false;
    humanity = 0.0f;
    statEntity = 0.0f;
    dynaEntity = 0.0f;
    occupationTime = 0.0f;
    intervals.clear();
  }

  ///////////////////////
  /// \brief Constructor.
  /////////////////////// 
  ChairStateData()
  {
    clear_state_data();
  }

  //////////////////////
  /// \brief Destructor.
  ////////////////////// 
  virtual ~ChairStateData() {}
};

typedef  std::vector<ChairStateData>  ChairStateDataArr;


///////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Structure keeps state information of a chair. 
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Chair : public BaseChairEx, public csalgo2::ChairStateData
{
  DynamicAccumulator  dynaAccum;   //!< accumulator of dynamic information
  csalgo2::Image2fSet samples;     //!< the set of different empty-chair views
  Arr2f               weights;     //!< weights of chair points specified inside bounding rectangle

  ////////////////////////////////////////////////
  /// \brief Function clears temporal, state data.
  //////////////////////////////////////////////// 
  void clear_state_data()
  {
    csalgo2::ChairStateData::clear_state_data();
    dynaAccum.Reset();
  }

  //////////////////////////////////////////////////////////////
  /// \brief Function serializes this object in a memory buffer.
  ////////////////////////////////////////////////////////////// 
  __int8 * serialize( __int8 * p, csutility::SerializeAction action )
  {
    p = csutility::Serialize( p, cameraNo,      action );
    p = csutility::Serialize( p, index,         action );
    p = csutility::Serialize( p, bOccupied,     action );
    p = csutility::Serialize( p, bHardOccupied, action );
    p = csutility::Serialize( p, bHuman,        action );
    p = csutility::Serialize( p, humanity,      action );
    p = csutility::Serialize( p, statEntity,    action );
    p = csutility::Serialize( p, dynaEntity,    action );
    p = csutility::SerializeComplexContainer( p, intervals, action );
    return p;
  }

  ///////////////////////
  /// \brief Constructor.
  ///////////////////////
  Chair() : BaseChairEx(), csalgo2::ChairStateData(), dynaAccum(), samples(), weights()
  {
  }
};

typedef  std::vector<Chair>  ChairArr;

} // csalgo2

