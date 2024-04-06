/****************************************************************************
  chair_ex.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

#include "../../CSAlgo/algorithm/algo_types.h"
#include "../../CSAlgo/algorithm/base_chair.h"
#include "../../CSAlgo/algorithm/chair_state_data.h"
#include "../../CSAlgo/algorithm/chair.h"

//=================================================================================================
/** \struct ChairEx.
    \brief  Extended chair structure contains some computed off-line
            characteristics and temporal state variables. */
//=================================================================================================
struct ChairEx : public Chair
{
  struct Statistics
  {
    int   dynaPeriodNum;
    int   calmPeriodNum;
    float dynaDuration;
    float calmDuration;
    float startTime;
    float charge;
    bool  activity;

    void clear()
    {
      dynaPeriodNum = 0;
      calmPeriodNum = 0;
      dynaDuration = 0;
      calmDuration = 0;
      startTime = 0;
      charge = 0;
      activity = false;
    }

    Statistics() { clear(); }
  };

  typedef  Arr2D<Statistics,Statistics>  StatMap;

  int     type;          //!< the type of an object that occupies this chair (human, clothes, empty, etc.)
  float   typeStartTime; //!< the beginning time of occupation by an object of given type
  StatMap statistics;    //!< statistics of activity for different parameter settings

  /** \brief Function clears temporal, state data. */
  void clear_state_data()
  {
    ChairStateData::clear_state_data();
    type = CHAIR_UNDEFINED;
    typeStartTime = 0;
    statistics.clear();
  }

  /** \brief Constructor. */
  ChairEx() : Chair() { clear_state_data(); }
};

typedef  std::vector<ChairEx>  ChairExArr;

